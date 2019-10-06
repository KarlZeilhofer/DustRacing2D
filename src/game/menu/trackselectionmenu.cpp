// This file is part of Dust Racing 2D.
// Copyright (C) 2015 Jussi Lind <jussi.lind@iki.fi>
//
// Dust Racing 2D is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// Dust Racing 2D is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Dust Racing 2D. If not, see <http://www.gnu.org/licenses/>.

#include "trackselectionmenu.hpp"

#include "game.hpp"
#include "mainmenu.hpp"
#include "scene.hpp"
#include "settings.hpp"
#include "renderer.hpp"
#include "timing.hpp"
#include "track.hpp"
#include "trackdata.hpp"
#include "tracktile.hpp"

#include <MenuItem>
#include <MenuManager>

#include <MCAssetManager>
#include <MCSurface>
#include <MCTextureFont>
#include <MCTextureText>

#include "../common/config.hpp"
#include "../common/mapbase.hpp"

#include <cassert>
#include <memory>
#include <sstream>

#include <QObject> // For QObject::tr()

const float SAIL_AWAY_HONEY_X = 1000;

const int ANIMATION_STEPS = 15;

const int ANIMATION_EXP = 3;

std::string TrackSelectionMenu::MenuId = "trackSelection";

class TrackItem : public MTFH::MenuItem
{
public:

    TrackItem(int width, int height, Track & track)
    : MenuItem(width, height)
    , m_game(Game::instance())
    , m_track(track)
    , m_font(MCAssetManager::textureFontManager().font(m_game.fontName()))
    , m_star(MCAssetManager::surfaceManager().surface("star"))
    , m_glow(MCAssetManager::surfaceManager().surface("starGlow"))
    , m_lock(MCAssetManager::surfaceManager().surface("lock"))
    , m_lapRecord(Settings::instance().loadLapRecord(m_track))
    , m_raceRecord(Settings::instance().loadRaceRecord(
        m_track, m_game.lapCount(), m_game.difficultyProfile().difficulty()))
    , m_bestPos(Settings::instance().loadBestPos(
        m_track, m_game.lapCount(), m_game.difficultyProfile().difficulty()))
    {
        auto && program = Renderer::instance().program("menu");
        m_star.setShaderProgram(program);
        m_glow.setShaderProgram(program);
        m_lock.setShaderProgram(program);
    }

    Track & track() const
    {
        return m_track;
    }

    virtual void setFocused(bool focused) override
    {
        MenuItem::setFocused(focused);

        m_lapRecord  = Settings::instance().loadLapRecord(m_track);
        m_raceRecord = Settings::instance().loadRaceRecord(
            m_track, m_game.lapCount(), m_game.difficultyProfile().difficulty());
        m_bestPos = Settings::instance().loadBestPos(
            m_track, m_game.lapCount(), m_game.difficultyProfile().difficulty());
    }

    //! \reimp
    virtual void render() override;

private:

    void renderTiles();

    void renderTitle();

    void renderStars();

    void renderLock();

    void renderTrackProperties();

    Game & m_game;

    Track & m_track;

    MCTextureFont & m_font;

    MCSurface & m_star;

    MCSurface & m_glow;

    MCSurface & m_lock;

    int m_lapRecord;

    int m_raceRecord;

    int m_bestPos;
};

void TrackItem::renderTiles()
{
    const MapBase & rMap = m_track.trackData().map();

    const int previewW = width();
    const int previewH = height();

    // Set tileW and tileH so that they are squares
    float tileW = previewW / rMap.cols();
    float tileH = previewH / rMap.rows();

    if (tileW > tileH)
    {
        tileW = tileH;
    }
    else
    {
        tileH = tileW;
    }

    // Center the preview
    int initX, initY;
    if (rMap.cols() % 2 == 0)
    {
        initX = x() - rMap.cols() * tileW / 2 + tileW / 4;
    }
    else
    {
        initX = x() - rMap.cols() * tileW / 2;
    }

    initY = y() - rMap.rows() * tileH / 2;

    initX += menu()->x();
    initY += menu()->y();

    // Loop through the visible tile matrix and draw the tiles
    float tileY = initY;
    for (unsigned int j = 0; j < rMap.rows(); j++)
    {
        float tileX = initX;
        for (unsigned int i = 0; i < rMap.cols(); i++)
        {
            auto tile = std::static_pointer_cast<TrackTile>(rMap.getTile(i, j));
            auto surface = tile->previewSurface();
            if (surface && !tile->excludeFromMinimap())
            {
                surface->setShaderProgram(Renderer::instance().program("menu"));
                surface->bind();

                if (m_track.trackData().isLocked())
                {
                    surface->setColor(MCGLColor(0.5, 0.5, 0.5));
                }
                else
                {
                    surface->setColor(MCGLColor(1.0, 1.0, 1.0));
                }

                surface->setSize(tileH, tileW);
                surface->render(
                    nullptr,
                    MCVector3dF(tileX + tileW / 2, tileY + tileH / 2), tile->rotation());
            }

            tileX += tileW;
        }

        tileY += tileH;
    }
}

void TrackItem::renderTitle()
{
    MCTextureText text(L"");

    const int shadowY = -2;
    const int shadowX =  2;

    std::wstringstream ss;
    ss << m_track.trackData().name().toUpper().toStdWString();
    text.setText(ss.str());
    text.setGlyphSize(30, 30);
    text.setShadowOffset(shadowX, shadowY);
    text.render(menu()->x() + x() - text.width(m_font) / 2, menu()->y() + y() + height() / 2 + text.height(m_font), nullptr, m_font);
}

void TrackItem::renderStars()
{
    if (!m_track.trackData().isLocked())
    {
        const int starW = m_star.width();
        const int starH = m_star.height();
        const int startX = menu()->x() + x() - 5 * starW + starW / 2;
        const MCGLColor yellow(1.0, 1.0, 0.0);
        const MCGLColor grey(.75, .75, .75);

        const int numStars = 10;
        for (int i = 0; i < numStars; i++)
        {
            const MCVector3dF starPos(startX + i * starW, menu()->y() + y() - height() / 2 + starH / 2, 0);

            // The range of m_bestPos is 1..NUM_CARS
            if (m_bestPos != -1 && numStars - i >= m_bestPos)
            {
                m_star.setColor(yellow);
                m_glow.render(nullptr, starPos, 0);
            }
            else
            {
                m_star.setColor(grey);
            }

            m_star.render(nullptr, starPos, 0);
        }
    }
}

void TrackItem::renderLock()
{
    if (m_track.trackData().isLocked())
    {
        m_lock.render(nullptr, MCVector3dF(menu()->x() + x(), menu()->y() + y(), 0), 0);
    }
}

void TrackItem::renderTrackProperties()
{
    MCTextureText text(L"");

    const int shadowY = -2;
    const int shadowX =  2;

    std::wstringstream ss;
    text.setGlyphSize(20, 20);
    text.setShadowOffset(shadowX, shadowY);

    const int textX = menu()->x() + x();
    int maxWidth = 0;

    std::vector<MCTextureText> texts;

    // Render track properties
    ss.str(L"");
    ss << QObject::tr("       Laps: ").toStdWString() << Game::instance().lapCount();
    text.setText(ss.str());
    text.setGlyphSize(20, 20);
    maxWidth = std::fmax(maxWidth, text.width(m_font));
    texts.push_back(text);

    ss.str(L"");
    ss << QObject::tr("     Length: ").toStdWString()
       << int(m_track.trackData().route().geometricLength() * MCWorld::metersPerUnit());
    text.setText(ss.str());
    maxWidth = std::fmax(maxWidth, text.width(m_font));
    texts.push_back(text);

    if (!m_track.trackData().isLocked())
    {
        ss.str(L"");
        ss << QObject::tr(" Lap Record: ").toStdWString() << Timing::msecsToString(m_lapRecord);
        text.setText(ss.str());
        maxWidth = std::fmax(maxWidth, text.width(m_font));
        texts.push_back(text);

        ss.str(L"");
        ss << QObject::tr("Race Record: ").toStdWString() << Timing::msecsToString(m_raceRecord);
        text.setText(ss.str());
        maxWidth = std::fmax(maxWidth, text.width(m_font));
        texts.push_back(text);
    }

    const float yPos = menu()->y() + y() - height() / 2;
    float lineHeight = text.height(m_font);
    int line = 2;
    for (auto && text: texts)
    {
        text.render(textX - maxWidth / 2, yPos - lineHeight * line, nullptr, m_font);
        line++;
    }
}

void TrackItem::render()
{
    renderTiles();

    renderTitle();

    renderStars();

    renderLock();

    renderTrackProperties();
}

TrackSelectionMenu::TrackSelectionMenu(int width, int height, Scene & scene)
    : SurfaceMenu("trackSelectionBack", MenuId, width, height, Menu::Style::ShowMany, true, true, true)
    , m_selectedTrack(nullptr)
    , m_scene(scene)
{
    setWrapAround(false);
}

void TrackSelectionMenu::addTrack(Track & track)
{
    auto item = MTFH::MenuItemPtr(new TrackItem(width() / 2, height() / 2, track));
    item->setPos(width() / 2, height() / 2);
    addItem(item);
    setCurrentIndex(0);
    setItemsToShow({0});
}

void TrackSelectionMenu::left()
{
    const int prevIndex = currentIndex();

    if (prevIndex > 0)
    {
        currentItem()->setPos(width() / 2, height() / 2, width() + SAIL_AWAY_HONEY_X, height() / 2);
        currentItem()->resetAnimationCurve(ANIMATION_STEPS, ANIMATION_EXP);

        Menu::left();

        currentItem()->setPos(-SAIL_AWAY_HONEY_X, height() / 2, width() / 2, height() / 2);
        currentItem()->resetAnimationCurve(ANIMATION_STEPS, ANIMATION_EXP);

        setItemsToShow({prevIndex, currentIndex()});
    }
}

void TrackSelectionMenu::right()
{
    const int prevIndex = currentIndex();

    if (prevIndex + 1 < static_cast<int>(itemCount()))
    {
        currentItem()->setPos(width() / 2, height() / 2, -SAIL_AWAY_HONEY_X, height() / 2);
        currentItem()->resetAnimationCurve(ANIMATION_STEPS, ANIMATION_EXP);

        Menu::right();

        currentItem()->setPos(width() + SAIL_AWAY_HONEY_X, height() / 2, width() / 2, height() / 2);
        currentItem()->resetAnimationCurve(ANIMATION_STEPS, ANIMATION_EXP);

        setItemsToShow({prevIndex, currentIndex()});
    }
}

void TrackSelectionMenu::up()
{
    left();
}

void TrackSelectionMenu::down()
{
    right();
}

void TrackSelectionMenu::exit()
{
    MTFH::MenuManager::instance().popToMenu(MainMenu::MenuId);
}

void TrackSelectionMenu::selectCurrentItem()
{
    Menu::selectCurrentItem();
    auto && selection = std::static_pointer_cast<TrackItem>(currentItem())->track();
    if (!selection.trackData().isLocked())
    {
        m_selectedTrack = &selection;
        m_scene.setActiveTrack(*m_selectedTrack);
        setIsDone(true);
    }
}

Track * TrackSelectionMenu::selectedTrack() const
{
    return m_selectedTrack;
}
