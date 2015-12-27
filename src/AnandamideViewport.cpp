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

#include "../include/AnandamideViewport.h"
#include "AnandamideDragData.h"
#include "../include/MainWindow.h"
#include "Renderer.h"
#include "../include/TreeItemBase.h"
#include "AnandamideLogic.h"
#include "Str.h"

#include <QColorDialog>
#include <QInputDialog>
#include <QMenu>
#include <QApplication>
#include <QClipboard>
#include <QStatusBar>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QToolTip>
#include <QTimer>
#include <AnandamideDragData.h>

AnandamideViewport::AnandamideViewport(QWidget *parent) : QGLWidget(parent) { //QGLWidget
	setMouseTracking(true);
	setAutoFillBackground(false);
	setAcceptDrops(true);
	drag_neurone = NULL;
	contextNode = NULL;
	contextGroup = NULL;
	wnd = NULL;
	editor = NULL;
	renderer = new Anandamide::Renderer();
	shiftFlag = false;
	editableFlag = true;
	contextLink = new Anandamide::Link();
	
	contextMenu = new QMenu();
//	connect(contextMenu,
//			SIGNAL(triggered(QAction*)),
//			SLOT(onContextMenuTriggered(QAction*)));
//	connect(contextMenu,
//			SIGNAL(aboutToHide()),
//			SLOT(onContextMenuHide()));
	
	actionCreateNewGroup = new QAction(tr("Group blocks"), this);
	actionCreateNewGroup->setEnabled(false);
	actionCreateNewGroup->setIcon(QIcon(":/icons/group_16.png"));
	connect(actionCreateNewGroup,
			SIGNAL(triggered()),
			SLOT(onActionCreateNewGroup()));
	
	actionRemove = new QAction(tr("Remove"), this);
	actionRemove->setIcon(QIcon(":/icons/remove.png"));
	connect(actionRemove,
			SIGNAL(triggered()),
			SLOT(onActionRemove()));
	
	actionRemoveGroupOnly = new QAction(tr("Remove group only"), this);
	actionRemoveGroupOnly->setIcon(QIcon(":/icons/remove_thin_16.png"));
	connect(actionRemoveGroupOnly,
			SIGNAL(triggered()),
			SLOT(onActionRemoveGroupOnly()));
	
	actionEditComment = new QAction(tr("Edit comment"), this);
	actionEditComment->setIcon(QIcon(":/icons/comment_16.png"));
	connect(actionEditComment,
			SIGNAL(triggered()),
			SLOT(onActionEditComment()));
	
	actionEditGroupColor = new QAction(tr("Change color"), this);
	connect(actionEditGroupColor,
			SIGNAL(triggered()),
			SLOT(onActionEditGroupColor()));
	
	actionCopyElements = new QAction(tr("Copy"), this);
	actionCopyElements->setIcon(QIcon(":/icons/copy_16.png"));
	connect(actionCopyElements,
			SIGNAL(triggered()),
			SLOT(onActionCopyElements()));
	
	actionPasteElements = new QAction(tr("Paste"), this);
	actionPasteElements->setIcon(QIcon(":/icons/paste_16.png"));
	actionPasteElements->setEnabled(false);
	connect(actionPasteElements,
			SIGNAL(triggered()),
			SLOT(onActionPasteElements()));
	
	actionCloneElements = new QAction(tr("Clone"), this);
	actionCloneElements->setIcon(QIcon(":/icons/clone.png"));
	connect(actionCloneElements,
			SIGNAL(triggered()),
			SLOT(onActionCloneElements()));
	
	actionAddToGroup = new QAction(tr("Add to group"), this);
	actionAddToGroup->setIcon(QIcon(":/icons/add.png"));
	connect(actionAddToGroup,
			SIGNAL(triggered()),
			SLOT(onActionAddToGroup()));
	
	actionRemoveFromGroup = new QAction(tr("Remove from group"), this);
	actionRemoveFromGroup->setIcon(QIcon(":/icons/minus.png"));
	connect(actionRemoveFromGroup,
			SIGNAL(triggered()),
			SLOT(onActionRemoveFromGroup()));
	
	actionMakeGroupLogic = new QAction(tr("Make logic"), this);
	actionMakeGroupLogic->setIcon(QIcon(":/icons/spark.png"));
	connect(actionMakeGroupLogic,
			SIGNAL(triggered()),
			SLOT(onActionMakeGroupLogic()));
	
	actionExpandLogicToGroup = new QAction(tr("Expand to group"), this);
	actionExpandLogicToGroup->setIcon(QIcon(":/icons/spark.png"));
	connect(actionExpandLogicToGroup,
			SIGNAL(triggered()),
			SLOT(onActionExpandLogicToGroup()));
	
	actionOpenLogic = new QAction(tr("Open logic"), this);
	actionOpenLogic->setIcon(QIcon(":/icons/puzzle_blue.png"));
	connect(actionOpenLogic,
			SIGNAL(triggered()),
			SLOT(onActionOpenLogic()));

	actionUnlinkBegin = new QAction(tr("Unlink source"), this);
	actionUnlinkBegin->setIcon(QIcon(":/icons/left_arror_red.png"));
	connect(actionUnlinkBegin,
			SIGNAL(triggered()),
			SLOT(onActionUnlinkBegin()));
	
	actionUnlinkEnd = new QAction(tr("Unlink recipient"), this);
	actionUnlinkEnd->setIcon(QIcon(":/icons/right_arror_red.png"));
	connect(actionUnlinkEnd,
			SIGNAL(triggered()),
			SLOT(onActionUnlinkEnd()));
}

AnandamideViewport::~AnandamideViewport() {
	delete renderer;
	delete contextMenu;
	delete editor;
	delete contextLink;
}

void AnandamideViewport::setEditor(Anandamide::LogicEditor *editor) {
	this->editor = editor;
}

Anandamide::LogicEditor *AnandamideViewport::getEditor() {return editor;}

void AnandamideViewport::setMainWindow(MainWindow *w)
{
	wnd = w;
}

void AnandamideViewport::setEditable(bool editable)
{
	editableFlag = editable;
	if(editor != NULL)
		editor->setEditable(editable);
	actionCreateNewGroup->setEnabled(editable);
	actionRemove->setEnabled(editable);
	actionRemoveGroupOnly->setEnabled(editable);
	actionPasteElements->setEnabled(editable);
	actionCloneElements->setEnabled(editable);
	actionAddToGroup->setEnabled(editable);
	actionRemoveFromGroup->setEnabled(editable);
	actionMakeGroupLogic->setEnabled(editable);
	actionExpandLogicToGroup->setEnabled(editable);
	actionUnlinkBegin->setEnabled(editable);
	actionUnlinkEnd->setEnabled(editable);
	
	setAcceptDrops(editable);
}

//Создание новой группы
void AnandamideViewport::onActionCreateNewGroup()
{
	editor->createNewGroup();
	editor->onChangeLogic();
	if (wnd)
		wnd->onChangeLogic();
}

//Изменение комментария
void AnandamideViewport::onActionEditComment()
{
	if(!contextGroup && !contextNode)
		return;
	
	if(contextGroup) {
		bool ok;
		QString newComment = QInputDialog::getMultiLineText(NULL,
															tr("Group comment editor"),
															QString(),
															QString::fromLocal8Bit(contextGroup->getComment()), 
															&ok);
		if(ok) {
			contextGroup->setComment(newComment.toLocal8Bit().constData());
			contextGroup->render(renderer);
			editor->onChangeLogic();
		}
	}
	else if(contextNode) {
		bool ok;
		const Anandamide::NeuroneDef *def = contextNode->getDef();
		QString id = "Neurone";
		if(def != NULL) 
			id = "Neurone::" + QString::fromLocal8Bit(def->getLibrary()->getName()) + "::" + QString::fromLocal8Bit(def->getHeader());
		QString newComment = QInputDialog::getMultiLineText(NULL,
															tr("Block comment editor"),
															id,
															QString::fromLocal8Bit(contextNode->getComment()), 
															&ok);
		if(ok) {
			contextNode->setComment(newComment.toLocal8Bit().constData());
			contextNode->render(renderer);
			editor->onChangeLogic();
		}
	}
	contextNode = NULL;
	contextGroup = NULL;
	if (wnd)
		wnd->onChangeLogic();
}

//Изменение цвета группы
void AnandamideViewport::onActionEditGroupColor()
{
	if(!contextGroup)
		return;
	float r, g, b;
	contextGroup->getColor(r, g, b);
	QColor oldColor((int)(r*255), (int)(g*255), (int)(b*255));
	QColor newColor = QColorDialog::getColor(oldColor, NULL, tr("Change group color"));
	if(newColor.isValid()) {
		r = newColor.red() / 255.0;
		g = newColor.green() / 255.0;
		b = newColor.blue() / 255.0;
		contextGroup->setColor(r, g, b);
		contextGroup->render(renderer);
		editor->onChangeLogic();
		contextNode = NULL;
		contextGroup = NULL;
	}
	if (wnd)
		wnd->onChangeLogic();
}

//Удаление групп и блоков
void AnandamideViewport::onActionRemove()
{
	editor->removeSelected();
	editor->onChangeLogic();
	contextNode = NULL;
	contextGroup = NULL;
	if (wnd)
		wnd->onChangeLogic();
}

//Удаление группы вместе с блоками
void AnandamideViewport::onActionRemoveGroupOnly()
{
	if(!contextGroup)
		return;
	editor->removeGroup(contextGroup);
	editor->onChangeLogic();
	contextNode = NULL;
	contextGroup = NULL;
	if (wnd)
		wnd->onChangeLogic();
}

//Копирование групп и блоков
void AnandamideViewport::onActionCopyElements()
{
	Anandamide::Str str;
	editor->copyAsXml(str);
	QApplication::clipboard()->setText(QString::fromLocal8Bit(str.str()));
	contextNode = NULL;
	contextGroup = NULL;
	if (wnd)
		wnd->onChangeLogic();
}

//Вставка групп и блоков
void AnandamideViewport::onActionPasteElements()
{
	Anandamide::Str str = QApplication::clipboard()->text().toLocal8Bit().constData();
	if(editor->pasteAsXml(str, -editor->getCameraPosX(), -editor->getCameraPosY())) {
		editor->onChangeLogic();
	}

	if(wnd)
		wnd->updateProjectTree();
	contextNode = NULL;
	contextGroup = NULL;
	if (wnd)
		wnd->onChangeLogic();
}

//Клонирование групп и блоков
void AnandamideViewport::onActionCloneElements()
{
	if(editor->cloneSelected()) {
		editor->onChangeLogic();
	}
	contextNode = NULL;
	contextGroup = NULL;
	if (wnd)
		wnd->onChangeLogic();
}

//Добавление блоков в группу
void AnandamideViewport::onActionAddToGroup()
{
	editor->addToGroupPrepare();
	if(wnd)
		wnd->statusBar()->showMessage("Please select a group", 3000);
	if (wnd)
		wnd->onChangeLogic();
}

//Удаление блоков из группы
void AnandamideViewport::onActionRemoveFromGroup()
{
	if(editor->removeSelectedFromGroup())
		editor->onChangeLogic();
	if (wnd)
		wnd->onChangeLogic();
}

//Создание логики по группе
void AnandamideViewport::onActionMakeGroupLogic()
{
	editor->makeLogicFromGroup(editor->getSelectedGroup());
	editor->mouseUp(0, 0, 0, false);
	if(wnd) {
		wnd->repaintViewport();
		wnd->updateProjectTree();
	}
	editor->onChangeLogic();
	if (wnd)
		wnd->onChangeLogic();
}

//Создание группы по логике
void AnandamideViewport::onActionExpandLogicToGroup()
{
	editor->makeGroupFromLogic(editor->getSelectedNode());
	editor->mouseUp(0, 0, 0, false);
	editor->onChangeLogic();
	if (wnd)
		wnd->onChangeLogic();
}

//Открыть логику блока LogicDummy
void AnandamideViewport::onActionOpenLogic()
{
	QString libName = QString::fromLocal8Bit(contextNode->getLibraryName());
	QString logicName = QString::fromLocal8Bit(contextNode->getType());
	if(libName != QString()) {
		if(!main_window->openLibrary(libName))
			return;
	}
	main_window->setCurrentLogic(logicName);
}

//Отвязать начало соединения
void AnandamideViewport::onActionUnlinkBegin()
{
	if(!contextLink->is()) return;
	Anandamide::Link copyLink = *contextLink;
	contextLink->remove();
	editor->clearLinks();
	
	if(copyLink.event_action) {
		editor->createActionLink(copyLink.dst, copyLink.dst_index);
	}
	else {
		editor->createInputLink(copyLink.dst, copyLink.dst_index);
	}
	if (wnd)
		wnd->onChangeLogic();
}

//Отвязать конец соединения
void AnandamideViewport::onActionUnlinkEnd()
{
	if(!contextLink->is()) return;
	Anandamide::Link copyLink = *contextLink;
	contextLink->remove();
	editor->clearLinks();
	
	if(copyLink.event_action) {
		editor->createEventLink(copyLink.src, copyLink.src_index);
	}
	else {
		editor->createOutputLink(copyLink.src, copyLink.src_index);
	}
	if (wnd)
		wnd->onChangeLogic();
}

void AnandamideViewport::onContextMenuPopup()
{
	contextMenu->clear();
	switch (contextMenuMode) {
	case CMM_BLOCK: {
		contextMenu->addAction(actionEditComment);
		if(editor->canMakeGroupFromLogic())
			contextMenu->addAction(actionExpandLogicToGroup);
		if(Anandamide::Str(typeid(*contextNode).name()) == "class Anandamide::LogicDummy" && Anandamide::Str(contextNode->getLibraryName()) == "")
			contextMenu->addAction(actionOpenLogic);
		contextMenu->addSeparator();
		contextMenu->addAction(actionCopyElements);
		contextMenu->addAction(actionCloneElements);
		contextMenu->addSeparator();
		if(contectNodeAction != NULL) {
			contextMenu->addSeparator();
		}
		if(editor->canCreateNewGroup()) {
			actionCreateNewGroup->setEnabled(editableFlag);
			contextMenu->addAction(actionCreateNewGroup);
			actionAddToGroup->setEnabled(editor->getGroupsCount() > 0 && editableFlag);
			contextMenu->addAction(actionAddToGroup);
			contextMenu->addSeparator();
		}
		else if(editor->getGroupOfSelectedNodes() != NULL) {
			contextMenu->addAction(actionRemoveFromGroup);
			contextMenu->addSeparator();
		}
		actionRemove->setEnabled(editableFlag);
		contextMenu->addAction(actionRemove);
		break;}
	case CMM_GROUP: {
		contextMenu->clear();
		contextMenu->addAction(actionEditComment);
		contextMenu->addAction(actionEditGroupColor);
		actionMakeGroupLogic->setEnabled(editor->getSelectedGroupsCount() == 1 && editableFlag);
		contextMenu->addAction(actionMakeGroupLogic);
		contextMenu->addSeparator();
		contextMenu->addAction(actionCopyElements);
		contextMenu->addAction(actionCloneElements);
		contextMenu->addSeparator();
		actionRemove->setEnabled(editableFlag);
		contextMenu->addAction(actionRemove);
		contextMenu->addAction(actionRemoveGroupOnly);
		break;}
	case CMM_LINK: {
		contextMenu->clear();
		contextMenu->addAction(actionUnlinkBegin);
		contextMenu->addAction(actionUnlinkEnd);
		break;}
	case CMM_FREE: {
		bool crtNewGrp = editor->canCreateNewGroup();
		actionCreateNewGroup->setEnabled(crtNewGrp && editableFlag);
		contextMenu->clear();
		if(editor->getGroupOfSelectedNodes() != NULL) {
			contextMenu->addAction(actionRemoveFromGroup);
			contextMenu->addSeparator();
		}
		actionRemove->setEnabled(editor->isSomethingSelected() && editableFlag);
		contextMenu->addAction(actionRemove);
		contextMenu->addSeparator();
		actionPasteElements->setEnabled(QApplication::clipboard()->text() != "QApplication::clipboard()->text()" && editableFlag);
		contextMenu->addAction(actionPasteElements);
		contextMenu->addSeparator();
		contextMenu->addAction(actionCreateNewGroup);
		actionAddToGroup->setEnabled(editor->getGroupsCount() > 0 && crtNewGrp && editableFlag);
		contextMenu->addAction(actionAddToGroup);
		break;}
	default:
		return;
	}	
	contextMenu->exec(contextMenuPoint);
}

void AnandamideViewport::onContextMenuTriggered(QAction *action)
{
	Q_UNUSED(action)
	if(needToMouseUp) {
		needToMouseUp = false;
		editor->mouseUp(mousePos.x(), mousePos.y(), 0, mouseCtrl);
	}
}

void AnandamideViewport::onContextMenuHide()
{
	if(needToMouseUp) {
		needToMouseUp = false;
		editor->mouseUp(mousePos.x(), mousePos.y(), 0, mouseCtrl);
		mousePos = this->mapFromGlobal(QCursor::pos());
		editor->mouseDown(mousePos.x(), mousePos.y(), 0, mouseCtrl);
		editor->mouseUp(mousePos.x(), mousePos.y(), 0, mouseCtrl);
	}
}

void AnandamideViewport::dragEnterEvent(QDragEnterEvent *event) {
	try
	{
		if(drag_neurone != NULL)
			return;
		if(event->source() == NULL)
			return;
		
		if(event->mimeData() && event->mimeData()->hasFormat("AnandamideBlock")) {
			Anandamide::BlockDragData bdd;
			bdd.fillFromMimedata(event->mimeData());
			for(int i=0; i<bdd.contents.size(); i++) {
				bdd.content = bdd.contents[i];
				drag_neurone = editor->create(bdd.content.type.toLocal8Bit().constData(), bdd.content.library.toLocal8Bit().constData(), bdd.content.instance.toLocal8Bit().constData());
				if(drag_neurone) bdd.fillNeurone(drag_neurone);
			}
			repaintViewPort();
			event->accept();
			return;
		}
		
		if(event->source()->objectName() != "project_tree" &&
				event->source()->objectName() != "library_tree" &&
				event->source()->objectName() != "objects_tree")
			return;
		
		QTreeWidget *tree = (QTreeWidget *)event->source();
		QList <QTreeWidgetItem *> items = tree->selectedItems();
		foreach(QTreeWidgetItem *item, items) {
			editor->mouseMove(event->pos().x(), event->pos().y());
			
			Anandamide::BlockDragData bdd;
			bdd.content.type = item->data(0, TreeItemRoleType).toString();
			bdd.content.instance = item->data(0, TreeItemRoleInstance).toString();
			bdd.content.metaType = item->data(0, Qt::UserRole + 3).value<QString>();
			bdd.content.ownerName = item->data(0, Qt::UserRole + 4).value<QString>();
			bdd.content.library = item->data(0, TreeItemRoleLibrary).toString();
			bdd.content.itemValue = item->data(0, Qt::DisplayRole).value<QString>();
			
			if(bdd.content.type == "") continue;
			
			drag_neurone = editor->create(bdd.content.type.toLocal8Bit().constData(), bdd.content.library.toLocal8Bit().constData(), bdd.content.instance.toLocal8Bit().constData());
			if(drag_neurone) bdd.fillNeurone(drag_neurone);
			
			repaintViewPort();
			event->accept();
			break;
		}
	}
	catch(const char *msg)
	{
		qDebug()<<msg;
	}
}

void AnandamideViewport::dragMoveEvent(QDragMoveEvent *event) {
	editor->mouseMove(event->pos().x(), event->pos().y());
	repaintViewPort();
}

void AnandamideViewport::dragLeaveEvent(QDragLeaveEvent *event) {
	editor->removeNode(drag_neurone);
	drag_neurone = NULL;
	repaintViewPort();
}

void AnandamideViewport::dropEvent(QDropEvent *event) {
	drag_neurone = NULL;
	editor->mouseUp(event->pos().x(), event->pos().y(), 0);
	repaintViewPort();
	if (wnd)
		wnd->onChangeLogic();
}


void AnandamideViewport::repaintViewPort()
{
	this->update();
}

void AnandamideViewport::initializeGL() {
}

void AnandamideViewport::resizeGL(int w, int h) {
	width = w;
	height = h;
	renderer->setResolution(width, height);
}

void AnandamideViewport::paintGL() {
	//	glPushMatrix();
	//	renderer->clear();
	//	editor->render(width, height);
	//	glPopMatrix();
}

void AnandamideViewport::mouseMoveEvent(QMouseEvent *event) {
	if(editor == NULL) return;
	editor->mouseMove(event->pos().x(), event->pos().y());
	event->accept();
	//this->updateGL();
	
	/*
	QString tooltip = QString::fromLocal8Bit(editor->getInfo(event->pos().x(), event->pos().y()).str());
	setToolTip(tooltip);
	
	if(tooltip == "") {
		QToolTip::hideText();
	}
	*/
	QToolTip::hideText();
	
	
	//------------------Custom neurone mouseMoveEvent------------------------
	if (editor->getSelectedNode()) {
		if (editor->getSelectedNode()->isCustom() && (event->buttons() == Qt::NoButton || event->buttons() == Qt::LeftButton)) {
			editor->getSelectedNode()->onMouseMove(editor->getCursorPos().x,\
												   editor->getCursorPos().y,\
												   event->button(),\
												   event->buttons(),\
												   event->modifiers());
		}
	}
	//-----------------------------------------------------------------------
		
	repaintViewPort();
}

void AnandamideViewport::mousePressEvent(QMouseEvent *event) {
	
	//------------------Custom neurone mousePressEvent------------------------
	
	if (event->buttons() == Qt::NoButton || event->buttons() == Qt::LeftButton) {
	
		QApplication::setActiveWindow(this);
		setFocus();
		Anandamide::Neurone *selected = editor->getSelectedNode();
		int x = editor->getCursorPos().x;
		int y = editor->getCursorPos().y;
		if (selected)
			if (!selected->isActive(x, y) &&\
					editor->getNodeByPos(event->pos().x(), event->pos().y()) != selected)
				selected = 0;
		
		if (!selected && editor->getNodeByPos(event->pos().x(), event->pos().y())) {
			selected = editor->getNodeByPos(event->pos().x(), event->pos().y());
			if (!selected->isCustom() || !selected->isActive(x, y)) {
				selected = 0;
			} else {
				editor->selectNode(selected);
			}
		}
	
		if (selected) {
			if (	selected->isCustom() &&\
					selected->isActive(x, y) &&\
					!(	selected->isActionSelected(x, y) != -1 ||\
						selected->isEventSelected(x, y) != -1 ||\
						selected->isInputSelected(x, y) != -1 ||\
						selected->isOutputSelected(x, y) != -1)	) {
				selected->onMousePress(x, y, event->button(),\
									   event->buttons(), event->modifiers());
				repaintViewPort();
				return;
			}
		}
		
		clearFocus();
		QApplication::setActiveWindow(main_window);
	}
	
	//-----------------------------------------------------------------------
	

	
	int b = -1;
	if(event->button() & Qt::LeftButton) {
		b = 0;
		if(editor->isAddToGroupMode()) {
			if(editor->addSelectedToGroupByPos(event->pos().x(), event->pos().y())) {
				if(wnd)
					wnd->statusBar()->showMessage("Addition to group complete", 3000);
			}
			else {
				if(wnd)
					wnd->statusBar()->showMessage("There is no groups", 3000);
			}
		}
	}
	else if(event->button() & Qt::RightButton) {
		b = 1;
		needToMouseUp = false;
		if(editor->canPopupMenu()) {
			contextMenuPoint = this->mapToGlobal(event->pos());
			contextGroup = NULL;
			contextNode = editor->getNodeByPos(event->pos().x(), event->pos().y());
			contectNodeAction = editor->getActionByPos(event->pos().x(), event->pos().y());
			if(contectNodeAction != NULL)
				contextNode = contectNodeAction->getNeurone();
			*contextLink = editor->getPickedLink();
			if(contextLink->is()) { //Выбрана связь
				b = 0;
//				needToMouseUp = true;
//				mousePos = event->pos();
//				mouseCtrl = event->modifiers() & Qt::ControlModifier;
				contextMenuMode = CMM_LINK;
				QTimer::singleShot(50, this, SLOT(onContextMenuPopup()));
			}
			else if(contextNode) { //Выбран блок
				b = 0;
				needToMouseUp = true;
				mousePos = event->pos();
				mouseCtrl = event->modifiers() & Qt::ControlModifier;
				contextMenuMode = CMM_BLOCK;
				QTimer::singleShot(50, this, SLOT(onContextMenuPopup()));
			}
			else {
				contextGroup = editor->getGroupByPos(event->pos().x(), event->pos().y());
				if(contextGroup) { //Выбрана группа блоков
					b = 0;
					needToMouseUp = true;
					mousePos = event->pos();
					mouseCtrl = event->modifiers() & Qt::ControlModifier;
					contextMenuMode = CMM_GROUP;
					//Change color action prepare
					float r, g, b;
					contextGroup->getColor(r,g,b);
					QColor actColor((int)(r*255), (int)(g*255), (int)(b*255));
					QPixmap actPix(16, 16);
					QPainter p(&actPix);
					p.setBrush(QBrush(actColor));
					p.drawRect(QRectF(0, 0, actPix.width()-1, actPix.height()-1));
					actionEditGroupColor->setIcon(QIcon(actPix));
					QTimer::singleShot(50, this, SLOT(onContextMenuPopup()));
				}
				else { //Свободное поле
					contextMenuMode = CMM_FREE;
					QTimer::singleShot(50, this, SLOT(onContextMenuPopup()));
				}
			}
		}
	}
	else if(event->button() & Qt::MiddleButton) {
		
		b = 2;
		
		QString tooltip = QString::fromLocal8Bit(editor->getInfo(event->pos().x(), event->pos().y()).str());
		if(tooltip != "") {
			QToolTip::showText(event->globalPos(), tooltip);
		}
		
	}
	editor->mouseDown(event->pos().x(), event->pos().y(), b, event->modifiers() & Qt::ControlModifier, needToMouseUp);
	if(needToMouseUp) {
		needToMouseUp = false;
		editor->mouseUp(event->pos().x(), event->pos().y(), b, event->modifiers() & Qt::ControlModifier);
		editor->mouseMove(mousePos.x(), mousePos.y());
		editor->mouseDown(event->pos().x(), event->pos().y(), b, event->modifiers() & Qt::ControlModifier, true);
		editor->mouseUp(event->pos().x(), event->pos().y(), b, event->modifiers() & Qt::ControlModifier);
	}
	event->accept();
	//this->updateGL();
	repaintViewPort();
}

void AnandamideViewport::mouseReleaseEvent(QMouseEvent *event) {
	
	//------------------Custom neurone mousePressEvent------------------------
	Anandamide::Neurone *selected = editor->getSelectedNode();
	if (selected) {
		int x = editor->getCursorPos().x, y = editor->getCursorPos().y;
		if (	selected->isCustom() &&\
				selected->isActive(x, y) &&\
				!(	selected->isActionSelected(x, y) != -1 ||\
					selected->isEventSelected(x, y) != -1 ||\
					selected->isInputSelected(x, y) != -1 ||\
					selected->isOutputSelected(x, y) != -1)	) {
			selected->onMouseRelease(x, y, event->button(),\
									 event->buttons(), event->modifiers());
		}
	}
	//------------------------------------------------------------------------
	
	int b = -1;
	if(event->button() & Qt::LeftButton) b = 0;
	else if(event->button() & Qt::RightButton) b = 1;
	else if(event->button() & Qt::MiddleButton) b = 2;
	editor->mouseUp(event->pos().x(), event->pos().y(), b, event->modifiers() & Qt::ControlModifier);
	
	if(b == 0) {
		Anandamide::Neurone *selected = editor->getSelectedNode();
		if(selected != NULL) {
			QString id = "Neurone::" + QString::fromLocal8Bit(selected->getLibraryName()) + "::" + QString::fromLocal8Bit(selected->getHeader());
		}
	}
	
	event->accept();
	repaintViewPort();
}

void AnandamideViewport::mouseDoubleClickEvent(QMouseEvent *event)
{
	//------------------Custom neurone mouseDbClickEvent------------------------
	Anandamide::Neurone *selected = editor->getSelectedNode();
	if (selected) {
		int x = editor->getCursorPos().x, y = editor->getCursorPos().y;
		if (	selected->isCustom() &&\
				selected->isActive(x, y) &&\
				!(	selected->isActionSelected(x, y) != -1 ||\
					selected->isEventSelected(x, y) != -1 ||\
					selected->isInputSelected(x, y) != -1 ||\
					selected->isOutputSelected(x, y) != -1)	) {
			selected->onMouseDoubleClick(x, y, event->button(),\
										 event->buttons(), event->modifiers());
			repaintViewPort();
			return;
		}
	}	
	//------------------------------------------------------------------------
	
	int b = -1;
	if(event->button() & Qt::LeftButton) b = 0;
	else if(event->button() & Qt::RightButton) b = 1;
	else if(event->button() & Qt::MiddleButton) b = 2;
	
	if(b == 0) {
		Anandamide::Neurone *selected = editor->getSelectedNode();
		if(selected != NULL) {
			bool ok;
			QString id = "Neurone";
			if(selected->getDef() != NULL) 
				id = "Neurone::" + QString::fromLocal8Bit(selected->getLibraryName()) + "::" + QString::fromLocal8Bit(selected->getHeader());
			QString newComment = QInputDialog::getMultiLineText(NULL,
																tr("Block comment editor"),
																id,
																QString::fromLocal8Bit(selected->getComment()), 
																&ok);
			if(ok) {
				selected->setComment(newComment.toLocal8Bit().constData());
				selected->render(renderer);
			}
		}
		else {
			Anandamide::NeuroneGroup* group = editor->getSelectedGroup();
			if(group != NULL) {
				bool ok;
				QString newComment = QInputDialog::getMultiLineText(NULL,
																	tr("Group comment editor"),
																	QString(),
																	QString::fromLocal8Bit(group->getComment()), 
																	&ok);
				if(ok) {
					group->setComment(newComment.toLocal8Bit().constData());
					group->render(renderer);
				}
			}
		}
	}
	
	event->accept();
}

void AnandamideViewport::wheelEvent(QWheelEvent *event) {
	if(event->delta() > 0) editor->mouseWheel(-1.0f, shiftFlag);
	if(event->delta() < 0) editor->mouseWheel(1.0f, shiftFlag);
	event->accept();
	editor->setupCamera(renderer, width, height);
	editor->mouseMove(event->pos().x(), event->pos().y());
	repaintViewPort();
}

void AnandamideViewport::paintEvent(QPaintEvent *event) {
	Q_UNUSED(event)
	
	glDisable(GL_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);
	glPushMatrix();
	renderer->clear();
	editor->render(renderer, width, height, false);
	glPopMatrix();
	glDisable(GL_DEPTH_TEST);
	
	QPainter painter;
	painter.begin(this);
	painter.setRenderHint(QPainter::Antialiasing, true);
	painter.setRenderHint(QPainter::TextAntialiasing, true);
	painter.setRenderHint(QPainter::HighQualityAntialiasing, true);	
	painter.setRenderHint(QPainter::SmoothPixmapTransform, true);	
	renderer->setPainter(&painter);
	
	// matrix
	QMatrix m;
	m.translate(rect().width() / 2.0, rect().height() / 2.0);
	m.scale(1.0 / editor->getCameraZoom(), 1.0 / editor->getCameraZoom());
	m.translate(editor->getCameraPosX(), editor->getCameraPosY());
	painter.setMatrix(m);
	
	editor->render(renderer, width, height, true);
	
	painter.end();
	renderer->setPainter(NULL);
}

void AnandamideViewport::keyPressEvent(QKeyEvent *event)
{
	Anandamide::Neurone *selected = editor->getSelectedNode();
	if (selected) {
		if (selected->isCustom() && selected->isActive(editor->getCursorPos().x,\
													   editor->getCursorPos().y)) {
			selected->onKeyPress(event->key(), event->modifiers(),\
								 event->text().toLocal8Bit().constData());
			repaintViewPort();
			return;
		}
	}
}

void AnandamideViewport::keyReleaseEvent(QKeyEvent *event)
{
	Anandamide::Neurone *selected = editor->getSelectedNode();
	if (selected) {
		if (selected->isCustom() && selected->isActive(editor->getCursorPos().x,\
													   editor->getCursorPos().y)) {
			selected->onKeyRelease(event->key(), event->modifiers(),\
								   event->text().toLocal8Bit().constData());
			repaintViewPort();
			return;
		}
	}
}
