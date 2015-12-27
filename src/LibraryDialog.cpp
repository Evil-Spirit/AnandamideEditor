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

#include "LibraryDialog.h"
#include "ui_LibraryDialog.h"

LibraryDialog::LibraryDialog(QWidget *parent) : QDialog(parent), ui(new Ui::LibraryDialog) {
	ui->setupUi(this);
	connect(ui->library_tree, SIGNAL(doubleClicked()), this, SLOT(onDoubleClicked()));
}

LibraryDialog::~LibraryDialog()
{
	delete ui;
}

void LibraryDialog::search(const QString &name, Anandamide::Input *input, Anandamide::Output *output) {
	this->input = input;
	this->output = output;
	ui->library_tree->search(name, input, output);
	ui->search->setText(name);
	ui->search->setFocus();
}

const Anandamide::NeuroneDef *LibraryDialog::getSelectedDef() const {
	return ui->library_tree->getSelectedDef();
}

void LibraryDialog::on_search_textChanged(const QString &text) {
	ui->library_tree->search(text, input, output);
}

void LibraryDialog::on_clear_search_clicked() {
	ui->library_tree->search("", input, output);
}

void LibraryDialog::onDoubleClicked() {
	if(getSelectedDef() == NULL) return;
	this->accept();
}
