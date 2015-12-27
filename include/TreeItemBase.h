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

#ifndef TREEITEMBASE_H
#define TREEITEMBASE_H

//------------------------------------------------------------------------------

#include <QTreeWidgetItem>
#include <QAction>
#include <QStyledItemDelegate>

//------------------------------------------------------------------------------

class MainWindow;

namespace Common {
	class Action;
}

//------------------------------------------------------------------------------

enum {
	TreeItemRoleType = Qt::UserRole,
	TreeItemRoleInstance,
	TreeItemRoleLibrary = Qt::UserRole + 5,
};

//------------------------------------------------------------------------------
//
// class TreeItemBase
//
//------------------------------------------------------------------------------

class TreeItemBase : public QTreeWidgetItem {

	private:
		
		QList <QAction *> actions;
		
		void init();
		
	protected:
		
		MainWindow *main_window;
		void addAction(const char *name, Common::Action *action, const QString &icon);
		QAction* getAction(const char* name);
		void addRenameAction();
		void clearActions();
		TreeItemBase *parentTreeItem();
		
	public:
		
		TreeItemBase(QTreeWidget *parent, MainWindow *main_window);
		TreeItemBase(TreeItemBase *parent);
		TreeItemBase(TreeItemBase *parent, const char *string);
		~TreeItemBase();
		
		void setText(const char *name);
		QString getText() const;
		void setValueText(const char *name);

		void setLibraryType(bool type);
		bool getLibraryType() const;
		
		QList <QAction *> &getActions();
		
		void setEditable(bool state);
		void setDraggable(bool state);
		
		void setChildrenHidden(bool state);
		
		void setDragDataType(const char *type);
		void setDragDataInstance(const char *instance);
		void setDragDataLibrary(const char *library);
		
		void rename();
		
		void addExpandActions();
		void expandAll();
		void collapseAll();
		
		TreeItemBase* getChildByText(const QString& text) const;
		
		virtual bool setName(const char *name);
		virtual const char *getName();
		
		virtual void onDoubleClicked();
		virtual void onClicked();
		
		virtual void updateChildren();
		virtual void clearChildren();
		
		virtual void setEditorData(QWidget *data);
		virtual void getEditorData(QWidget *data);
		virtual QWidget *createEditor(QWidget *parent);
		
		void setActionsEnabled(bool enabled);
};

//------------------------------------------------------------------------------
//
// class MyAction
//
//------------------------------------------------------------------------------

class MyAction : public QAction {
	Q_OBJECT
		
		Common::Action *action;
		
	public:
		MyAction(const QString &text, Common::Action *action);
		~MyAction();
		
	private slots:
		
		void doTrigger();
};

//------------------------------------------------------------------------------
//
// class MyDelegate
//
//------------------------------------------------------------------------------

class MyDelegate : public QStyledItemDelegate {
		
		QTreeWidget *treeWidget;
		
	public:
		
		MyDelegate(QTreeWidget *parent);
	
		QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	
		void setEditorData(QWidget *editor, const QModelIndex &index) const;
		void setModelData(QWidget *editor, QAbstractItemModel *model,const QModelIndex &index) const;
		void updateEditorGeometry(QWidget *editor,const QStyleOptionViewItem &option, const QModelIndex &index) const;
		
		virtual bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);
		
	private:
	
		void showContextMenu(TreeItemBase *item, const QPoint &globalPos);
	
};

//------------------------------------------------------------------------------
//
// class MyPropertiesDelegate
//
//------------------------------------------------------------------------------

class MyPropertiesDelegate : public QStyledItemDelegate {

		QTreeWidget *treeWidget;

	public:

		MyPropertiesDelegate(QTreeWidget *parent);

		QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;

		void setEditorData(QWidget *editor, const QModelIndex &index) const;
		void setModelData(QWidget *editor, QAbstractItemModel *model,const QModelIndex &index) const;
		void updateEditorGeometry(QWidget *editor,const QStyleOptionViewItem &option, const QModelIndex &index) const;

		virtual bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);

	private:

		void showContextMenu(TreeItemBase *item, const QPoint &globalPos);

};



#endif // TREEITEMBASE_H
