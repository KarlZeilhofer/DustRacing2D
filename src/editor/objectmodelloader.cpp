// This file is part of Dust Racing 2D.
// Copyright (C) 2011 Jussi Lind <jussi.lind@iki.fi>
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

#include <QDomDocument>
#include <QDomElement>
#include <QFile>
#include <QTextStream>

#include "../common/config.hpp"
#include "mainwindow.hpp"
#include "objectmodelloader.hpp"

ObjectModelLoader::ObjectModelLoader()
{
}

bool ObjectModelLoader::load(QString path)
{
    QDomDocument doc;

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }

    if (!doc.setContent(&file)) {
        file.close();
        return false;
    }

    file.close();

    QDomElement root = doc.documentElement();
    QString version = root.attribute("version",
                                     Config::Editor::EDITOR_VERSION);

    m_objects.clear();

    QDomNode node = root.firstChild();
    while (!node.isNull()) {
        QDomElement tag = node.toElement();
        if (!tag.isNull()) {
            // Parse an object tag
            if (tag.nodeName() == "object") {
                ObjectModel newData;

                newData.category = tag.attribute("category", "undefined");
                newData.role = tag.attribute("role", "undefined");
                newData.width = tag.attribute("width", "0").toUInt();
                newData.height = tag.attribute("height", "0").toUInt();
                QString imagePath = tag.attribute("imagePath", "undefined");

                // The corresponding image is loaded
                // from Config::DATA_PATH/model.imagePath.
                // Check that it's available and load it.
                imagePath = QString(Config::Common::dataPath) + QDir::separator() + imagePath;

                if (QFile::exists(imagePath)) {
                    newData.pixmap = QPixmap(imagePath);
                    m_objects << newData;
                } else {
                    MainWindow::instance()->console("WARNING!!: " + imagePath + " cannot be read.");
                }
            }

            node = node.nextSibling();
        }
    }

    return true;
}

ObjectModelLoader::ObjectDataVector ObjectModelLoader::getObjectModelsByCategory(
  QString category) const
{
    ObjectDataVector result;

    for (int i = 0; i < m_objects.size(); i++) {
        if (m_objects[i].category == category) {
            result << m_objects[i];
        }
    }

    return result;
}

ObjectModel ObjectModelLoader::getObjectModelByRole(QString role) const
{
    for (int i = 0; i < m_objects.size(); i++) {
        if (m_objects[i].role == role) {
            return m_objects[i];
        }
    }

    return ObjectModel();
}

QString ObjectModelLoader::getCategoryByRole(QString role) const
{
    for (int i = 0; i < m_objects.size(); i++) {
        if (m_objects[i].role == role) {
            return m_objects[i].category;
        }
    }

    return QString();
}

ObjectModelLoader::ObjectDataVector ObjectModelLoader::objects() const
{
    return m_objects;
}

QPixmap ObjectModelLoader::getPixmapByRole(QString role) const
{
    for (int i = 0; i < m_objects.size(); i++) {
        if (m_objects[i].role == role) {
            return m_objects[i].pixmap;
        }
    }

    return QPixmap();
}
