// This file belongs to the "Dust" car racing game
// Copyright (C) 2010 Jussi Lind <jussi.lind@iki.fi>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
// MA  02110-1301, USA.
//

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>
#include <QString>

class Editor;
class EditorView;
class EditorScene;
class TrackData;
class QTextEdit;
class QAction;

/*! \class MainWindow
 *  \brief The main window of the editor.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    //! Constructor
    MainWindow();
    
    //! Destructor
    ~MainWindow();

protected:

    //! \reimp
    void closeEvent(QCloseEvent * event);

private slots:

    void initializeNewTrack();
    void openTrack();
    void saveTrack();
    void saveAsTrack();

private:

    void createMenuBar();
    void createGrid();
    void console(QString text);

    Editor      * m_editor;
    EditorView  * m_editorView;
    EditorScene * m_editorScene;
    TrackData   * m_trackData;
    QTextEdit   * m_console;
    QAction     * m_saveAction;
    QAction     * m_saveAsAction;
};

#endif // MAINWINDOW_H
