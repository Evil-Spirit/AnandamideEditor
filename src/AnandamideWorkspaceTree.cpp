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

#include "AnandamideWorkspaceTree.h"

#include "MainWindow.h"
#include "Anandamide.h"
#include "AnandamideLibrary.h"
#include "AnandamideViewport.h"
#include "XmlSettings.h"
#include "AnandamideViewport.h"
#include "AnandamideLibraryWidget.h"

#include <QFileInfo>
#include <QDir>

//------------------------------------------------------------------------------
//
// TreeItemWorkspace
//
//------------------------------------------------------------------------------

void TreeItemWorkspace::setChanged(bool changed)
{
	changeFlag = changed;
	QString text = "Default workspace";
	if(fileName != QString()) {
		text = QFileInfo(fileName).baseName();
	}
	if(changeFlag)
		text.append('*');
	setText(text.toLocal8Bit().constData());
}

TreeItemWorkspace::TreeItemWorkspace(QTreeWidget *parent, MainWindow *main_window) : TreeItemBase(parent, main_window) {
	setIcon(0, QIcon(":/icons/folder2.png"));
	setExpanded(true);
	setChanged(false);
}

bool TreeItemWorkspace::addScriptItem(Anandamide::Script *script, const QString& fileName, int tabIndex, EditorWidgetsCollection *collection) {
	if(getScriptWorkspaceIndex(script) >= 0) return false;
	clearSelection();
	TreeItemDepProject* project = new TreeItemDepProject(script, fileName, tabIndex, this);
	project->setCollection(collection);
	project->setSelected(true);
	setChanged(true);
	return true;
}

bool TreeItemWorkspace::removeScriptItem(Anandamide::Script *script)
{
	int idx = getScriptWorkspaceIndex(script);
	if(idx < 0) return false;
	QTreeWidgetItem* child = this->child(idx);
	TreeItemDepProject* project = dynamic_cast<TreeItemDepProject*>(child);
	if(project) {
		QList<int> tabs = project->getChildsTabs();
		qSort(tabs);
		while(tabs.size())
			main_window->on_tabWidgetViewports_tabCloseRequested(tabs.takeLast());
	}
	this->removeChild(child);
	delete child;
	setChanged(true);
	return true;
}

bool TreeItemWorkspace::removeScriptProjectByIndex(int tabIndex)
{
	for(int i = 0; i < this->childCount(); ++i) {
		TreeItemDepProject* proj = dynamic_cast<TreeItemDepProject*>(this->child(i));
		if(proj == NULL) continue;
		if(proj->getTabIndex() == tabIndex) {
			QTreeWidgetItem* child = this->child(i);
			this->removeChild(child);
			delete child;
			setChanged(true);
			return true;
		}
	}
	return false;
}

int TreeItemWorkspace::getScriptWorkspaceIndex(Anandamide::Script *script)
{
	for(int i = 0; i < this->childCount(); ++i) {
		TreeItemDepProject* proj = dynamic_cast<TreeItemDepProject*>(this->child(i));
		if(proj == NULL) continue;
		if(proj->getScript() == script /*|| Anandamide::Str(proj->script->getName()) == Anandamide::Str(script->getName())*/)
			return i;
	}
	return -1;
}

bool TreeItemWorkspace::updateProject(Anandamide::Script *script)
{
//	int idx = getScriptWorkspaceIndex(script);
//	if(idx < 0) return false;
//	TreeItemDepProject* proj = dynamic_cast<TreeItemDepProject*>(this->child(idx));
//	if(proj == NULL) return false;
//	proj->updateChildren();
//	return true;
	bool res = false;
	for(int i = 0; i < this->childCount(); ++i) {
		TreeItemDepProject* proj = dynamic_cast<TreeItemDepProject*>(this->child(i));
		if(proj == NULL) continue;
		if(proj->updateProject(script)) res = true;
	}
	return res;
}

TreeItemDepProject *TreeItemWorkspace::getScriptProject(Anandamide::Script *script)
{
	for(int i = 0; i < this->childCount(); ++i) {
		TreeItemDepProject* proj = dynamic_cast<TreeItemDepProject*>(this->child(i));
		if(proj == NULL) continue;
		TreeItemDepProject* res = proj->getScriptProject(script);
		if(res != NULL) return res;
	}
	return NULL;
}

TreeItemDepProject *TreeItemWorkspace::getTabIndexProject(int tabIndex)
{
	for(int i = 0; i < this->childCount(); ++i) {
		TreeItemDepProject* proj = dynamic_cast<TreeItemDepProject*>(this->child(i));
		if(proj == NULL) continue;
		TreeItemDepProject* res = proj->getTabIndexProject(tabIndex);
		if(res != NULL) return res;
	}
	return NULL;
}

TreeItemDepProject *TreeItemWorkspace::getProjectByFilename(const QString &filename)
{
	for(int i = 0; i < this->childCount(); ++i) {
		TreeItemDepProject* proj = dynamic_cast<TreeItemDepProject*>(this->child(i));
		if(proj == NULL) continue;
		if(filename == proj->info.fileName)
			return proj;
	}
	return NULL;
}

void TreeItemWorkspace::updateChildren()
{
	for(int i = 0; i < this->childCount(); ++i) {
		TreeItemBase* item = dynamic_cast<TreeItemBase*>(this->child(i));
		if(item == NULL) continue;
		item->updateChildren();
	}
}

EditorWidgetsCollection *TreeItemWorkspace::getCollection(int tabIndex)
{
	for(int i = 0; i < this->childCount(); ++i) {
		TreeItemDepProject* child = dynamic_cast<TreeItemDepProject*>(this->child(i));
		if(child == NULL) continue;
		EditorWidgetsCollection* res = child->getCollection(tabIndex);
		if(res != NULL) return res;
	}
	return NULL;
}

Anandamide::Script *TreeItemWorkspace::getScript(int tabIndex)
{
	for(int i = 0; i < this->childCount(); ++i) {
		TreeItemDepProject* child = dynamic_cast<TreeItemDepProject*>(this->child(i));
		if(child == NULL) continue;
		Anandamide::Script* res = child->getScript(tabIndex);
		if(res != NULL) return res;
	}
	return NULL;
}

void TreeItemWorkspace::selectByTabIndex(int tabIndex)
{
	clearSelection();
	for(int i = 0; i < this->childCount(); ++i) {
		TreeItemDepProject* child = dynamic_cast<TreeItemDepProject*>(this->child(i));
		if(child == NULL) continue;
		if(child->selectByTabIndex(tabIndex)) return;
	}
}

void TreeItemWorkspace::tabMoved(int from, int to)
{
	for(int i = 0; i < this->childCount(); ++i) {
		TreeItemDepProject* child = dynamic_cast<TreeItemDepProject*>(this->child(i));
		if(child == NULL) continue;
		child->tabMoved(from, to);
	}
}

void TreeItemWorkspace::resetTabIndex(int tabIndex)
{
	for(int i = 0; i < this->childCount(); ++i) {
		TreeItemDepProject* child = dynamic_cast<TreeItemDepProject*>(this->child(i));
		if(child == NULL) continue;
		child->resetTabIndex(tabIndex);
	}
}

QString TreeItemWorkspace::getFileName(int tabIndex)
{
	for(int i = 0; i < this->childCount(); ++i) {
		TreeItemDepProject* child = dynamic_cast<TreeItemDepProject*>(this->child(i));
		if(child == NULL) continue;
		QString res = child->getFileName(tabIndex);
		if(res != QString()) return res;
	}
	return QString();
}

bool TreeItemWorkspace::setFileName(const QString &filename, int tabIndex)
{
	for(int i = 0; i < this->childCount(); ++i) {
		TreeItemDepProject* child = dynamic_cast<TreeItemDepProject*>(this->child(i));
		if(child == NULL) continue;
		if(child->setFileName(filename, tabIndex)) return true;
	}
	return false;
}

void TreeItemWorkspace::setShiftOnWheel(bool shift)
{
	for(int i = 0; i < this->childCount(); ++i) {
		TreeItemDepProject* child = dynamic_cast<TreeItemDepProject*>(this->child(i));
		if(child == NULL) continue;
		child->setShiftOnWheel(shift);
	}
}

void TreeItemWorkspace::clearSelection()
{
	for(int i = 0; i < this->childCount(); ++i) {
		TreeItemDepProject* child = dynamic_cast<TreeItemDepProject*>(this->child(i));
		if(child == NULL) continue;
		child->setSelected(false);
	}
}

bool TreeItemWorkspace::saveWorkspace()
{
	if(fileName == QString()) return false;
	return saveWorkspace(fileName);
}

bool TreeItemWorkspace::saveWorkspace(const QString &filename)
{
	if(filename == QString()) return false;
	CXml_Settings sets;
	sets.beginGroup("WorkSpace");
	
	sets.beginGroup("Scripts");
	int cntr = 0;
	for(int i = 0; i < this->childCount(); ++i) {
		TreeItemDepProject* child = dynamic_cast<TreeItemDepProject*>(this->child(i));
		if(child == NULL) continue;
		sets.beginGroup(QString("Script_%1").arg(cntr));
		sets.setValue("filename", QFileInfo(filename).absoluteDir().relativeFilePath(child->info.fileName));
		sets.endGroup();
		child->setSelected(false);
		cntr++;
	}
	sets.endGroup();
	
	sets.setValue("ScriptsCount", cntr);
	sets.endGroup();
	
	QFile file( filename );
	if ( !file.open(QIODevice::WriteOnly) )
		return false;
	sets.saveToIO(&file);
	file.close();
	
	fileName = filename;
	setChanged(false);
	return true;
}

bool TreeItemWorkspace::loadWorkspace(const QString &filename)
{	
	QFile file( filename );
	if ( !file.open(QIODevice::ReadOnly) )
		return false;
	CXml_Settings sets;
	if(!sets.loadFromIO(&file)) {
		file.close();
		return false;
	}
	file.close();
	main_window->closeAllTabs(true, -1);
	this->clearChildren();
	
	sets.beginGroup("WorkSpace");
	
	int n = sets.value("ScriptsCount").toInt();
	
	sets.beginGroup("Scripts");
	
	for(int i = 0; i < n; ++i) {
		sets.beginGroup(QString("Script_%1").arg(i));
		QString fn = sets.value("filename").toString();
		sets.endGroup();
		if(fn != QString()) {
			fn = QFileInfo(filename).absoluteDir().absoluteFilePath(fn);
			
			Anandamide::Script* scr = main_window->createScript();
			
			EditorWidgetsCollection* coll = new EditorWidgetsCollection(main_window);
			main_window->currentCollection = coll;
			coll->createCollection(main_window);
			coll->viewport->shiftFlag = main_window->getShiftFlag();
			main_window->setMessangerView(coll);
			
			scr->load(fn.toLocal8Bit().constData());
			if(scr->getLogicsCount() <= 0) {
				scr->destroy();
				continue;
			}
//			main_window->createTestLibraryForScript(scr);
			
			addScriptItem(scr, fn, -1, coll);
		}
	}
	sets.endGroup();
	sets.endGroup();
	
	fileName = filename;
	setChanged(false);
	selectByTabIndex(-1);
	return true;
}

bool TreeItemWorkspace::isWorkspaceChanged()
{
	return changeFlag;
}

QTreeWidgetItem *TreeItemWorkspace::getLibraryItem(const QString &libraryName)
{
	for(int i = 0; i < this->childCount(); ++i) {
		TreeItemDepProject* child = dynamic_cast<TreeItemDepProject*>(this->child(i));
		if(child == NULL) continue;
		QTreeWidgetItem* item = child->getLibraryItem(libraryName, main_window->currentCollection);
		if(item != NULL) return item;
	}
	return NULL;
}

void TreeItemWorkspace::onStopExecuting()
{
	for(int i = 0; i < this->childCount(); ++i) {
		TreeItemDepProject* child = dynamic_cast<TreeItemDepProject*>(this->child(i));
		if(child == NULL) continue;
		child->onStopExecuting();
	}
}

//------------------------------------------------------------------------------
//
//  TreeItemDepProject
//
//------------------------------------------------------------------------------

void TreeItemDepProject::clearCache()
{
	while(cache.size())
		delete cache.takeFirst();
}

int TreeItemDepProject::getCacheIndex(Anandamide::Script *script)
{
	for(int i = 0; i < cache.size(); ++i)
		if(cache[i]->script == script)
			return i;
	return -1;
}

void TreeItemDepProject::saveToCache()
{
	clearCache();
	for(int i = 0; i < this->childCount(); ++i) {
		TreeItemDepProject* child = dynamic_cast<TreeItemDepProject*>(this->child(i));
		if(child == NULL) continue;
		child->saveToCache();
		cache.append(new DepProjectInfo());
		cache.last()->script = child->info.script;
		cache.last()->fileName = child->info.fileName;
		cache.last()->tabIndex = child->info.tabIndex;
		foreach (DepProjectInfo* cc, child->cache) {
			cache.last()->childInfo.append(new DepProjectInfo(cc));
		}
		cache.last()->collection = child->getCollection();
		cache.last()->selected = child->info.selected;
	}
}

QList<int> TreeItemDepProject::restoreFromCache()
{
	QSet<int> unused;
	QList<int> tabIndexes;
	for(int i = 0; i < cache.size(); ++i)
		unused.insert(i);
	for(int i = 0; i < this->childCount(); ++i) {
		TreeItemDepProject* child = dynamic_cast<TreeItemDepProject*>(this->child(i));
		if(child == NULL) continue;
		int idx = getCacheIndex(child->info.script);
		if(idx < 0) continue;
		unused.remove(idx);
		child->info.fileName = cache[idx]->fileName;
		child->info.tabIndex = cache[idx]->tabIndex;
		child->clearCache();
		foreach (DepProjectInfo* cc, cache[idx]->childInfo) {
			child->cache.append(new DepProjectInfo(cc));
		}
		child->setCollection(cache[idx]->collection);
		child->setSelected(cache[idx]->selected);
		child->restoreFromCache();
	}
	QList<int> unusedList = unused.toList();
	foreach (int i, unusedList) {
		if(cache[i]->tabIndex >= 0)
			tabIndexes.append(cache[i]->tabIndex);
		tabIndexes.append( cache[i]->getChildsTabs() );
	}
	clearCache();
	return tabIndexes;
}

TreeItemDepProject::TreeItemDepProject(Anandamide::Script *script, const QString& fileName, int tabIndex, TreeItemBase *parent) : TreeItemBase(parent) {
	info.script = script;
	info.fileName = fileName;
	info.tabIndex = tabIndex;
	setIcon(0, QIcon(isTopLevelProject() ? ":/icons/blocks.png" : ":/icons/library.png"));
	setExpanded(true);
	updateChildren();
}

TreeItemDepProject::~TreeItemDepProject()
{
	clearCache();
}

bool TreeItemDepProject::isTopLevelProject()
{
	TreeItemDepProject* prg = dynamic_cast<TreeItemDepProject*>(this->parent());
	return prg == NULL;
}

void TreeItemDepProject::updateChildren() {
	if(Anandamide::Str(info.script->getName()) == Anandamide::Str())
		setText("Unnamed Script");
	else
		setText(info.script->getName());
	saveToCache();
	clearChildren();
	for(int i = 0; i < info.script->getLibraries()->getLibrariesCount(); ++i) {
		Anandamide::ScriptLibrary* script_lib = dynamic_cast<Anandamide::ScriptLibrary*>(const_cast<Anandamide::Library*>(info.script->getLibraries()->getLibrary(i)));
		if(script_lib == NULL) continue;
		if(isScriptUsed(script_lib->getScript())) continue;
		/*TreeItemDepProject *item = */new TreeItemDepProject(script_lib->getScript(), QString::fromLocal8Bit(script_lib->getFileName()), -1, this);
	}
	QList<int> tabIndexes = restoreFromCache();
	qSort(tabIndexes);
	while(tabIndexes.size()) {
		main_window->on_tabWidgetViewports_tabCloseRequested(tabIndexes.takeLast());
	}
}

bool TreeItemDepProject::isScriptUsed(Anandamide::Script *script) {
	if(info.script == script/* || Anandamide::Str(info.script->getName()) == Anandamide::Str(script->getName())*/)
		return true;
	TreeItemDepProject* parent = dynamic_cast<TreeItemDepProject*>(this->parent());
	if(parent == NULL) return false;
	return parent->isScriptUsed(script);
}

Anandamide::Script *TreeItemDepProject::getScript() {return info.script;}

Anandamide::Script *TreeItemDepProject::getScript(int tabIndex)
{
	if(info.tabIndex == tabIndex && tabIndex >= 0)
		return info.script;
	for(int i = 0; i < this->childCount(); ++i) {
		TreeItemDepProject* child = dynamic_cast<TreeItemDepProject*>(this->child(i));
		if(child == NULL) continue;
		Anandamide::Script* res = child->getScript(tabIndex);
		if(res != NULL) return res;
	}
	return NULL;
}

int TreeItemDepProject::getScriptIndex(Anandamide::Script *script)
{
	for(int i = 0; i < this->childCount(); ++i) {
		TreeItemDepProject* child = dynamic_cast<TreeItemDepProject*>(this->child(i));
		if(child == NULL) continue;
		if(script = child->getScript()) return i;
	}
	return -1;
}

bool TreeItemDepProject::updateProject(Anandamide::Script *script)
{
	bool res = false;
	if(script == info.script) {
		res = true;
		updateChildren();
	}
	for(int i = 0; i < this->childCount(); ++i) {
		TreeItemDepProject* child = dynamic_cast<TreeItemDepProject*>(this->child(i));
		if(child == NULL) continue;
		if(child->updateProject(script)) res = true;
	}
	return res;
}

EditorWidgetsCollection *TreeItemDepProject::getCollection() {return info.collection;}

EditorWidgetsCollection *TreeItemDepProject::getCollection(int tabIndex)
{
	if(info.tabIndex == tabIndex && tabIndex >= 0)
		return info.collection;
	for(int i = 0; i < this->childCount(); ++i) {
		TreeItemDepProject* child = dynamic_cast<TreeItemDepProject*>(this->child(i));
		if(child == NULL) continue;
		EditorWidgetsCollection* res = child->getCollection(tabIndex);
		if(res != NULL) return res;
	}
	return NULL;
}

void TreeItemDepProject::setCollection(EditorWidgetsCollection *collection)
{
	info.collection = collection;
}

int TreeItemDepProject::getTabIndex()
{
	return info.tabIndex;
}

void TreeItemDepProject::setTabIndex(int tabIndex)
{
	info.tabIndex = tabIndex;
}

void TreeItemDepProject::tabMoved(int from, int to)
{
	if(info.tabIndex == from)
		info.tabIndex = to;
	else if(info.tabIndex == to)
		info.tabIndex = from;
	for(int i = 0; i < this->childCount(); ++i) {
		TreeItemDepProject* child = dynamic_cast<TreeItemDepProject*>(this->child(i));
		if(child == NULL) continue;
		child->tabMoved(from, to);
	}
}

void TreeItemDepProject::resetTabIndex(int tabIndex)
{
	if(tabIndex >= 0) {
		if(info.tabIndex == tabIndex)
			info.tabIndex = -1;
		else if(info.tabIndex > tabIndex)
			info.tabIndex--;
	}
	for(int i = 0; i < this->childCount(); ++i) {
		TreeItemDepProject* child = dynamic_cast<TreeItemDepProject*>(this->child(i));
		if(child == NULL) continue;
		child->resetTabIndex(tabIndex);
	}
}

void TreeItemDepProject::setShiftOnWheel(bool shift)
{
	if(info.collection != NULL)
		if(info.collection->isCreated())
			info.collection->viewport->shiftFlag = shift;
	for(int i = 0; i < this->childCount(); ++i) {
		TreeItemDepProject* child = dynamic_cast<TreeItemDepProject*>(this->child(i));
		if(child == NULL) continue;
		child->setShiftOnWheel(shift);
	}
}

void TreeItemDepProject::setSelected(bool selected)
{
	info.selected = selected;
	QFont f;
	if(selected)
		f.setBold(true);
	this->setFont(0, f);
	this->setBackground(0, selected ? QBrush(QColor(230, 193, 193)) : QBrush());
	
	for(int i = 0; i < this->childCount(); ++i) {
		TreeItemDepProject* child = dynamic_cast<TreeItemDepProject*>(this->child(i));
		if(child == NULL) continue;
		child->setSelected(false);
	}
}

bool TreeItemDepProject::selectByTabIndex(int tabIndex)
{
	if(info.tabIndex == tabIndex && tabIndex >= 0) {
		setSelected(true);
		return true;
	}
	for(int i = 0; i < this->childCount(); ++i) {
		TreeItemDepProject* child = dynamic_cast<TreeItemDepProject*>(this->child(i));
		if(child == NULL) continue;
		if(child->selectByTabIndex(tabIndex)) return true;
	}
	return false;
}

void TreeItemDepProject::createNewTab()
{
	if(info.collection == NULL)
		info.collection = new EditorWidgetsCollection(main_window);
	main_window->currentCollection = info.collection;
	if(!info.collection->isCreated())
		info.collection->createCollection(main_window);
	
	info.tabIndex = main_window->getTabsCount();
	main_window->addCollection(info.collection, info.script->getName(), isTopLevelProject());
	
	Anandamide::Logic *logic = info.script->getLogic(0);
	info.collection->editor->setLogic(logic);
	info.collection->editor->reset();
	info.collection->project_tree->clear();
	TreeItemScript *script_item = new TreeItemScript(info.collection->project_tree, main_window, info.script);
	info.collection->project_tree->addTopLevelItem(script_item);
	script_item->setExpanded(true);
	info.collection->library_widget->invalidate();
	info.collection->viewport->repaint();
}

QString TreeItemDepProject::getFileName(int tabIndex)
{
	if(info.tabIndex == tabIndex && tabIndex >= 0)
		return info.fileName;
	for(int i = 0; i < this->childCount(); ++i) {
		TreeItemDepProject* child = dynamic_cast<TreeItemDepProject*>(this->child(i));
		if(child == NULL) continue;
		QString res = child->getFileName(tabIndex);
		if(res != QString()) return res;
	}
	return QString();
}

bool TreeItemDepProject::setFileName(const QString &filename, int tabIndex)
{
	if(info.tabIndex == tabIndex && tabIndex >= 0) {
		info.fileName = filename;
		return true;
	}
	for(int i = 0; i < this->childCount(); ++i) {
		TreeItemDepProject* child = dynamic_cast<TreeItemDepProject*>(this->child(i));
		if(child == NULL) continue;
		if(child->setFileName(filename, tabIndex)) return true;
	}
	return false;
}

QList<int> TreeItemDepProject::getChildsTabs() {
	QList<int> res;
	if(info.tabIndex >= 0)
		res << info.tabIndex;
	for(int i = 0; i < this->childCount(); ++i) {
		TreeItemDepProject* child = dynamic_cast<TreeItemDepProject*>(this->child(i));
		if(child == NULL) continue;
		res << child->getChildsTabs();
	}
	return res;
}

QTreeWidgetItem *TreeItemDepProject::getLibraryItem(const QString &libraryName, EditorWidgetsCollection *collection)
{
	if(info.collection == collection) {
		for(int i = 0; i < this->childCount(); ++i) {
			TreeItemDepProject* child = dynamic_cast<TreeItemDepProject*>(this->child(i));
			if(child == NULL) continue;
			if(QString::fromLocal8Bit( child->info.script->getName() ) == libraryName) return child;
		}
	}
	else {
		for(int i = 0; i < this->childCount(); ++i) {
			TreeItemDepProject* child = dynamic_cast<TreeItemDepProject*>(this->child(i));
			if(child == NULL) continue;
			QTreeWidgetItem* item = child->getLibraryItem(libraryName, collection);
			if(item != NULL) return item;
		}
	}
	return NULL;
}

TreeItemDepProject *TreeItemDepProject::getChildByName(const QString &name)
{
	return dynamic_cast<TreeItemDepProject*>(this->getChildByText(name));
}

void TreeItemDepProject::onStopExecuting()
{
	for(int i = 0; i < this->childCount(); ++i) {
		TreeItemDepProject* child = dynamic_cast<TreeItemDepProject*>(this->child(i));
		if(child == NULL) continue;
		child->onStopExecuting();
	}
}

TreeItemDepProject *TreeItemDepProject::getScriptProject(Anandamide::Script *script)
{
	if(info.script == script)
		return this;
	for(int i = 0; i < this->childCount(); ++i) {
		TreeItemDepProject* child = dynamic_cast<TreeItemDepProject*>(this->child(i));
		if(child == NULL) continue;
		TreeItemDepProject* res = child->getScriptProject(script);
		if(res != NULL) return res;
	}
	return NULL;
}

TreeItemDepProject *TreeItemDepProject::getTabIndexProject(int tabIndex)
{
	if(info.tabIndex == tabIndex && tabIndex >= 0)
		return this;
	for(int i = 0; i < this->childCount(); ++i) {
		TreeItemDepProject* child = dynamic_cast<TreeItemDepProject*>(this->child(i));
		if(child == NULL) continue;
		TreeItemDepProject* res = child->getTabIndexProject(tabIndex);
		if(res != NULL) return res;
	}
	return NULL;
}

//------------------------------------------------------------------------------
//
// AnandamideWorkspaceTree
//
//------------------------------------------------------------------------------

AnandamideWorkspaceTree::AnandamideWorkspaceTree(QWidget *parent) :
	QTreeWidget(parent)
{
	root = new TreeItemWorkspace(this, main_window);
	connect(this, 
			SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), 
			SLOT(onItemDoubleClicked(QTreeWidgetItem*, int)));
}

bool AnandamideWorkspaceTree::addScriptProject(Anandamide::Script *script, const QString &fileName, int tabIndex, EditorWidgetsCollection *collection)
{
	return root->addScriptItem(script, fileName, tabIndex, collection);
}

bool AnandamideWorkspaceTree::removeScriptProject(Anandamide::Script *script)
{
	return root->removeScriptItem(script);
}

bool AnandamideWorkspaceTree::removeScriptProjectByIndex(int tabIndex)
{
	return root->removeScriptProjectByIndex(tabIndex);
}

bool AnandamideWorkspaceTree::isScriptProject(Anandamide::Script *script)
{
	return root->getScriptWorkspaceIndex(script) >= 0;
}

bool AnandamideWorkspaceTree::setScriptProject(const QString &fileName)
{
	TreeItemDepProject* project = root->getProjectByFilename(fileName);
	if(project == NULL) return false;
	onItemDoubleClicked(project, 0);
	return true;
}

bool AnandamideWorkspaceTree::updateProject(Anandamide::Script *script)
{
	return root->updateProject(script);
}

void AnandamideWorkspaceTree::update()
{
	root->updateChildren();
}

EditorWidgetsCollection *AnandamideWorkspaceTree::getCollection(int tabIndex)
{
	return root->getCollection(tabIndex);
}

Anandamide::Script *AnandamideWorkspaceTree::getScript(int tabIndex)
{
	return root->getScript(tabIndex);
}

QString AnandamideWorkspaceTree::getFileName(int tabIndex)
{
	return root->getFileName(tabIndex);
}

bool AnandamideWorkspaceTree::setFileName(const QString &filename, int tabIndex)
{
	return root->setFileName(filename, tabIndex);
}

void AnandamideWorkspaceTree::newWorkspace()
{
	main_window->closeAllTabs(true, -1);
	delete root;
	root = new TreeItemWorkspace(this, main_window);
}

bool AnandamideWorkspaceTree::saveWorkspace()
{
	return root->saveWorkspace();
}

bool AnandamideWorkspaceTree::saveWorkspace(const QString &filename)
{
	return root->saveWorkspace(filename);
}

bool AnandamideWorkspaceTree::loadWorkspace(const QString &filename)
{
	main_window->setCursorShape(Qt::WaitCursor);
	bool res = root->loadWorkspace(filename);
	main_window->setCursorShape(Qt::ArrowCursor);
	return res;
}

bool AnandamideWorkspaceTree::isWorkspaceChanged()
{
	return root->isWorkspaceChanged();
}

void AnandamideWorkspaceTree::clearChanges()
{
	root->setChanged(false);
}

bool AnandamideWorkspaceTree::openLibrary(const QString &libraryName)
{
	QTreeWidgetItem* item = root->getLibraryItem(libraryName);
	if(item == NULL) return false;
	onItemDoubleClicked(item, 0);
	return true;
}

void AnandamideWorkspaceTree::onStopExecuting()
{
	root->clearSelection();
	root->onStopExecuting();
}

void AnandamideWorkspaceTree::setCurrentScript(QStringList &logicLibList)
{/*
	if(logicLibList.takeLast() != QString()) return;
	while(logicLibList.size() && project != NULL) {
		project = project->getChildByName(logicLibList.takeLast());
	}
	onItemDoubleClicked(project, 0);
*/
}

void AnandamideWorkspaceTree::openScriptView(Anandamide::Script *script)
{
	TreeItemDepProject* project = root->getScriptProject(script);
	if(project == NULL) return;
	if(project->getTabIndex() >= 0) {
		main_window->setTabIndex(project->getTabIndex());
	}
	else {
		project->createNewTab();
	}
}

bool AnandamideWorkspaceTree::isProjectRunning(int tabIndex)
{
	if(tabIndex < 0) return false;
	TreeItemDepProject* project = root->getTabIndexProject(tabIndex);
	while(project != NULL) {
		project = dynamic_cast<TreeItemDepProject*>(project->parent());		
	}
	return false;
}

void AnandamideWorkspaceTree::selectByTabIndex(int tabIndex)
{
	root->selectByTabIndex(tabIndex);
}

void AnandamideWorkspaceTree::tabMoved(int from, int to)
{
	root->tabMoved(from, to);
}

void AnandamideWorkspaceTree::resetTabIndex(int tabIndex)
{
	root->resetTabIndex(tabIndex);
}

void AnandamideWorkspaceTree::setShiftOnWheel(bool shift)
{
	root->setShiftOnWheel(shift);
}

void AnandamideWorkspaceTree::onItemDoubleClicked(QTreeWidgetItem *tree_item, int column)
{
	Q_UNUSED(column);
	TreeItemDepProject* project = dynamic_cast<TreeItemDepProject*>(tree_item);
	if(project == NULL) return;
	root->clearSelection();
	project->setSelected(true);
	if(project->getTabIndex() >= 0) {
		main_window->setTabIndex(project->getTabIndex());
	}
	else {
		project->createNewTab();
	}
}



TreeItemDepProject::DepProjectInfo::DepProjectInfo() {
	collection = NULL;
	selected = false;
}

TreeItemDepProject::DepProjectInfo::DepProjectInfo(TreeItemDepProject::DepProjectInfo *other) {
	script = other->script;
	fileName = other->fileName;
	tabIndex = other->tabIndex;
	collection = other->collection;
	selected = other->selected;
	childCopy(other);
}

void TreeItemDepProject::DepProjectInfo::clear() {
	while(childInfo.size()) {
		delete childInfo.takeFirst();
	}
}

void TreeItemDepProject::DepProjectInfo::childCopy(TreeItemDepProject::DepProjectInfo *other) {
	clear();
	foreach (DepProjectInfo* cc, other->childInfo) {
		childInfo.append(new DepProjectInfo(cc));
	}
}

QList<int> TreeItemDepProject::DepProjectInfo::getChildsTabs() {
	QList<int> res;
	foreach (DepProjectInfo* ci, childInfo) {
		if(ci->tabIndex >= 0)
			res.append(ci->tabIndex);
		res.append( ci->getChildsTabs() );
	}
	return res;
}
