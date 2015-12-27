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

#ifndef TYPEDIALOG_H
#define TYPEDIALOG_H

#include <QDialog>
#include <AnandamideTypeInfo.h>

namespace Ui {
	class TypeDialog;
}

class QTreeWidgetItem;

class TypeDialog : public QDialog
{
		Q_OBJECT
		
	public:
		
		explicit TypeDialog(QWidget *parent = 0);
		~TypeDialog();
		Anandamide::TypeInfo getType() const;
		void invalidate();
		
	private:
		
		Ui::TypeDialog *ui;
		bool decIdView;
		
		void showAll(QTreeWidgetItem *parent = NULL);
		void hideAll(QTreeWidgetItem *parent = NULL);
		void filterByName(const QString& name, QTreeWidgetItem *parent = NULL);
		bool processEmpty(QTreeWidgetItem *parent = NULL);
		
	private slots:
		
		void itemDoubleClicked(QTreeWidgetItem *item, int col);

		void onHeaderClicked(int section);
		void on_leFilter_textChanged(const QString &arg1);
		void on_btnClearFilter_clicked();
};

#endif // TYPEDIALOG_H
