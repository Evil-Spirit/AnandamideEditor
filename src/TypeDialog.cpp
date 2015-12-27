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

#include "TypeDialog.h"
#include "ui_TypeDialog.h"
#include "TreeItemBase.h"
#include "MainWindow.h"

class TreeItemType : public TreeItemBase {
		
		Anandamide::TypeInfo type;
		
	public:
		
		TreeItemType(TreeItemBase *parent, const Anandamide::TypeInfo &type) : TreeItemBase(parent) {
			this->type = type;
			setText(type.name());
		}
		
		Anandamide::TypeInfo  getType() const {
			return type;
		}
};

TypeDialog::TypeDialog(QWidget *parent) : QDialog(parent), ui(new Ui::TypeDialog) {
	ui->setupUi(this);
	ui->type_tree->setItemDelegate(new MyDelegate(ui->type_tree));
	ui->type_tree->setHeaderHidden(false);
	ui->type_tree->setColumnWidth(0, 270);
	decIdView = true;
	ui->type_tree->header()->setSectionsClickable(true);
	connect(ui->type_tree->header(),
			SIGNAL(sectionClicked(int)),
			SLOT(onHeaderClicked(int)));

	connect(ui->type_tree, SIGNAL(itemDoubleClicked(QTreeWidgetItem *,int)), this, SLOT(itemDoubleClicked(QTreeWidgetItem *, int)));
}

TypeDialog::~TypeDialog() {
	delete ui;
}

Anandamide::TypeInfo TypeDialog::getType() const {
	QTreeWidgetItem *current = ui->type_tree->currentItem();
	TreeItemType *type_item = dynamic_cast <TreeItemType *>(current);
	if(type_item == NULL) return TYPEID(int);
	return type_item->getType();
}

void TypeDialog::invalidate() {
	ui->type_tree->headerItem()->setData(1, Qt::DisplayRole, decIdView ? "Type ID (dec)" : "Type ID (hex)");
	ui->type_tree->clear();
	Anandamide::Script *script = main_window->getScript();
	Anandamide::Libraries *libraries = script->getLibraries();
	
	TreeItemBase *root = new TreeItemBase(ui->type_tree, main_window);
	root->setText("Libraries");
	root->setExpanded(true);
	root->setIcon(0, QIcon("://icons/project.png"));
	
	for(int i=0; i<libraries->getLibrariesCount(); i++) {
		const Anandamide::Library *library = libraries->getLibrary(i);
		if(library->getTypeDefsCount() < 1) continue;
		TreeItemBase *library_item = new TreeItemBase(root, library->getName());
		library_item->setIcon(0, QIcon("://icons/library.png"));
		library_item->setExpanded(true);
		for(int j=0; j<library->getTypeDefsCount(); j++) {
			const Anandamide::TypeDef *type_def = library->getTypeDef(j);
			TreeItemBase *type_item = new TreeItemType(library_item, type_def->getTypeInfo());
			if(type_def->getImage()) type_item->setIcon(0, QIcon(QPixmap::fromImage(*type_def->getImage())));
			type_item->setData(1, Qt::DisplayRole, QString::number(type_def->getTypeInfo().id(), decIdView ? 10 : 16 ));
			QColor typeColor;
			typeColor.setRgbF(type_def->getR(), type_def->getG(), type_def->getB());
			QPixmap pix(16, 16);
			pix.fill(typeColor);
			type_item->setData(1, Qt::DecorationRole, QIcon(pix));
		}
		library_item->sortChildren(0, Qt::AscendingOrder);
	}	
	
	//	ui->type_tree->sortItems(0, Qt::AscendingOrder);
}

void TypeDialog::showAll(QTreeWidgetItem *parent)
{
	if(parent == NULL) {
		for(int i = 0; i < ui->type_tree->topLevelItemCount(); ++i)
			showAll(ui->type_tree->topLevelItem(i));
	}
	else {
		parent->setHidden(false);
		for(int i = 0; i < parent->childCount(); ++i)
			showAll(parent->child(i));
	}
}

void TypeDialog::hideAll(QTreeWidgetItem *parent)
{
	if(parent == NULL) {
		for(int i = 0; i < ui->type_tree->topLevelItemCount(); ++i)
			showAll(ui->type_tree->topLevelItem(i));
	}
	else {
		parent->setHidden(true);
		for(int i = 0; i < parent->childCount(); ++i)
			showAll(parent->child(i));
	}
}

void TypeDialog::filterByName(const QString &name, QTreeWidgetItem *parent)
{
	if(parent == NULL) {
		for(int i = 0; i < ui->type_tree->topLevelItemCount(); ++i)
			filterByName(name, ui->type_tree->topLevelItem(i));
	}
	else {
		bool hide = parent->text(0).indexOf(name, 0, Qt::CaseInsensitive) == -1;
		parent->setHidden(hide);
		if(!hide) {
			showAll(parent);
			return;
		}
		
		for(int i = 0; i < parent->childCount(); ++i)
			filterByName(name, parent->child(i));
	}
}

bool TypeDialog::processEmpty(QTreeWidgetItem *parent)
{
	if(parent == NULL) {
		for(int i = 0; i < ui->type_tree->topLevelItemCount(); ++i)
			processEmpty(ui->type_tree->topLevelItem(i));
		return true;
	}
	else {
		if(parent->childCount() == 0) 
			return parent->isHidden();
		bool empty = true;
		for(int i=0; i<parent->childCount(); i++) {
			empty = processEmpty(parent->child(i)) && empty;
		}
		parent->setHidden(empty);
		if(!parent->isHidden())
			parent->setExpanded(true);
		return empty;
	}
}

void TypeDialog::itemDoubleClicked(QTreeWidgetItem *item, int col) {
	TreeItemType *type_item = dynamic_cast <TreeItemType *>(item);
	if(type_item == NULL) return;
	this->accept();
}

void TypeDialog::onHeaderClicked(int section)
{
	if(section != 1)
		return;
	decIdView = !decIdView;
	invalidate();
}

void TypeDialog::on_leFilter_textChanged(const QString &arg1)
{
	if(arg1 == "") {
		showAll();
	}
	else {
		filterByName(arg1);
		processEmpty();
	}
}

void TypeDialog::on_btnClearFilter_clicked()
{
    ui->leFilter->clear();
}
