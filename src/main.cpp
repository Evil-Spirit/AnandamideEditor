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

#include <QApplication>
#include "AnandamideEditor.h"
#include "Anandamide.h"
#include "AnandamideLibrary.h"

#include "MainWindow.h"
#include <QDebug>
#include <QTextCodec>
#include <QGLWidget>
#include <QFile>
#include <QDir>

using namespace Anandamide;
int main(int argc, char *argv[]) {
	
	QApplication a(argc, argv);
	
	qDebug() << "Neurone size : " << sizeof(Anandamide::Neurone);
	
	int result = 0;
	try {
		QGLFormat fmt;
		fmt.setSampleBuffers(true);
		fmt.setSamples(16);
		QGLFormat::setDefaultFormat(fmt);

		QDir::setCurrent(QCoreApplication::applicationDirPath());

		main_window = new MainWindow();
		qDebug() << "main_window" << main_window;
		runtime = new RuntimeWindow();

		main_window->show();
		
		if(argc > 1) {
			if(main_window->openFile(QString::fromLocal8Bit(argv[1]))) {
				if(argc > 2) {
					main_window->setCurrentLogic(QString::fromLocal8Bit(argv[2]));
				}
				main_window->closeAllTabs(true, main_window->getCurrentTab());
			}
		}
		
		result = a.exec();
		
//		Libraries::staticUnloadAll();
		
		delete type_dialog;
		delete library_dialog;
		delete runtime;
		delete main_window;

	} catch(const char *message) {
		qDebug() << message;
	}

	return result;
}
