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

#include "AnandamideLibraryWidget.h"
#include "ui_AnandamideLibraryWidget.h"

#include "XmlSettings.h"
#include "TreeItemBase.h"

AnandamideLibraryWidget::AnandamideLibraryWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::AnandamideLibraryWidget)
{
	ui->setupUi(this);
}

AnandamideLibraryWidget::~AnandamideLibraryWidget()
{
	delete ui;
}

void AnandamideLibraryWidget::loadSearchTemplates(CXml_Settings *sets)
{
	sets->beginGroup("SearchTemplates");
	int cnt = sets->value("count", 0).toInt();
	int curr = sets->value("current", -1).toInt();
	ui->cbSearchLibrary->clear();
	for(int i = 0; i < cnt; ++i) {
		sets->beginGroup(QString("Template_%1").arg(i));
		QString str = sets->value("string").toString();
		int mode = sets->value("mode").toInt();
		sets->endGroup();
		ui->cbSearchLibrary->addItem(str);
		ui->cbSearchLibrary->setItemData(i, mode);
	}
	ui->cbSearchLibrary->setCurrentIndex(curr);
	sets->endGroup();
	
	//	sets->beginGroup("LibrariesExpanded");
	//	loadTreeLibrariesExpanded(sets);
	//	sets->endGroup();
}

void AnandamideLibraryWidget::saveSearchTemplates(CXml_Settings *sets)
{
	sets->beginGroup("SearchTemplates");
	int cnt = ui->cbSearchLibrary->count();
	sets->setValue("count", cnt);
	sets->setValue("current", ui->cbSearchLibrary->currentIndex());
	for(int i = 0; i < cnt; ++i) {
		sets->beginGroup(QString("Template_%1").arg(i));
		sets->setValue("string", ui->cbSearchLibrary->itemText(i));
		sets->setValue("mode", ui->cbSearchLibrary->itemData(i).toInt());
		sets->endGroup();
	}
	sets->endGroup();
	
	//	sets->beginGroup("LibrariesExpanded");
	//	saveTreeLibrariesExpanded(sets);
	//	sets->endGroup();
}

void AnandamideLibraryWidget::setEditable(bool editable)
{
	for(int i = 0; i < ui->library_tree->topLevelItemCount(); ++i) {
		TreeItemBase* child = dynamic_cast<TreeItemBase*>(ui->library_tree->topLevelItem(i));
		if(child == NULL) continue;
		child->setActionsEnabled(editable);
	}
}

void AnandamideLibraryWidget::invalidate()
{
	ui->library_tree->invalidate();
}

void AnandamideLibraryWidget::closeLibraries()
{
	ui->library_tree->closeLibraries();
}

void AnandamideLibraryWidget::library_search(const QString &str, bool block, bool kind)
{
	int mode = 0;
	if(block)
		mode |= SM_BLOCKNAME;
	if(kind)
		mode |= SM_KIND;
	ui->library_tree->search(str, mode);
}

void AnandamideLibraryWidget::on_cbSearchLibrary_editTextChanged(const QString &arg1)
{
    library_search(arg1, ui->cbSearchBlock->isChecked(), ui->cbSearchKind->isChecked());
}

void AnandamideLibraryWidget::on_cbSearchLibrary_currentIndexChanged(int index)
{
	int mode = ui->cbSearchLibrary->itemData(index).toInt();
	if(index < 0)
		mode = SM_BLOCKNAME;
	ui->cbSearchBlock->setChecked(mode & SM_BLOCKNAME);
	ui->cbSearchKind->setChecked(mode & SM_KIND);
	ui->library_tree->search(ui->cbSearchLibrary->currentText(), mode);
}

void AnandamideLibraryWidget::on_btnClearSearch_clicked()
{
    ui->cbSearchLibrary->setCurrentIndex(-1);
}

void AnandamideLibraryWidget::on_btnAddSearchTemplate_clicked()
{
	if(ui->cbSearchLibrary->currentText() == QString())
		return;
	int mode = 0;
	if(ui->cbSearchBlock->isChecked())
		mode |= SM_BLOCKNAME;
	if(ui->cbSearchKind->isChecked())
		mode |= SM_KIND;
	int ft = ui->cbSearchLibrary->findText(ui->cbSearchLibrary->currentText());
	if(ft >= 0)
		ui->cbSearchLibrary->removeItem(ft);
	ui->cbSearchLibrary->addItem(ui->cbSearchLibrary->currentText());
	ui->cbSearchLibrary->setItemData(ui->cbSearchLibrary->count()-1, mode);
	on_cbSearchLibrary_currentIndexChanged(ui->cbSearchLibrary->count()-1);
}

void AnandamideLibraryWidget::on_btnRemoveSearchTemplate_clicked()
{
	if(ui->cbSearchLibrary->currentIndex() < 0)
		return;
	ui->cbSearchLibrary->removeItem(ui->cbSearchLibrary->currentIndex());
}

void AnandamideLibraryWidget::on_cbSearchBlock_clicked(bool checked)
{
	if(!checked)
		ui->cbSearchKind->setChecked(true);
	library_search(ui->cbSearchLibrary->currentText(), ui->cbSearchBlock->isChecked(), ui->cbSearchKind->isChecked());
}

void AnandamideLibraryWidget::on_cbSearchKind_clicked(bool checked)
{
	if(!checked)
		ui->cbSearchBlock->setChecked(true);
	library_search(ui->cbSearchLibrary->currentText(), ui->cbSearchBlock->isChecked(), ui->cbSearchKind->isChecked());
}

void AnandamideLibraryWidget::saveTreeLibrariesExpanded(CXml_Settings *sets, QTreeWidgetItem *item)
{
	int cnt = item == NULL ? ui->library_tree->topLevelItemCount() : item->childCount();
	sets->setValue("count", cnt);
	for(int i = 0; i < cnt; ++i) {
		QTreeWidgetItem* child = item == NULL ? ui->library_tree->topLevelItem(i) : item->child(i);
		sets->beginGroup(QString("Item_%1").arg(i));
		sets->setValue("expanded", child->isExpanded());
		saveTreeLibrariesExpanded(sets, child);
		sets->endGroup();
	}
}

void AnandamideLibraryWidget::loadTreeLibrariesExpanded(CXml_Settings *sets, QTreeWidgetItem *item)
{
	int cnt = item == NULL ? ui->library_tree->topLevelItemCount() : item->childCount();
	int cntXml = sets->value("count", 0).toInt();
	cnt = qMin(cnt, cntXml);
	for(int i = 0; i < cnt; ++i) {
		QTreeWidgetItem* child = item == NULL ? ui->library_tree->topLevelItem(i) : item->child(i);
		sets->beginGroup(QString("Item_%1").arg(i));
		child->setExpanded(sets->value("expanded").toBool());
		loadTreeLibrariesExpanded(sets, child);
		sets->endGroup();
	}
}
