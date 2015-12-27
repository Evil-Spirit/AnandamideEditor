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

#include <QWidget>

#ifndef CONSOLEOUTWIDGET_H
#define CONSOLEOUTWIDGET_H

namespace Ui {
	class ConsoleOutWidget;
}

class ConsoleOutWidget : public QWidget
{
	Q_OBJECT
	
public:
	explicit ConsoleOutWidget(QWidget *parent = 0);
	~ConsoleOutWidget();
	
	void message(const char *message);
	void messageExt(const char *message, bool endl, Qt::GlobalColor color, bool bold, bool italic);
	
public slots:
	void onRunScript();
	
private slots:
	void on_btnClear_clicked();
	
private:
	Ui::ConsoleOutWidget *ui;
};

#endif // CONSOLEOUTWIDGET_H
