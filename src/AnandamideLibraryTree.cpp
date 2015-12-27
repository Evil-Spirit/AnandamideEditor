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

#include "AnandamideLibraryTree.h"

//------------------------------------------------------------------------------

#include "TreeItemBase.h"
#include "MainWindow.h"
#include "AnandamideAPI.h"

#include <QFileDialog>
#include <QDebug>
#include <QTime>

//------------------------------------------------------------------------------
//
// TreeItemLibraryBlock
//
//------------------------------------------------------------------------------

class TreeItemLibraryBlock: public TreeItemBase {
		
		const Anandamide::NeuroneDef *def;
		static QIcon blockIcon;
		
		void contextOpenLogic() {
			main_window->openLibrary(QString::fromLocal8Bit(def->getLibrary()->getName()));
			main_window->setCurrentLogic(QString::fromLocal8Bit(def->getName()));
		}
		
	public:
		
		TreeItemLibraryBlock(TreeItemBase *parent, const Anandamide::NeuroneDef *def, bool scriptLibFlag) : TreeItemBase(parent) {
			this->def = def;
//			setText(def->getHeader());
			setText(def->getName());
			setIcon(0, blockIcon);
			setDraggable(true);
			setDragDataType(def->getName());
			if(def->getLibrary() != NULL) {
				setDragDataLibrary(def->getLibrary()->getName());
			}
			if(scriptLibFlag && def->getLibrary() != NULL)
				addAction("Open logic", Common::Action::create(this, &TreeItemLibraryBlock::contextOpenLogic), ":/icons/puzzle_blue.png");
		}
		
		const Anandamide::NeuroneDef *getDef() const {
			return def;
		}
		
		void onClicked() {
			QString id = "Neurone::" + QString::fromLocal8Bit(def->getLibrary()->getName()) + "::" + QString::fromLocal8Bit(def->getHeader());
		}
		
		static void setBlockIcon() {
			blockIcon = QIcon("://icons/puzzle_blue.png");
		}
};

QIcon TreeItemLibraryBlock::blockIcon;

//------------------------------------------------------------------------------
//
// TreeItemLibrary
//
//------------------------------------------------------------------------------

class TreeItemLibraries;

class TreeItemLibrary: public TreeItemBase {
		
	friend class TreeItemLibraries;
	
		const Anandamide::Library *library;
		static QIcon libraryIcon;
		static QIcon scriptLibraryIcon;
		static QIcon groupIcon;
		
		void contextUnloadLib() {
			Anandamide::Library* lib = const_cast<Anandamide::Library *>(library);
			if(lib) {
				main_window->removeLibrary(lib);
				this->parent()->removeChild(this);
				main_window->repaintViewport();
				main_window->updateInWorkspace();
			}
		}
		
		void setAbsolutePath() {
			QString libFN = QString::fromLocal8Bit(library->getFileName());
			if(!QFileInfo(libFN).isRelative()) return;
			QString new_libFN = QFileInfo(main_window->getFileName()).dir().absoluteFilePath(libFN);
			if(!QFileInfo(new_libFN).exists()) {
				new_libFN = QDir("./").absoluteFilePath(libFN);
				if(!QFileInfo(new_libFN).exists()) return;
			}
			const_cast<Anandamide::Library *>(library)->setFileName(new_libFN.toLocal8Bit().constData());
			setToolTip(0, QString(library->getFileName()));
			
			getAction("Set absolute path")->setVisible(false);
			getAction("Set relative path")->setVisible(true);
		}
		
		void setRelativePath() {
			QString libFN = QString::fromLocal8Bit(library->getFileName());
			if(QFileInfo(libFN).isRelative()) return;
			if(!QFileInfo(libFN).exists()) return;
			QString new_libFN = QFileInfo(main_window->getFileName()).dir().relativeFilePath(libFN);
			const_cast<Anandamide::Library *>(library)->setFileName(new_libFN.toLocal8Bit().constData());
			setToolTip(0, QString(library->getFileName()));
			
			getAction("Set absolute path")->setVisible(true);
			getAction("Set relative path")->setVisible(false);
		}
		
	public:
		
		TreeItemLibrary(TreeItemBase *parent, const Anandamide::Library *library) : TreeItemBase(parent) {
			this->library = library;
			bool scriptLibFlag = (dynamic_cast<Anandamide::ScriptLibrary*>(const_cast<Anandamide::Library*>(library)) != NULL);
			setText(library->getName());
			QString libFileName = QString::fromLocal8Bit(library->getFileName());
			setToolTip(0, libFileName); //getLibFileName
			if(dynamic_cast<const Anandamide::ScriptLibrary*>(library) != NULL) {
				setIcon(0, scriptLibraryIcon);
			}
			else {
				setIcon(0, libraryIcon);
			}
			if(Anandamide::Str(library->getName()) != "Std")
				addAction("Exclude Lib", Common::Action::create(this, &TreeItemLibrary::contextUnloadLib), "://icons/minus.png");
			addExpandActions();
			if(scriptLibFlag) {
				addAction("Set absolute path", Common::Action::create(this, &TreeItemLibrary::setAbsolutePath), "");
				addAction("Set relative path", Common::Action::create(this, &TreeItemLibrary::setRelativePath), "");
				if(QFileInfo(libFileName).isRelative())
					getAction("Set relative path")->setVisible(false);
				else
					getAction("Set absolute path")->setVisible(false);
			}
			
			QMap <QString, TreeItemBase *> kinds;
			QSet<QTreeWidgetItem*> sortItems;
			sortItems << this;
			
			for(int j = 0; j<library->getDefsCount(); j++) {
				const Anandamide::NeuroneDef *def = library->getDef(j);
				if(def->isHidden()) continue;
				QString def_kind = def->getKind();
				
				TreeItemBase *loc_parent = NULL;
				if(def_kind == "") {
					loc_parent = this;
				} else
				if(!kinds.contains(def_kind)) {
					QStringList def_kind_list = def_kind.split('/');
					QString def_kind_path;
					TreeItemBase *parent_library_item = this;
					for(int i = 0; i < def_kind_list.size(); ++i) {
						if(!i)
							def_kind_path = def_kind_list[0];
						else
							def_kind_path += "/" + def_kind_list[i];
						if(!kinds.contains(def_kind_path)) {
							kinds[def_kind_path] = new TreeItemBase(parent_library_item, def_kind_list[i].toLocal8Bit().constData());
							kinds[def_kind_path]->addExpandActions();
							sortItems << kinds[def_kind_path];
							kinds[def_kind_path]->setIcon(0, groupIcon);
							kinds[def_kind_path]->setLibraryType(true);
						}
						parent_library_item = kinds[def_kind_path];
					}
				}
				
				if(loc_parent == NULL) loc_parent = kinds[def_kind];
				
				new TreeItemLibraryBlock(loc_parent, def, scriptLibFlag);
				
				sortItems << loc_parent;
			}
			
			QList<QTreeWidgetItem*> sortItemList = sortItems.toList();
			foreach (QTreeWidgetItem* item, sortItemList) {
				item->sortChildren(0, Qt::AscendingOrder);
			}
		}
		
		static void setLibraryIcon() {
			libraryIcon = QIcon("://icons/library.png");
			scriptLibraryIcon = QIcon("://icons/puzzle_red.png");
			groupIcon = QIcon("://icons/book.png");
		}
};

QIcon TreeItemLibrary::libraryIcon;
QIcon TreeItemLibrary::scriptLibraryIcon;
QIcon TreeItemLibrary::groupIcon;

//------------------------------------------------------------------------------
//
// TreeItemLibraries
//
//------------------------------------------------------------------------------

class TreeItemLibraries: public TreeItemBase {
		
	void contextLoadLib() {
		QString selfilter = "All library types (*.ui *.dll *.and *.adl);; UI Library (*.ui);; Dynamic Link Library (*.dll);; Anandamide Script (*.and);; Dummy Library(*.adl)";
		QString str = QFileDialog::getOpenFileName(0, 
												   "Open library", 
												   dir, 
												   selfilter,
												   &selected_filter);
		if(str.length() > 0) {
			dir = str;
			QString relStr = QDir("./").relativeFilePath(str);
			main_window->getScript()->getLibraries()->loadLibrary(relStr.toLocal8Bit().constData());
			main_window->updateInWorkspace();
			updateChildren();
		}
	}
	QString dir;
	QString selected_filter;
		
public:
	
	TreeItemLibraries(QTreeWidget *parent,MainWindow *main_window) : TreeItemBase(parent, main_window) {
		setText("Libraries");
		setIcon(0, QIcon("://icons/project.png"));
		setExpanded(true);
		addAction("Include Lib", Common::Action::create(this, &TreeItemLibraries::contextLoadLib), "://icons/add.png");
		addExpandActions();
		updateChildren();
		dir = "./";
	}
	
	void updateChildren() {
//		QTime t;
//		t.start();
//		clearChildren();
		if(main_window->getScript() == NULL) return;
		Anandamide::Libraries *libraries = main_window->getScript()->getLibraries();
		
		//Удаление несуществующих
		for(int i = 0; i < this->childCount(); ++i) {
			TreeItemLibrary* itemLib = dynamic_cast<TreeItemLibrary*>(this->child(i));
			if(itemLib) {
				bool findflag = false;
				for(int j = 0; j < libraries->getLibrariesCount(); j++) {
					if(libraries->getLibrary(j) == itemLib->library) {
						findflag = true;
						break;
					}
				}
				if(!findflag) {
					this->removeChild(itemLib);
					delete itemLib;
					i--;
				}
			}
		}
		//Добавление новых
		for(int i = 0; i < libraries->getLibrariesCount(); i++) {
			const Anandamide::Library *libr = libraries->getLibrary(i);
			bool findflag = false;
			for(int j = 0; j < this->childCount(); ++j) {
				TreeItemLibrary* itemLib = dynamic_cast<TreeItemLibrary*>(this->child(j));
				if(itemLib->library == libr) {
					findflag = true;
					break;
				}
			}
			if(!findflag)
				new TreeItemLibrary(this, libr);
		}
//		qDebug() << "TreeItemLibraries::updateChildren()" << t.elapsed() << "ms";
	}
	
	void onClicked() {
	}
		
};

//------------------------------------------------------------------------------
//
// AnandamideLibraryTree
//
//------------------------------------------------------------------------------

void AnandamideLibraryTree::showAll(QTreeWidgetItem *parent) {
	parent->setHidden(false);
	for(int i=0; i<parent->childCount(); i++) {
		showAll(parent->child(i));
	}
}

void AnandamideLibraryTree::hideAll(QTreeWidgetItem *parent)
{
	parent->setHidden(true);
	for(int i=0; i<parent->childCount(); i++) {
		hideAll(parent->child(i));
	}
}

bool AnandamideLibraryTree::processEmpty(QTreeWidgetItem *parent) {
	TreeItemLibraryBlock *block = dynamic_cast <TreeItemLibraryBlock *>(parent);
	if(block != NULL) return block->isHidden();
	
	bool empty = true;
	for(int i=0; i<parent->childCount(); i++) {
		empty = processEmpty(parent->child(i)) && empty;
	}
	parent->setHidden(empty);
	/**Zakhar's code**/
	if(!parent->isHidden())
		parent->setExpanded(true);
	/**~Zakhar's code**/
	return empty;
}

void AnandamideLibraryTree::filterByName(QTreeWidgetItem *parent, const QString &name, int mode) {
	/**Zakhar's code**/
	TreeItemBase *itemBase = dynamic_cast <TreeItemBase *>(parent);
	if(itemBase) {
		if(mode & SM_KIND && itemBase->getLibraryType()) {
			bool hide = itemBase->getText().indexOf(name, 0, Qt::CaseInsensitive) == -1;
			itemBase->setHidden(hide);
			if(!hide) {
				showAll(parent);
				return;
			}
			else
				hideAll(parent);
		}
		if(mode & SM_BLOCKNAME) {
			TreeItemLibraryBlock *block = dynamic_cast <TreeItemLibraryBlock *>(parent);
			if(block) {
				bool hide = QString::fromLocal8Bit(block->getDef()->getHeader()).indexOf(name, 0, Qt::CaseInsensitive) == -1;
				hide &= QString::fromLocal8Bit(block->getDef()->getName()).indexOf(name, 0, Qt::CaseInsensitive) == -1;
				block->setHidden(hide);
			}
		}
	}
	/**~Zakhar's code**/
	/**Native code
	TreeItemLibraryBlock *block = dynamic_cast <TreeItemLibraryBlock *>(parent);
	if(block) {
		//bool hide = name != QString::fromLocal8Bit(block->getDef()->getName());
		bool hide = QString::fromLocal8Bit(block->getDef()->getHeader()).indexOf(name, 0, Qt::CaseInsensitive) == -1;
		hide &= QString::fromLocal8Bit(block->getDef()->getName()).indexOf(name, 0, Qt::CaseInsensitive) == -1;
		block->setHidden(hide);
	}**/
	for(int i=0; i<parent->childCount(); i++) {
		filterByName(parent->child(i), name, mode);
	}
}

void AnandamideLibraryTree::filterByNameType(QTreeWidgetItem *parent, const QString &name, Anandamide::Input *input, Anandamide::Output *output) {
	TreeItemLibraryBlock *block = dynamic_cast <TreeItemLibraryBlock *>(parent);
	if(block) {
		const Anandamide::NeuroneDef *def = block->getDef();
		bool hide = name != "" && QString::fromLocal8Bit(def->getHeader()).indexOf(name, 0, Qt::CaseInsensitive) == -1;
		hide &= QString::fromLocal8Bit(def->getName()).indexOf(name, 0, Qt::CaseInsensitive) == -1;
		if(!hide && input != NULL) {
			hide = true;
			for(int i=0; i<def->getOutputsCount(); i++) {
				if(def->getOutput(i).type_id != input->getVariable().getType().id()) continue;
				hide = false;
				break;
			}
		} else
		if(!hide && output != NULL) {
			hide = true;
			for(int i=0; i<def->getInputsCount(); i++) {
				if(def->getInput(i).type_id != output->getVariable().getType().id()) continue;
				hide = false;
				break;
			}
		}
		block->setHidden(hide);
	}
	for(int i=0; i<parent->childCount(); i++) {
		filterByNameType(parent->child(i), name, input, output);
	}
}

AnandamideLibraryTree::AnandamideLibraryTree(QWidget *parent) : QTreeWidget(parent) {
	TreeItemLibraryBlock::setBlockIcon();
	TreeItemLibrary::setLibraryIcon();
	setItemDelegate(new MyDelegate(this));
	connect(this, SIGNAL(itemDoubleClicked(QTreeWidgetItem *,int)), this, SLOT(itemDoubleClicked(QTreeWidgetItem *, int)));
	connect(this, SIGNAL(itemClicked(QTreeWidgetItem *,int)), this, SLOT(itemClicked(QTreeWidgetItem *, int)));
	
	setDragDropMode(QAbstractItemView::DragOnly);
	root = new TreeItemLibraries(this, main_window);
}

const Anandamide::NeuroneDef *AnandamideLibraryTree::getSelectedDef() const {
	QTreeWidgetItem *current = currentItem();
	TreeItemLibraryBlock *block = dynamic_cast <TreeItemLibraryBlock *>(current);
	if(block == NULL) return NULL;
	return block->getDef();
}

void AnandamideLibraryTree::itemDoubleClicked(QTreeWidgetItem *tree_item, int column) {
	TreeItemBase *item = (TreeItemBase *)tree_item;
	item->onDoubleClicked();
	emit doubleClicked();
}

void AnandamideLibraryTree::itemClicked(QTreeWidgetItem *tree_item, int column) {
	TreeItemBase *item = (TreeItemBase *)tree_item;
	item->onClicked();
}

void AnandamideLibraryTree::search(const QString &name, int mode) {
	if(name == "") {
		showAll(root);
	} else {
		filterByName(root, name, mode);
		processEmpty(root);
	}
}

void AnandamideLibraryTree::search(const QString &name, Anandamide::Input *input, Anandamide::Output *output) {
	filterByNameType(root, name, input, output);
	processEmpty(root);
}

void AnandamideLibraryTree::invalidate() {
	if(!root)
		root = new TreeItemLibraries(this, main_window);
	root->updateChildren();
//	this->sortItems(0, Qt::AscendingOrder);
}

void AnandamideLibraryTree::closeLibraries()
{
	this->clear();
	root = NULL;
}
