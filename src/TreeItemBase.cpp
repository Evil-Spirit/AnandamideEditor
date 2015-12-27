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

#include "TreeItemBase.h"

//------------------------------------------------------------------------------

#include "Action.h"

#include <QEvent>
#include <QLineEdit>
#include <QMenu>
#include <QtGui>

//------------------------------------------------------------------------------
//
// class TreeItemBase
//
//------------------------------------------------------------------------------

void TreeItemBase::init() {
	setDraggable(false);
	setDragDataType("");
	setDragDataInstance("");
	setDragDataLibrary("");
}

void TreeItemBase::addAction(const char *name, Common::Action *action, const QString &icon) {
	MyAction *my_action = new MyAction(name, action);
	my_action->setIcon(QIcon(icon));
	actions.append(my_action);
}

QAction *TreeItemBase::getAction(const char *name)
{
	foreach (QAction* action, actions) {
		if(action->text() == QString::fromLocal8Bit(name))
			return action;
	}
	return NULL;
}

void TreeItemBase::addRenameAction() {
	addAction("Rename", Common::Action::create(this, &TreeItemBase::rename), "://icons/rename.png");
}

void TreeItemBase::addExpandActions()
{
	addAction("Expand all", Common::Action::create(this, &TreeItemBase::expandAll), "://icons/bullet_toggle_plus.png");
	addAction("Collapse all", Common::Action::create(this, &TreeItemBase::collapseAll), "://icons/bullet_toggle_minus.png");
}

void TreeItemBase::clearActions()
{
	while(actions.size())
		delete actions.takeLast();
}

TreeItemBase *TreeItemBase::parentTreeItem() {
	return (TreeItemBase *)parent();
}

TreeItemBase::TreeItemBase(QTreeWidget *parent, MainWindow *main_window) : QTreeWidgetItem(parent, QStringList("<empty>")) {
	this->main_window = main_window;
	init();
}

TreeItemBase::TreeItemBase(TreeItemBase *parent) : QTreeWidgetItem(parent, QStringList("<empty>")) {
	this->main_window = parent->main_window;
	init();
}

TreeItemBase::TreeItemBase(TreeItemBase *parent, const char *string) : QTreeWidgetItem(parent, QStringList(QString::fromLocal8Bit(string))) {
	this->main_window = parent->main_window;
	init();
}

TreeItemBase::~TreeItemBase()
{
	clearActions();
}

void TreeItemBase::onDoubleClicked() {
	
}

void TreeItemBase::onClicked() {
	
}

void TreeItemBase::setText(const char *name) {
	setData(0, Qt::DisplayRole, QVariant(QString::fromLocal8Bit(name)));
}

QString TreeItemBase::getText() const
{
	return this->data(0, Qt::DisplayRole).toString();
}

void TreeItemBase::setValueText(const char *name) {
	setData(1, Qt::DisplayRole, QVariant(QString::fromLocal8Bit(name)));
}

void TreeItemBase::setLibraryType(bool type)
{
	this->setData(0, Qt::UserRole+111, type);
}

bool TreeItemBase::getLibraryType() const
{
	return this->data(0, Qt::UserRole+111).toBool();
}

QList<QAction *> &TreeItemBase::getActions() {
	return actions;
}

void TreeItemBase::setEditable(bool state) {
	if(state) {
		setFlags(flags() | Qt::ItemIsEditable);
	} else {
		setFlags(flags() & ~Qt::ItemIsEditable);
	}
}

void TreeItemBase::setDraggable(bool state) {
	if(state) {
		setFlags(flags() | Qt::ItemIsDragEnabled);
	} else {
		setFlags(flags() & ~Qt::ItemIsDragEnabled);
	}
}

bool TreeItemBase::setName(const char *name) {
	return true;
}

const char *TreeItemBase::getName() {
	return "<noname>";
}

void TreeItemBase::updateChildren() {
	
}

void TreeItemBase::clearChildren() {
	while(childCount()) {
		QTreeWidgetItem* child = this->child(0);
		this->removeChild(child);
		delete child;
	}
}

void TreeItemBase::setChildrenHidden(bool state) {
	for(int i=0; i<childCount(); i++) {
		this->child(i)->setHidden(state);
	}
	
}

void TreeItemBase::setEditorData(QWidget *data) {
	
}

void TreeItemBase::getEditorData(QWidget *data) {
	
}

QWidget *TreeItemBase::createEditor(QWidget *parent) {
	return NULL;
}

void TreeItemBase::setActionsEnabled(bool enabled)
{
	foreach (QAction* action, actions) {
		action->setEnabled(enabled);
	}
	for(int i = 0; i < this->childCount(); ++i) {
		TreeItemBase* child = dynamic_cast<TreeItemBase*>(this->child(i));
		if(child == NULL) continue;
		child->setActionsEnabled(enabled);
	}
}

void TreeItemBase::setDragDataType(const char *type) {
	setData(0, TreeItemRoleType, QVariant(QString::fromLocal8Bit(type)));
}

void TreeItemBase::setDragDataInstance(const char *instance) {
	setData(0, TreeItemRoleInstance, QVariant(QString::fromLocal8Bit(instance)));
}

void TreeItemBase::setDragDataLibrary(const char *library) {
	setData(0, TreeItemRoleLibrary, QVariant(QString::fromLocal8Bit(library)));
}

void TreeItemBase::rename() {
	setEditable(true);
	this->treeWidget()->editItem(this, 0);
	setEditable(false);
}

void TreeItemBase::expandAll()
{
	this->setExpanded(true);
	for(int i = 0; i < this->childCount(); ++i) {
		TreeItemBase* child = dynamic_cast<TreeItemBase*>(this->child(i));
		if(child == NULL) continue;
		child->expandAll();
	}
}

void TreeItemBase::collapseAll()
{
	this->setExpanded(false);
	for(int i = 0; i < this->childCount(); ++i) {
		TreeItemBase* child = dynamic_cast<TreeItemBase*>(this->child(i));
		if(child == NULL) continue;
		child->collapseAll();
	}
}

TreeItemBase *TreeItemBase::getChildByText(const QString &text) const
{
	for(int i = 0; i < this->childCount(); ++i) {
		TreeItemBase* child = dynamic_cast<TreeItemBase*>(this->child(i));
		if(child) {
			if(child->getText() == text)
				return child;
		}
	}
	return NULL;
}

//------------------------------------------------------------------------------
//
// class MyAction
//
//------------------------------------------------------------------------------

MyAction::MyAction(const QString &text, Common::Action *action) : QAction(text, NULL) {
	this->action = action;
	connect(this, SIGNAL(triggered(bool)), this, SLOT(doTrigger()));
}

MyAction::~MyAction() {
	delete action;
}

void MyAction::doTrigger() {
	if(action == NULL) return;
	action->run();
}

//------------------------------------------------------------------------------
//
// class MyDelegate
//
//------------------------------------------------------------------------------

MyDelegate::MyDelegate(QTreeWidget *parent) {
	treeWidget = parent;
}

QWidget *MyDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const {
	if(index.column() == 0) {
		return new QLineEdit(parent);
	} else
	if(index.column() == 1) {
		TreeItemBase *item = (TreeItemBase *)index.internalPointer();
		return item->createEditor(parent);
	}
	return NULL;
}

void MyDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
	TreeItemBase *item = (TreeItemBase *)index.internalPointer();
	if(index.column() == 0) {
		QLineEdit *line_edit = (QLineEdit *)editor;
		line_edit->setText(QString::fromLocal8Bit(item->getName()));
		line_edit->selectAll();
	} else
		if(index.column() == 1) {
			item->setEditorData(editor);
		}
}

void MyDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const {
	TreeItemBase *item = (TreeItemBase *)index.internalPointer();
	if(index.column() == 0) {
		QLineEdit *line_edit = (QLineEdit *)editor;
		item->setName(line_edit->text().toLocal8Bit().constData());
		item->setText(item->getName());
	} else
		if(index.column() == 1) {
			item->getEditorData(editor);
		}
}

void MyDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const {
	editor->setGeometry(option.rect);
}

bool MyDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) {
	QMouseEvent* mouseEvent = NULL;
	
	if (event->type() == QEvent::MouseButtonPress) {
		mouseEvent = static_cast <QMouseEvent*> (event);
	}
	
	if (mouseEvent && mouseEvent->button() == Qt::RightButton) {
		TreeItemBase *item = (TreeItemBase *)treeWidget->itemAt(mouseEvent->pos());
		showContextMenu(item, mouseEvent->globalPos());
		return true;
	}
	
	return QAbstractItemDelegate::editorEvent(event, model, option, index);
}

void MyDelegate::showContextMenu(TreeItemBase *item, const QPoint &globalPos) {
	if(item->getActions().size() == 0) return;
	QMenu menu;
	menu.addActions(item->getActions());
	menu.exec(globalPos);
}

//------------------------------------------------------------------------------
//
// class MyPropertiesDelegate
//
//------------------------------------------------------------------------------

MyPropertiesDelegate::MyPropertiesDelegate(QTreeWidget *parent) {
	treeWidget = parent;
}

QWidget *MyPropertiesDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const {
	if(index.column() == 1) {
		TreeItemBase *item = (TreeItemBase *)index.internalPointer();
		return item->createEditor(parent);
	}
	return NULL;
}

void MyPropertiesDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
	TreeItemBase *item = (TreeItemBase *)index.internalPointer();
	if(index.column() == 1) {
		item->setEditorData(editor);
	}
}

void MyPropertiesDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const {
	TreeItemBase *item = (TreeItemBase *)index.internalPointer();
	if(index.column() == 1) {
		item->getEditorData(editor);
	}
}

void MyPropertiesDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const {
	editor->setGeometry(option.rect);
}

bool MyPropertiesDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) {
	QMouseEvent* mouseEvent = NULL;

	if (event->type() == QEvent::MouseButtonPress) {
		mouseEvent = static_cast <QMouseEvent*> (event);
	}

	if (mouseEvent && mouseEvent->button() == Qt::RightButton) {
		TreeItemBase *item = (TreeItemBase *)treeWidget->itemAt(mouseEvent->pos());
		showContextMenu(item, mouseEvent->globalPos());
		return true;
	}

	return QAbstractItemDelegate::editorEvent(event, model, option, index);
}

void MyPropertiesDelegate::showContextMenu(TreeItemBase *item, const QPoint &globalPos) {
	if(item->getActions().size() == 0) return;
	QMenu menu;
	menu.addActions(item->getActions());
	menu.exec(globalPos);
}
