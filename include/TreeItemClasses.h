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

#ifndef TREEITEMCLASSES_H
#define TREEITEMCLASSES_H

#include "TreeItemBase.h"

namespace Anandamide {
	class Action;
	class Logic;
	class ActionNeurone;
	class Input;
	class Output;
	class InputNeurone;
	class EventNeurone;
	class Event;
	class OutputNeurone;
	class Variable;
	class Script;
	class Neurone;
}

//------------------------------------------------------------------------------
//
// TreeItemAction
//
//------------------------------------------------------------------------------

class TreeItemAction : public TreeItemBase {

	Anandamide::Action *action;
	Anandamide::Logic *logic;

	void contextRemove();

public:

	TreeItemAction(TreeItemBase *parent, Anandamide::Logic *logic, Anandamide::Action *action);

	virtual bool setName(const char *name);

	virtual const char *getName();

	virtual void onDoubleClicked();

};

//------------------------------------------------------------------------------
//
// TreeItemActions
//
//------------------------------------------------------------------------------

class TreeItemActions: public TreeItemBase {

	Anandamide::Logic *logic;
	Anandamide::ActionNeurone *new_node;

	void contextAdd();

public:

	TreeItemActions(TreeItemBase *parent, Anandamide::Logic *logic);

	void updateChildren();
};

//------------------------------------------------------------------------------
//
// TreeItemInput
//
//------------------------------------------------------------------------------

class TreeItemInput : public TreeItemBase {

	Anandamide::Input *input;
	Anandamide::Logic *logic;

	void contextRemove();

	void contextSetType();

public:

	TreeItemInput(TreeItemBase *parent, Anandamide::Logic *logic, Anandamide::Input *input);

	virtual bool setName(const char *name);

	virtual const char *getName();

	virtual void onDoubleClicked();

};

//------------------------------------------------------------------------------
//
// TreeItemInputs
//
//------------------------------------------------------------------------------

class TreeItemInputs: public TreeItemBase {

	Anandamide::Logic *logic;
	Anandamide::InputNeurone *new_node;

	void contextAdd();

public:

	TreeItemInputs(TreeItemBase *parent, Anandamide::Logic *logic);

	void updateChildren();
};

//------------------------------------------------------------------------------
//
// TreeItemEvent
//
//------------------------------------------------------------------------------

class TreeItemEvent : public TreeItemBase {

	Anandamide::Event *event;
	Anandamide::Logic *logic;


	void contextRemove();

public:

	TreeItemEvent(TreeItemBase *parent, Anandamide::Logic *logic, Anandamide::Event *event);

	virtual bool setName(const char *name);

	virtual const char *getName();

	virtual void onDoubleClicked();

};

//------------------------------------------------------------------------------
//
// TreeItemEvents
//
//------------------------------------------------------------------------------

class TreeItemEvents: public TreeItemBase {

	Anandamide::Logic *logic;
	Anandamide::EventNeurone *new_node;

	void contextAdd();

public:

	TreeItemEvents(TreeItemBase *parent, Anandamide::Logic *logic);

	virtual void onDoubleClicked();

	virtual void onLeftClicked();

	void updateChildren();

};

//------------------------------------------------------------------------------
//
// TreeItemOutput
//
//------------------------------------------------------------------------------

class TreeItemOutput : public TreeItemBase {

	Anandamide::Output *output;
	Anandamide::Logic *logic;

	void contextRemove();

	void contextSetType();

public:

	TreeItemOutput(TreeItemBase *parent, Anandamide::Logic *logic, Anandamide::Output *output);

	virtual bool setName(const char *name);

	virtual const char *getName();

	virtual void onDoubleClicked();

};

//------------------------------------------------------------------------------
//
// TreeItemOutputs
//
//------------------------------------------------------------------------------

class TreeItemOutputs: public TreeItemBase {

	Anandamide::Logic *logic;
	Anandamide::OutputNeurone *new_node;

	void contextAdd();

public:

	TreeItemOutputs(TreeItemBase *parent, Anandamide::Logic *logic);

	void updateChildren();
};


//------------------------------------------------------------------------------
//
// TreeItemVariable
//
//------------------------------------------------------------------------------

class TreeItemVariable : public TreeItemBase {

	Anandamide::Variable *variable;
	Anandamide::Logic *logic;
	TreeItemBase *get_item;
	TreeItemBase *get_item_ptr;
	TreeItemBase *set_item;

	void contextRemove();

	void contextSetType();


public:

	TreeItemVariable(TreeItemBase *parent, Anandamide::Logic *logic, Anandamide::Variable *variable);

	virtual bool setName(const char *name);

	virtual const char *getName();

	virtual void onDoubleClicked();

};

//------------------------------------------------------------------------------
//
// TreeItemVariables
//
//------------------------------------------------------------------------------

class TreeItemVariables: public TreeItemBase {

	Anandamide::Logic *logic;
	Anandamide::Variable *new_node;

	void contextAdd();

public:

	TreeItemVariables(TreeItemBase *parent, Anandamide::Logic *logic);

	void updateChildren();

};


//------------------------------------------------------------------------------
//
// TreeItemLogic
//
//------------------------------------------------------------------------------

class TreeItemScript;
class TreeItemGroup;

class TreeItemLogic : public TreeItemBase {

	friend class TreeItemScript;
	Anandamide::Logic *logic;
	QWidget *widget;
	

	void contextRemove();

	void contextClone();
	
	void contextMove();

public:

	TreeItemLogic(TreeItemBase *parent, Anandamide::Logic *logic);

	void setSelected(bool state);

	void select();

	virtual void onDoubleClicked();

	virtual bool setName(const char *name);

	virtual const char *getName();

	void updateChildren();
	
	void setKindGroup(TreeItemGroup* group);
	
	void setKind(const QString& kind);

	TreeItemScript* getSctiptItem() const;
};

//------------------------------------------------------------------------------
//
// TreeItemScript
//
//------------------------------------------------------------------------------

class TreeItemScript: public TreeItemBase {

	Anandamide::Script *script;
	Anandamide::Logic *new_logic;
	QList<Anandamide::Logic*> subLogics;
	void getSubLogics(TreeItemGroup* group);
	void actionAddBlock();
	void actionAddGroup();

public:

	TreeItemScript(QTreeWidget *parent,MainWindow *main_window, Anandamide::Script *script);
	
	void removeGroup(TreeItemGroup* group);
	
	void removeLogic(TreeItemLogic* itemLogic);

	void updateChildren();

	virtual bool setName(const char *name);

	virtual const char *getName();

	void unselectAll(TreeItemLogic* current_logic = NULL);
};

//------------------------------------------------------------------------------
//
// TreeItemGroup
//
//------------------------------------------------------------------------------

class TreeItemGroup: public TreeItemBase {

	Anandamide::Script *script;
	Anandamide::Logic *new_logic;
	TreeItemScript* getSctiptItem() const;
	void contextRemove();
	void actionAddBlock();
	void actionAddGroup();
	QByteArray baGetText;

public:

	TreeItemGroup(TreeItemBase *parent, const QString& name, Anandamide::Script *script);

	void updateChildren();
	
	virtual bool setName(const char *name);

	virtual const char *getName();
	
	QString getKind() const;
	
	void unselectAll(TreeItemLogic* current_logic = NULL);
};

//------------------------------------------------------------------------------
//
// TreeItemProperty
//
//------------------------------------------------------------------------------

class TreeItemProperty: public TreeItemBase {
	Anandamide::Variable *variable;
	MainWindow* m_window;
public:

	TreeItemProperty(QTreeWidget *parent, MainWindow *main_window, const char *name, Anandamide::Variable *variable);
	
	TreeItemProperty(TreeItemBase *parent, const char *name, Anandamide::Variable *variable, bool readOnly = false);

	virtual void setEditorData(QWidget *editor);

	virtual void getEditorData(QWidget *editor);

	virtual QWidget *createEditor(QWidget *parent);
	
	void toggleBoolValue();

};

//------------------------------------------------------------------------------
//
// TreeItemProperties
//
//------------------------------------------------------------------------------

class TreeItemProperties: public TreeItemBase {

	Anandamide::Neurone *neurone;

	void onSelectionChanged();

public:

	TreeItemProperties(QTreeWidget *parent,MainWindow *main_window);

	void updateChildren();

	void setNeurone(Anandamide::Neurone *neurone);

	Anandamide::Neurone *getNeurone();

	void clearChildren();

};

#endif // TREEITEMCLASSES_H
