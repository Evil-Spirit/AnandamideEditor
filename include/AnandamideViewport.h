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

#ifndef ANANDAMIDEVIEWPORT_H
#define ANANDAMIDEVIEWPORT_H

#include <QGLWidget>
//#include "AnandamideLogic.h"

enum ContextMenuMode {
	CMM_BLOCK,
	CMM_GROUP,
	CMM_LINK,
	CMM_FREE
};

class MainWindow;
class QMouseEvent;
class QWheelEvent;

namespace Anandamide {
	class LogicEditor;
	class Neurone;
	class Action;
	class NeuroneGroup;
	class Link;
	class Renderer;
}

class QMenu;

class AnandamideViewport : public QGLWidget { //QGLWidget 

		Q_OBJECT
		
		MainWindow* wnd;
		int width;
		int height;
		Anandamide::Neurone *drag_neurone;
		Anandamide::Neurone *contextNode;
		Anandamide::Action *contectNodeAction;
		Anandamide::NeuroneGroup *contextGroup;
		Anandamide::Link *contextLink;
		Anandamide::Renderer *renderer;
		bool needToMouseUp;
		bool mouseCtrl;
		QPoint mousePos;
		QAction* actionCreateNewGroup;			//Создание новой группы
		QAction* actionEditComment;				//Изменение комментария
		QAction* actionEditGroupColor;			//Изменение цвета группы
		QAction* actionRemove;					//Удаление групп и блоков
		QAction* actionRemoveGroupOnly;			//Удаление группы без удаления содержимого
		QAction* actionCopyElements;			//Копирование групп и блоков
		QAction* actionPasteElements;			//Вставка групп и блоков
		QAction* actionCloneElements;			//Клонирование групп и блоков
		QAction* actionAddToGroup;				//Добавление блоков в группу
		QAction* actionRemoveFromGroup;			//Удаление блоков из группы
		QAction* actionMakeGroupLogic;			//Создание логики по группе
		QAction* actionExpandLogicToGroup;		//Создание группы по логике
		QAction* actionOpenLogic;				//Открыть логику блока LogicDummy
		QAction* actionBreakPointSet;			//Установить точку останова
		QAction* actionBreakPointRemove;		//Удалить точку останова
		QAction* actionBreakPointEnable;		//Активировать точку останова
		QAction* actionBreakPointDisable;		//Деактивировать точку останова
		QAction* actionUnlinkBegin;				//Отвязать начало соединения
		QAction* actionUnlinkEnd;				//Отвязать конец соединения
		QMenu* contextMenu;
		QPoint contextMenuPoint;
		ContextMenuMode contextMenuMode;
		bool editableFlag;
		void repaintViewPort();
		
	protected:
		
		Anandamide::LogicEditor *editor;
		
		void initializeGL();
		void resizeGL(int width, int height);
		void paintGL();
		
		void mouseMoveEvent(QMouseEvent *event);
		void mousePressEvent(QMouseEvent *event);
		void mouseReleaseEvent(QMouseEvent *event);
		void mouseDoubleClickEvent(QMouseEvent *event);
		void wheelEvent(QWheelEvent *event);
		void paintEvent(QPaintEvent *event);
		void keyPressEvent(QKeyEvent *event);
		void keyReleaseEvent(QKeyEvent *event);
		
	public:
		
		explicit AnandamideViewport(QWidget *parent = 0);
		~AnandamideViewport();
		
		void setEditor(Anandamide::LogicEditor *editor);
		Anandamide::LogicEditor* getEditor();
		void setMainWindow(MainWindow* w);

		bool shiftFlag;
		void setEditable(bool editable);
		
	signals:
		
	public slots:
		
		
		// QWidget interface
		
	private slots:
		void onActionCreateNewGroup();
		void onActionEditComment();
		void onActionEditGroupColor();
		void onActionRemove();
		void onActionRemoveGroupOnly();
		void onActionCopyElements();
		void onActionPasteElements();
		void onActionCloneElements();
		void onActionAddToGroup();
		void onActionRemoveFromGroup();
		void onActionMakeGroupLogic();
		void onActionExpandLogicToGroup();
		void onActionOpenLogic();
		void onActionUnlinkBegin();
		void onActionUnlinkEnd();
		
		void onContextMenuPopup();
		void onContextMenuTriggered(QAction* action);
		void onContextMenuHide();
		
	protected:
		
		void dragEnterEvent(QDragEnterEvent *event);
		void dragMoveEvent(QDragMoveEvent *event);
		void dragLeaveEvent(QDragLeaveEvent *event);
		void dropEvent(QDropEvent *event);
};

#endif // ANANDAMIDEVIEWPORT_H
