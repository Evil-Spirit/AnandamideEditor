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

#include "EditorWidgetsCollection.h"
#include "MainWindow.h"

#include "AnandamideViewport.h"

#include <AnandamideLibraryWidget.h>
#include <AnandamideLogWidget.h>
#include <ConsoleOutWidget.h>
#include <QHeaderView>
#include <QUndoStack>
#include <QUndoView>

EditorWidgetsCollection::EditorWidgetsCollection(QObject *parent) : QObject(parent) {
	project_tree = NULL;
	logWidget = NULL;
	library_widget = NULL;
	properties_tree = NULL;
	viewport = NULL;
	editor = NULL;
}

EditorWidgetsCollection::~EditorWidgetsCollection()
{
//	project_tree->setVisible(false);
//	objects_tree->setVisible(false);
//	helpBrowser->setVisible(false);
//	logWidget->setVisible(false);
//	library_widget->setVisible(false);
//	properties_tree->setVisible(false);
//	undoView->setVisible(false);
//	viewport->setVisible(false);
//	breakPointsTree->setVisible(false);
//	callStackTree->setVisible(false);
	
//	delete project_tree;
//	delete objects_tree;
//	delete helpBrowser;
//	delete logWidget;
//	delete library_widget;
//	delete properties_tree;
//	delete undoView;
//	delete undoStack;
//	delete viewport;
////	delete editor;
//	delete breakPointsTree;
//	delete callStackTree;
}

bool EditorWidgetsCollection::isCreated()
{
	return project_tree != NULL;
}

void EditorWidgetsCollection::createCollection(MainWindow *parent)
{
	if(isCreated()) return;
	
	viewport = new AnandamideViewport(parent);
	viewport->setMainWindow(parent);
	editor = new MainLogicEditor();
	viewport->setEditor(editor);
	
	// project_tree
	project_tree = new QTreeWidget(parent);
	connect(project_tree, 
			SIGNAL(itemDoubleClicked(QTreeWidgetItem *,int)), 
			SLOT(onItemDoubleClicked(QTreeWidgetItem *,int)));
	project_tree->setItemDelegate(new MyDelegate(project_tree));
	project_tree->setDragDropMode(QAbstractItemView::DragOnly);
	project_tree->setHeaderHidden(true);
	project_tree->setObjectName("project_tree");
	
	// properties_tree
	properties_tree = new QTreeWidget(parent);
	connect(properties_tree, 
			SIGNAL(itemDoubleClicked(QTreeWidgetItem *,int)), 
			SLOT(onItemDoubleClicked(QTreeWidgetItem *,int)));
	properties_tree->setItemDelegate(new MyPropertiesDelegate(properties_tree));
	properties_tree->setDragDropMode(QAbstractItemView::DragOnly);
	new TreeItemProperties(properties_tree, parent);
	properties_tree->setColumnCount(2);
	properties_tree->setHeaderLabels(QStringList() << "name" << "value");
	properties_tree->header()->setDefaultSectionSize(100);
//	properties_tree->header()->setMaximumSectionSize(100);
	
	// ConsoleOutWidget
	consoleOutWidget = new ConsoleOutWidget(parent);
	
	
	logWidget = new AnandamideLogWidget(parent);
	
	library_widget = new AnandamideLibraryWidget(parent);
}

void EditorWidgetsCollection::onItemDoubleClicked(QTreeWidgetItem *tree_item, int column)
{
	TreeItemBase *item = (TreeItemBase *)tree_item;
	if(item != NULL) item->onDoubleClicked();
	
	TreeItemProperty* prop = dynamic_cast<TreeItemProperty*>(item);
	if(!prop || column != 1)
		return;
	prop->toggleBoolValue();
}
