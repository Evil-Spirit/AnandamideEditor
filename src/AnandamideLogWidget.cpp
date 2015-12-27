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

#include "AnandamideLogWidget.h"
#include "ui_AnandamideLogWidget.h"

AnandamideLogWidget::AnandamideLogWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::AnandamideLogWidget)
{
	ui->setupUi(this);
}

AnandamideLogWidget::~AnandamideLogWidget()
{
	delete ui;
}

QTextEdit *AnandamideLogWidget::getLogTextEdit()
{
	return ui->logTextEdit;
}
