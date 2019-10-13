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

#ifndef TRACKDATABASE_HPP
#define TRACKDATABASE_HPP

#include <QString>

class Objects;
class Route;
class MapBase;

//! Common base class for track data shared by the editor and the game.
class TrackDataBase
{
public:
    /** Keywords used in the track data files. */
    struct DataKeywords
    {
        struct Header
        {
            static constexpr auto version = "version";

            static constexpr auto track = "track";

            static constexpr auto name = "name";

            static constexpr auto cols = "cols";

            static constexpr auto rows = "rows";

            static constexpr auto index = "index";

            static constexpr auto user = "isUserTrack";
        };

        struct Track
        {
            static constexpr auto node = "n";

            static constexpr auto tile = "t";

            static constexpr auto object = "o";
        };

        struct Node
        {
            static constexpr auto index = "i";

            static constexpr auto x = "x";

            static constexpr auto y = "y";

            static constexpr auto width = "w";

            static constexpr auto height = "h";
        };

        struct Tile
        {
            static constexpr auto type = "t";

            static constexpr auto i = "i";

            static constexpr auto j = "j";

            static constexpr auto orientation = "o";

            static constexpr auto computerHint = "c";

            static constexpr auto excludeFromMinimap = "e";
        };

        struct Object
        {
            static constexpr auto role = "r";

            static constexpr auto category = "c";

            static constexpr auto x = "x";

            static constexpr auto y = "y";

            static constexpr auto orientation = "o";

            static constexpr auto forceStationary = "fs";
        };
    };

    //! Constructor.
    TrackDataBase(QString name, bool isUserTrack);

    //! Copy constructor.
    TrackDataBase(const TrackDataBase & other);

    TrackDataBase & operator=(const TrackDataBase & other) = delete;

    //! Destructor.
    virtual ~TrackDataBase();

    //! Get name.
    virtual QString name() const;

    //! Set name.
    virtual void setName(QString name);

    //! Get file name.
    virtual QString fileName() const = 0;

    //! Set file name.
    virtual void setFileName(QString fileName) = 0;

    //! Get lap index.
    virtual unsigned int index() const;

    //! Set track index.
    virtual void setIndex(unsigned int index);

    //! Get if the track is user track (unlocked by default).
    virtual bool isUserTrack() const;

    //! Set user track option.
    virtual void setUserTrack(bool isUserTrack);

    //! Get map object.
    virtual MapBase & map() = 0;

    //! Get map object.
    virtual const MapBase & map() const = 0;

    //! Get route object.
    virtual Route & route() = 0;

    //! Get route object.
    virtual const Route & route() const = 0;

    //! Get objects object.
    virtual Objects & objects() = 0;

    //! Get objects object.
    virtual const Objects & objects() const = 0;

private:
    QString m_name;

    unsigned int m_index;

    bool m_isUserTrack;
};

#endif // TRACKDATABASE_HPP
