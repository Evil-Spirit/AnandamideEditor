//------------------------------------------------------------------------------
//
// This file is part of AnandamideEditor
//
// copyright:	(c) 2010 - 2016
// authors:		Alexey Egorov (FadeToBlack aka EvilSpirit)
// 				Zakhar Shelkovnikov
// 				Georgiy Kostarev
//
// mailto:		anandamide@mail.ru
// 				anandamide.script@gmail.com
//
// AnandamideEditor is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// AnandamideEditor is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with AnandamideEditor. If not, see <http://www.gnu.org/licenses/>.
//
//------------------------------------------------------------------------------

#ifndef EDITORWIDGETSCOLLECTION_H
#define EDITORWIDGETSCOLLECTION_H

#include <QObject>

class MainWindow;
class MainLogicEditor;
class QTreeWidget;
class QTreeWidget;
class AnandamideLogWidget;
class AnandamideLibraryWidget;
class QTreeWidget;
class AnandamideViewport;
class MainLogicEditor;
class ConsoleOutWidget;
class QTreeWidgetItem;

class EditorWidgetsCollection : public QObject
{
	Q_OBJECT
	
public:
	QTreeWidget*				project_tree;
	AnandamideLogWidget*		logWidget;
	AnandamideLibraryWidget*	library_widget;
	QTreeWidget*				properties_tree;
	AnandamideViewport*			viewport;
	MainLogicEditor*			editor;
	ConsoleOutWidget*			consoleOutWidget;
	
	EditorWidgetsCollection(QObject* parent = 0);
	~EditorWidgetsCollection();
	
	bool isCreated();
	void createCollection(MainWindow* parent);
	
private slots:
	void onItemDoubleClicked(QTreeWidgetItem *item, int column);
};

#endif // EDITORWIDGETSCOLLECTION_H
