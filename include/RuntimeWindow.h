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

#ifndef RUNTIMEWINDOW_H
#define RUNTIMEWINDOW_H

#include <QGraphicsScene>
#include <QMainWindow>
#include <QGraphicsView>
#include "AnandamideAPI.h"
#include <QDebug>

#include "AnandamideEditorGlobal.h"

class RuntimeView : public QGraphicsView {
		Q_OBJECT
	protected:
		
		Anandamide::LogicEditor *editor;
		
//		void mouseMoveEvent(QMouseEvent *event);
//		void mousePressEvent(QMouseEvent *event);
//		void mouseReleaseEvent(QMouseEvent *event);
//		void wheelEvent(QWheelEvent *event);
		
	public:
		
		RuntimeView(QWidget *parent) : QGraphicsView(parent) {
			
		}
		
		
		// QWidget interface
	protected:
		
//		void keyPressEvent(QKeyEvent *);
//		void keyReleaseEvent(QKeyEvent *);
};

//class Mouse : public Anandamide::Neurone {
		
//		static QList <Mouse *> &instances() {
//			static QList <Mouse *> insts;
//			return insts;
//		}
		
//	public:
		
//		Mouse() {
//			instances().append(this);
//		}
		
//		~Mouse() {
//			instances().removeOne(this);
//		}
		
//		static void define(Anandamide::NeuroneDef *def) {
//			def->setKind("Input");
//			def->addEvent("moved");	
//			def->addEvent("press");	
//			def->addEvent("release");
//			def->addOutput <float> ("x", "0");	
//			def->addOutput <float> ("y", "0");	
//			def->addOutput <int> ("button", "0");	
//		}
		
//		void init() {
//		}
		
//		static void doMouseMove(float x, float y) {
//			foreach(Mouse *m, instances()) {
//				if(m->isDummy()) continue;
//				m->getOutput("x")->setValue(Anandamide::Variable(x));
//				m->getOutput("y")->setValue(Anandamide::Variable(y));
//				m->getEvent("moved")->run();
//			}
//		}
		
//		static void doMouseDown(float x, float y, int b) {
//			foreach(Mouse *m, instances()) {
//				if(m->isDummy()) continue;
//				m->getOutput("x")->setValue(Anandamide::Variable(x));
//				m->getOutput("y")->setValue(Anandamide::Variable(y));
//				m->getOutput("button")->setValue(Anandamide::Variable(b));
//				m->getEvent("press")->run();
//			}
//		}
		
//		static void doMouseUp(float x, float y, int b) {
//			foreach(Mouse *m, instances()) {
//				if(m->isDummy()) continue;
//				m->getOutput("x")->setValue(Anandamide::Variable(x));
//				m->getOutput("y")->setValue(Anandamide::Variable(y));
//				m->getOutput("button")->setValue(Anandamide::Variable(b));
//				m->getEvent("release")->run();
//			}
//		}
//};
//enum KeyType {
//	KEY_NULL	= 0,
//	KEY_LEFT	= Qt::Key_Left,
//	KEY_RIGHT	= Qt::Key_Right,
//	KEY_UP		= Qt::Key_Up,
//	KEY_DOWN	= Qt::Key_Down,
//};

//DEFINE_TYPEID(QGraphicsItem *, 1300)


//ENUM_BEGIN(KeyType);
//ENUM_ITEM(KEY_LEFT);
//ENUM_ITEM(KEY_RIGHT);
//ENUM_ITEM(KEY_UP);
//ENUM_ITEM(KEY_DOWN);
//ENUM_END(KeyType, 1301);

//ENUM_BEGIN(Qt::GlobalColor);
////ENUM_ITEM(Qt::white);
//ENUM_ITEM(Qt::black);
//ENUM_ITEM(Qt::red);
//ENUM_ITEM(Qt::darkRed);
//ENUM_ITEM(Qt::green);
//ENUM_ITEM(Qt::darkGreen);
//ENUM_ITEM(Qt::blue);
//ENUM_ITEM(Qt::darkBlue);
//ENUM_ITEM(Qt::cyan);
//ENUM_ITEM(Qt::darkCyan);
//ENUM_ITEM(Qt::magenta);
//ENUM_ITEM(Qt::darkMagenta);
//ENUM_ITEM(Qt::yellow);
//ENUM_ITEM(Qt::darkYellow);
//ENUM_ITEM(Qt::gray);
//ENUM_ITEM(Qt::darkGray);
//ENUM_ITEM(Qt::lightGray);
//ENUM_END(Qt::GlobalColor, 1302);


//class Key : public Anandamide::Neurone {
		
//		static QList <Key *> &instances() {
//			static QList <Key *> insts;
//			return insts;
//		}
		
//	public:
		
//		Key() {
//			instances().append(this);
//		}
		
//		~Key() {
//			instances().removeOne(this);
//		}
		
//		static void define(Anandamide::NeuroneDef *def) {
//			def->setKind("Input");
//			def->addEvent("press");	
//			def->addEvent("release");	
//			def->addParameter <KeyType> ("key", "0");	
//			def->setCreateDummy(false);
//		}
		
//		bool hasCaption() {
//			return true;
//		}
		
//		const char *getCaption() {
//			return getParameter("key")->getValue().getStr();
//		}
		
//		void init() {
//		}
		
//		static void doPress(int key) {
//			foreach(Key *m, instances()) {
//				if(m->isDummy() || m->getParameter("key")->getValue().getInt() != key) continue;
//				m->getEvent("press")->run();
//			}
//		}
		
//		static void doRelease(int key) {
//			foreach(Key *m, instances()) {
//				if(m->isDummy() || m->getParameter("key")->getValue().getInt() != key) continue;
//				m->getEvent("release")->run();
//			}
//		}
		
//};



namespace Anandamide {
	class Script;
	class Libraries;
}

namespace Ui {
	class RuntimeWindow;
}

class ANANDAMIDE_EDITOR_API RuntimeWindow : public QMainWindow
{
		Q_OBJECT
		Anandamide::Script *script;
	public:
		explicit RuntimeWindow(QWidget *parent = 0);
		~RuntimeWindow();
		
		void run(Anandamide::Script *script);
		void message(const char *message);
		void messageExt(const char *message, bool endl, Qt::GlobalColor color, bool bold, bool italic);
		QGraphicsScene *getScene();
		QGraphicsView *getView();
		
	private slots:
		void on_pushButton_clicked();
		
	private:
		Ui::RuntimeWindow *ui;
		QGraphicsScene scene;
};

extern RuntimeWindow *runtime;
void createTestLibrary(Anandamide::Libraries *libraries);

#endif // RUNTIMEWINDOW_H
