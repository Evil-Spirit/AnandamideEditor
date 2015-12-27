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

#include "../include/RuntimeWindow.h"
#include "ui_RuntimeWindow.h"
#include "AnandamideScript.h"
#include "../include/Timer.h"

#include <QtGui>

RuntimeWindow *runtime = NULL;

using namespace Anandamide;

RuntimeWindow::RuntimeWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::RuntimeWindow)
{
	ui->setupUi(this);
	ui->graphicsView->setScene(&scene);
	ui->graphicsView->setMouseTracking(true);
	script = NULL;
	//Hide runtime view
	ui->graphicsView->setVisible(false);
	ui->verticalLayout->addWidget(ui->textEdit);
	ui->dockWidget->close();
	//~Hide runtime view
}

RuntimeWindow::~RuntimeWindow()
{
	delete ui;
}

void RuntimeWindow::run(Anandamide::Script *script) {
	try {
		scene.clear();
		//scene.setSceneRect(-100, -100, 200, 200);
		ui->textEdit->clear();
		script->compile();
		message("Script compiled.");
		qDebug("Script compiled.");
		show();
		message("Script start.");
		qDebug("Script start.");
		Timer timer;
		timer.start();
		script->run();
		double time = timer.elapsed() * 1000.0;
		message(format("Script end (%.2lf ms).", time));
		qDebug("Script end.");
		this->script = script;
		
	} catch (const char *mes) {
		message(mes);
	}
}

void RuntimeWindow::message(const char *message) {
	messageExt(message, true, Qt::black, false, false);
}

void RuntimeWindow::messageExt(const char *message, bool endl, Qt::GlobalColor color, bool bold, bool italic)
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
}

QGraphicsScene *RuntimeWindow::getScene() {
	return &scene;
}

QGraphicsView *RuntimeWindow::getView() {
	return ui->graphicsView;
}

void RuntimeWindow::on_pushButton_clicked()
{
	if(script == NULL) return;
	run(script);
}

//void RuntimeView::mouseMoveEvent(QMouseEvent *event)
//{
//	QPointF point = this->mapToScene(event->pos());
//	Mouse::doMouseMove(point.x(), point.y());
//}

//void RuntimeView::mousePressEvent(QMouseEvent *event)
//{
//	QPointF point = this->mapToScene(event->pos());
//	Mouse::doMouseDown(point.x(), point.y(), event->button());
//}

//void RuntimeView::mouseReleaseEvent(QMouseEvent *event)
//{
//	QPointF point = this->mapToScene(event->pos());
//	Mouse::doMouseUp(point.x(), point.y(), event->button());
//}

//void RuntimeView::wheelEvent(QWheelEvent *event) {
//	//qDebug() << "mouse moved";
//}

void message(const char *text) {
	runtime->message(text);
}

void messageExt(const char *message, bool endl, Qt::GlobalColor color, bool bold, bool italic) {
	runtime->messageExt(message, endl, color, bold, italic);
}

//QGraphicsItem *createRect(float x, float y, float w, float h) {
//	return runtime->getScene()->addRect(QRectF(x, y, w, h));
//}

//QGraphicsItem *createSprite(float x, float y, float w, float h, int sprite) {
//	switch(sprite) {
//		case 0: return runtime->getScene()->addPixmap(QPixmap(":/sprites/grass.png"));
//		case 1: return runtime->getScene()->addPixmap(QPixmap(":/sprites/brick.png"));
//		case 2: return runtime->getScene()->addPixmap(QPixmap(":/sprites/cat.png"));
//		case 3: return runtime->getScene()->addPixmap(QPixmap(":/sprites/cat2.png"));
//	}
			
//	return runtime->getScene()->addPixmap(QPixmap(":/icons/puzzle_green.png"));
//}

//void centerView(float x, float y) {
//	runtime->getView()->centerOn(x, y);
//}


//void setPos(QGraphicsItem *item, float x, float y) {
//	item->setPos(x, y);
//}

//#include "AnandamideStdLib.h"

void createTestLibrary(Libraries *libraries) {
	//Anandamide::Library *library = libraries->createLibrary("AnandamideEditor");
	//if(library == NULL) return;
	
//	library->addType <KeyType> (0.1f, 1.0f, 1.0f, NULL);
//	library->addType <QGraphicsItem *> (0.4f, 0.4f, 1.0f, ADD_IMG(":/icons/puzzle_green.png"));
//	library->addType <Qt::GlobalColor> (0.1f, 1.0f, 0.3f, NULL);
	
//	library->addDefinition("Console", Anandamide::makeNeuroneFunction1("Message", &message, "result", "text"));
//	library->addDefinition("Console", Anandamide::makeNeuroneFunction5("messageExt", &messageExt, "result", "text", "endLine", "color", "bold", "italic"));
//	library->addDefinition("Scene", Anandamide::makeNeuroneFunction4("CreateRect", &createRect, "rect", "x", "y", "w", "h"));
//	library->addDefinition("Scene", Anandamide::makeNeuroneFunction5("CreateSprite", &createSprite, "sprite", "x", "y", "w", "h", "sprite"));
//	library->addDefinition("Scene", Anandamide::makeNeuroneFunction3("SetPos", &setPos, "result", "object", "x", "y"));
//	library->addDefinition("Scene", Anandamide::makeNeuroneFunction2("CenterView", &centerView, "result", "x", "y"));

//	library->addTemplate <Mouse> ("Mouse");
//	library->addTemplate <Key> ("Key");
	
//	QColor c;
}


//void RuntimeView::keyPressEvent(QKeyEvent *event) {
//	Key::doPress(event->key());
//	event->accept();
//}


//void RuntimeView::keyReleaseEvent(QKeyEvent *event) {
//	Key::doRelease(event->key());
//	event->accept();
//}
