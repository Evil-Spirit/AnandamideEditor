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

#ifndef ANANDAMIDELIBRARYTREE_H
#define ANANDAMIDELIBRARYTREE_H

#include <QTreeWidget>

namespace Anandamide {
	class Input;
	class Output;
	class NeuroneDef;
}

class TreeItemBase;

enum SearchMode {
	SM_BLOCKNAME = 1,
	SM_KIND = 2
};

class AnandamideLibraryTree : public QTreeWidget {
		
		Q_OBJECT
		
		TreeItemBase *root;
		
	private:
		
		void showAll(QTreeWidgetItem *parent);
		void hideAll(QTreeWidgetItem *parent);
		bool processEmpty(QTreeWidgetItem *parent);
		
		void filterByName(QTreeWidgetItem *parent, const QString &name, int mode = SM_BLOCKNAME);
		void filterByNameType(QTreeWidgetItem *parent, const QString &name, Anandamide::Input *input, Anandamide::Output *output);
		
		
	public:
		
		explicit AnandamideLibraryTree(QWidget *parent = 0);
		const Anandamide::NeuroneDef *getSelectedDef() const;
		
	signals:
		
		void doubleClicked();
		
	public slots:
		
		void itemDoubleClicked(QTreeWidgetItem *tree_item, int column);
		void itemClicked(QTreeWidgetItem *tree_item, int column);
		
		void search(const QString &name, int mode = SM_BLOCKNAME);
		void search(const QString &name, Anandamide::Input *input, Anandamide::Output *output);
		void invalidate();
		void closeLibraries();
};

#endif // ANANDAMIDELIBRARYTREE_H
