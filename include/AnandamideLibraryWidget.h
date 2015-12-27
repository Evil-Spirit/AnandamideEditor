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

#ifndef ANANDAMIDELIBRARYWIDGET_H
#define ANANDAMIDELIBRARYWIDGET_H

#include <QWidget>

class CXml_Settings;
class QTreeWidgetItem;

namespace Ui {
class AnandamideLibraryWidget;
}

class AnandamideLibraryWidget : public QWidget
{
	Q_OBJECT
	
public:
	explicit AnandamideLibraryWidget(QWidget *parent = 0);
	~AnandamideLibraryWidget();
	
	void loadSearchTemplates(CXml_Settings* sets);
	void saveSearchTemplates(CXml_Settings* sets);
	
	void setEditable(bool editable);
	
public slots:
	void invalidate();
	void closeLibraries();
	
private slots:
	void library_search(const QString &str, bool block, bool kind);
	
	void on_cbSearchLibrary_editTextChanged(const QString &arg1);
	
	void on_cbSearchLibrary_currentIndexChanged(int index);
	
	void on_btnClearSearch_clicked();
	
	void on_btnAddSearchTemplate_clicked();
	
	void on_btnRemoveSearchTemplate_clicked();
	
	void on_cbSearchBlock_clicked(bool checked);
	
	void on_cbSearchKind_clicked(bool checked);
	
private:
	Ui::AnandamideLibraryWidget *ui;
	
	void saveTreeLibrariesExpanded(CXml_Settings* sets, QTreeWidgetItem* item = NULL);
	void loadTreeLibrariesExpanded(CXml_Settings* sets, QTreeWidgetItem* item = NULL);
};

#endif // ANANDAMIDELIBRARYWIDGET_H
