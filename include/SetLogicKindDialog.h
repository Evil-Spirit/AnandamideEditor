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

#ifndef SETLOGICKINDDIALOG_H
#define SETLOGICKINDDIALOG_H

#include "TreeItemClasses.h"
#include <QDialog>

namespace Ui {
class SetLogicKindDialog;
}

enum DialogTreeItemType {
	DTIT_SCRIPT,
	DTIT_GROUP,
	DTIT_LOGIC
};

class SetLogicKindDialog : public QDialog
{
	Q_OBJECT
	
	QString kind_str;
	void setTree(TreeItemScript* itemScript);
	void addTreeItem(TreeItemBase* item, QTreeWidgetItem* parent = NULL);
	QString getItemKindStr(QTreeWidgetItem* item);
	
public:
	explicit SetLogicKindDialog(TreeItemScript* itemScript, QWidget *parent = 0);
	~SetLogicKindDialog();
	
	const QString& getKind() const;
	
private slots:
	
	void on_btnOk_clicked();
	
	void on_btnCancel_clicked();
	
	void on_treeWidget_itemClicked(QTreeWidgetItem *item, int column);
	
private:
	Ui::SetLogicKindDialog *ui;
};

#endif // SETLOGICKINDDIALOG_H
