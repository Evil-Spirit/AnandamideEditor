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

#include "setlogickinddialog.h"
#include "ui_setlogickinddialog.h"


SetLogicKindDialog::SetLogicKindDialog(TreeItemScript *itemScript, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::SetLogicKindDialog)
{
	ui->setupUi(this);
	ui->btnOk->setEnabled(false);
	setTree(itemScript);
}

SetLogicKindDialog::~SetLogicKindDialog()
{
	ui->treeWidget->clear();
	delete ui;
}

const QString &SetLogicKindDialog::getKind() const
{
	return kind_str;
}

void SetLogicKindDialog::setTree(TreeItemScript *itemScript)
{
    ui->treeWidget->clear();
	addTreeItem(itemScript);
}

void SetLogicKindDialog::addTreeItem(TreeItemBase *item, QTreeWidgetItem *parent)
{
	DialogTreeItemType type;
	if(dynamic_cast<TreeItemScript*>(item))
		type = DTIT_SCRIPT;
	else if(dynamic_cast<TreeItemGroup*>(item))
		type = DTIT_GROUP;
	else if(dynamic_cast<TreeItemLogic*>(item))
		type = DTIT_LOGIC;
	else return;
	QTreeWidgetItem* child;
	if(parent) {
		child = new QTreeWidgetItem(parent);
	}
	else {
		child = new QTreeWidgetItem(ui->treeWidget);
	}
	child->setText(0, item->getText());
	child->setIcon(0, item->icon(0));
	child->setData(0, Qt::UserRole, (int)type);
	if(type != DTIT_LOGIC) {
		QFont f;
		f.setBold(true);
		child->setFont(0, f);
		child->setExpanded(true);
	}
	else {
		child->setBackgroundColor(0, Qt::lightGray);
	}
	for(int i = 0; i < item->childCount(); ++i)
		addTreeItem(dynamic_cast<TreeItemBase*>(item->child(i)), child);
}

QString SetLogicKindDialog::getItemKindStr(QTreeWidgetItem *item)
{
	QStringList kindList;
	DialogTreeItemType type = (DialogTreeItemType)item->data(0, Qt::UserRole).toInt();
	QTreeWidgetItem* parent = item;
	while(type == DTIT_GROUP) {
		kindList.prepend(parent->text(0));
		parent = parent->parent();
		type = (DialogTreeItemType)parent->data(0, Qt::UserRole).toInt();
	}
	return kindList.join('/');
}

void SetLogicKindDialog::on_btnOk_clicked()
{
    this->accept();
}

void SetLogicKindDialog::on_btnCancel_clicked()
{
    this->reject();
}

void SetLogicKindDialog::on_treeWidget_itemClicked(QTreeWidgetItem *item, int column)
{
	Q_UNUSED(column)
    DialogTreeItemType type = (DialogTreeItemType)item->data(0, Qt::UserRole).toInt();
	switch (type) {
	case DTIT_SCRIPT:
		ui->btnOk->setEnabled(true);
		kind_str = QString();
		break;
	case DTIT_GROUP:
		ui->btnOk->setEnabled(true);
		kind_str = getItemKindStr(item);
		break;
	case DTIT_LOGIC:
		ui->btnOk->setEnabled(false);
		return;
	}
}
