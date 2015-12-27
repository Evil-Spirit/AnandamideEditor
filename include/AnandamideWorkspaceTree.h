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

#ifndef ANANDAMIDE_WORKSPACE_TREE_H
#define ANANDAMIDE_WORKSPACE_TREE_H

#include <QTreeWidget>
#include "TreeItemBase.h"

namespace Anandamide {
	class Script;
}

//------------------------------------------------------------------------------
//
// TreeItemWorkspace
//
//------------------------------------------------------------------------------

class TreeItemDepProject;
class EditorWidgetsCollection;
class BreakPointsTree;
class CallStackTree;

class TreeItemWorkspace: public TreeItemBase {
		
		QString fileName;
		bool changeFlag;
	
	public:
		
		TreeItemWorkspace(QTreeWidget *parent, MainWindow *main_window);
		
		bool addScriptItem(Anandamide::Script* script, const QString& fileName, int tabIndex, EditorWidgetsCollection* collection);
		
		bool removeScriptItem(Anandamide::Script* script);
		bool removeScriptProjectByIndex(int tabIndex);
		
		int getScriptWorkspaceIndex(Anandamide::Script* script);
		
		bool updateProject(Anandamide::Script* script);
		TreeItemDepProject* getScriptProject(Anandamide::Script* script);
		TreeItemDepProject* getTabIndexProject(int tabIndex);
		TreeItemDepProject* getProjectByFilename(const QString& filename);
		
		void updateChildren();
		
		EditorWidgetsCollection* getCollection(int tabIndex);
		Anandamide::Script* getScript(int tabIndex);
		void selectByTabIndex(int tabIndex);
		
		void tabMoved(int from, int to);
		void resetTabIndex(int tabIndex);
		
		QString getFileName(int tabIndex);
		bool setFileName(const QString& filename, int tabIndex);
		
		void setShiftOnWheel(bool shift);
		
		void clearSelection();
		
		bool saveWorkspace();
		bool saveWorkspace(const QString& filename);
		bool loadWorkspace(const QString& filename);
		void setChanged(bool changed);
		bool isWorkspaceChanged();
		
		QTreeWidgetItem* getLibraryItem(const QString& libraryName);
		
		void onStopExecuting();
};

//------------------------------------------------------------------------------
//
// TreeItemDepProject
//
//------------------------------------------------------------------------------

class TreeItemDepProject: public TreeItemBase {
		
	friend class TreeItemWorkspace;
	friend class AnandamideWorkspaceTree;
	
	class DepProjectInfo {
	public:
		Anandamide::Script* script;
		QString fileName;
		int tabIndex;
		QList<DepProjectInfo*> childInfo;
		EditorWidgetsCollection* collection;
		bool selected;
		
		DepProjectInfo();
		
		DepProjectInfo(DepProjectInfo* other);
		
		void clear();

		~DepProjectInfo() {
			clear();
		}
		
		void childCopy(DepProjectInfo* other);
		
		QList<int> getChildsTabs();
	};
	DepProjectInfo info;
	QList<DepProjectInfo*> cache; 
	
	void clearCache();
	int getCacheIndex(Anandamide::Script* script);
	void saveToCache();
	QList<int> restoreFromCache();
	
	public:
		
		TreeItemDepProject(Anandamide::Script* script, const QString& fileName, int tabIndex, TreeItemBase *parent);
		~TreeItemDepProject();
		
		bool isTopLevelProject();
		void updateChildren();
		
		bool isScriptUsed(Anandamide::Script* script);
		
		Anandamide::Script* getScript();
		Anandamide::Script* getScript(int tabIndex);
		int getScriptIndex(Anandamide::Script* script);
		
		bool updateProject(Anandamide::Script* script);
		
		EditorWidgetsCollection* getCollection();
		EditorWidgetsCollection* getCollection(int tabIndex);
		void setCollection(EditorWidgetsCollection* collection);
		
		int getTabIndex();
		void setTabIndex(int tabIndex);
		void tabMoved(int from, int to);
		void resetTabIndex(int tabIndex);
		
		void setShiftOnWheel(bool shift);
		
		void setSelected(bool selected);
		
		bool selectByTabIndex(int tabIndex);
		
		void createNewTab();
		
		QString getFileName(int tabIndex);
		bool setFileName(const QString& filename, int tabIndex);
		
		QList<int> getChildsTabs();
		
		QTreeWidgetItem* getLibraryItem(const QString& libraryName, EditorWidgetsCollection* collection);
		TreeItemDepProject* getChildByName(const QString& name);
		
		void onStopExecuting();
		
		BreakPointsTree* breakPointsTree;
		CallStackTree* callStackTree;
		
		TreeItemDepProject* getScriptProject(Anandamide::Script* script);
		TreeItemDepProject* getTabIndexProject(int tabIndex);
};


//------------------------------------------------------------------------------
//
// AnandamideWorkspaceTree
//
//------------------------------------------------------------------------------

class AnandamideWorkspaceTree : public QTreeWidget
{
	Q_OBJECT
	
	TreeItemWorkspace *root;
	
public:
	explicit AnandamideWorkspaceTree(QWidget *parent = 0);
	
	bool addScriptProject(Anandamide::Script* script, const QString& fileName, int tabIndex, EditorWidgetsCollection* collection);
	
	bool removeScriptProject(Anandamide::Script* script);
	bool removeScriptProjectByIndex(int tabIndex);
	bool isScriptProject(Anandamide::Script* script);
	bool setScriptProject(const QString& fileName);
	
	bool updateProject(Anandamide::Script* script);
	
	void update();
	
	EditorWidgetsCollection* getCollection(int tabIndex);
	Anandamide::Script* getScript(int tabIndex);
	void selectByTabIndex(int tabIndex);
	
	void tabMoved(int from, int to);
	void resetTabIndex(int tabIndex);
	
	void setShiftOnWheel(bool shift);
	
	QString getFileName(int tabIndex);
	bool setFileName(const QString& filename, int tabIndex);
	
	void newWorkspace();
	bool saveWorkspace();
	bool saveWorkspace(const QString& filename);
	bool loadWorkspace(const QString& filename);
	bool isWorkspaceChanged();
	void clearChanges();
	
	bool openLibrary(const QString& libraryName);
	void onStopExecuting();
	void setCurrentScript(QStringList &logicLibList);
	void openScriptView(Anandamide::Script* script);
	
	bool isProjectRunning(int tabIndex);
	
private slots:
	void onItemDoubleClicked(QTreeWidgetItem *tree_item, int column);
};

#endif // ANANDAMIDE_WORKSPACE_TREE_H
