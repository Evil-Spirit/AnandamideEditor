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

#include "ConsoleOutWidget.h"
#include "ui_ConsoleOutWidget.h"
#include <QScrollBar>


ConsoleOutWidget::ConsoleOutWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::ConsoleOutWidget)
{
	ui->setupUi(this);
}

ConsoleOutWidget::~ConsoleOutWidget()
{
	delete ui;
}

void ConsoleOutWidget::message(const char *message)
{
	messageExt(message, true, Qt::black, false, false);
}

void ConsoleOutWidget::messageExt(const char *message, bool endl, Qt::GlobalColor color, bool bold, bool italic)
{
	ui->textEdit->textCursor().movePosition(QTextCursor::End);
	QColor cl(color);
	QString msg = QString("<font color=\"%2\">%1</font>").arg(QString::fromLocal8Bit(message)).arg(cl.name(QColor::HexRgb));
	if(bold)
		msg = QString("<b>%1</b>").arg(msg);
	if(italic)
		msg = QString("<i>%1</i>").arg(msg);
	ui->textEdit->insertHtml(msg);
	if(endl)
		ui->textEdit->insertPlainText("\n");
	ui->textEdit->verticalScrollBar()->setValue(ui->textEdit->verticalScrollBar()->maximum());
}

void ConsoleOutWidget::onRunScript()
{
	if(ui->btnBlankNewSession->isChecked())
		ui->textEdit->clear();
}

void ConsoleOutWidget::on_btnClear_clicked()
{
    ui->textEdit->clear();
}
