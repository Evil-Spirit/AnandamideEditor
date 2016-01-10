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

#include "TreeItemClasses.h"
#include "MainWindow.h"

#include "RuntimeWindow.h"
#include "LibraryDialog.h"
#include "TypeDialog.h"
#include "AnandamideTypeInfo.h"

#include <QComboBox>
#include <QLineEdit>
#include <QSpinBox>

using namespace Anandamide;

//------------------------------------------------------------------------------
//
// TreeItemAction
//
//------------------------------------------------------------------------------

#define FLT_MAX 1e10
#define DBL_MAX 1e20

void TreeItemAction::contextRemove() {
	logic->removeAction(action);
	if(this->main_window->getEditor())
		this->main_window->getEditor()->selectNode(NULL);
	this->main_window->repaintViewport();
	this->main_window->onChangeLogic();
	parentTreeItem()->updateChildren();
}

TreeItemAction::TreeItemAction(TreeItemBase *parent, Anandamide::Logic *logic, Anandamide::Action *action) : TreeItemBase(parent) {
	this->logic = logic;
	this->action = action;
	setText(action->getName());
	setIcon(0, QIcon("://icons/action.png"));
	addRenameAction();
	addAction("Remove", Common::Action::create(this, &TreeItemAction::contextRemove), "://icons/remove.png");
}

bool TreeItemAction::setName(const char *name) {
//	script->renameNodes(logic->getName(), name);
//	logic->setName(name);
//	setDragDataType(logic->getName());
//	this->main_window->repaintViewport();
//	Str oldName(action->getName());
	bool result = logic->renameAction(action->getName(), name);
	if(result) {
//		logic->getScript()
		this->main_window->repaintViewport();
		this->main_window->onChangeLogic();
	}
	return result;
}

const char *TreeItemAction::getName() {
	return action->getName();
}

void TreeItemAction::onDoubleClicked() {
	if(this->main_window->getEditor())
		this->main_window->getEditor()->focusNode(logic->getActionNode(action));
	this->main_window->repaintViewport();
}

//------------------------------------------------------------------------------
//
// TreeItemActions
//
//------------------------------------------------------------------------------

void TreeItemActions::contextAdd() {
	using namespace Anandamide;
	int index = 0;
	while(logic->getAction(format("action_%d", index)) != NULL) index++;
	new_node = new Anandamide::ActionNeurone(format("action_%d", index), logic);
	bool first = true;
	int pos_x = -240;
	int max_y = -160;
	for(int i=0; i<logic->getNodesCount(); i++) {
		Anandamide::Neurone *node = &logic->getNode(i);

		if(node == new_node) continue;

		if (dynamic_cast <Anandamide::InputNeurone *> (node) == NULL)
			if (dynamic_cast <Anandamide::ActionNeurone *> (node) == NULL) continue;

		int pos_y = node->getPosY() + node->getHeight() + 8;
		if(first) {
			max_y = pos_y;
			pos_x = node->getPosX();
			first = false;
		} else {
			if(max_y < pos_y) {
				max_y = pos_y;
				pos_x = node->getPosX();
			}
		}
	}
	new_node->setPos(pos_x, max_y);
	updateChildren();
	this->main_window->repaintViewport();
	this->main_window->onChangeLogic();
}

TreeItemActions::TreeItemActions(TreeItemBase *parent, Anandamide::Logic *logic) : TreeItemBase(parent) {
	this->logic = logic;
	setText("Actions");
	setIcon(0, QIcon("://icons/action.png"));
	addAction("Add Action", Common::Action::create(this, &TreeItemActions::contextAdd), "://icons/add.png");
	new_node = NULL;
	updateChildren();
}

void TreeItemActions::updateChildren() {
	clearChildren();
	for(int i = 0; i<logic->getActionsCount(); i++) {
		TreeItemAction *item = new TreeItemAction(this, logic, &logic->getAction(i));
		if(new_node && new_node->getLogicAction() == &logic->getAction(i)) {
			this->setExpanded(true);
			item->rename();
		}
	}
	new_node = NULL;
}

//------------------------------------------------------------------------------
//
// TreeItemInput
//
//------------------------------------------------------------------------------


void TreeItemInput::contextRemove() {
	logic->removeInput(input);
	if(this->main_window->getEditor())
		this->main_window->getEditor()->selectNode(NULL);
	this->main_window->repaintViewport();
	this->main_window->onChangeLogic();
	parentTreeItem()->updateChildren();
}

void TreeItemInput::contextSetType() {
	using namespace Anandamide;
	TypeInfo type;
	if(selectTypeDialog(type)) {
		Anandamide::Neurone *input_node = logic->getInputNode(input);
		Parameter *type_param = input_node->getParameter("type");
		if(type_param) {
			type_param->setValue(Variable(type));
			this->main_window->onChangeLogic();
		}
	}

	// hack for updating properties when corresponding input neurone currently selected
	if(this->main_window->getEditor())
		if(this->main_window->getEditor()->getOnSelectionChanged()) {
			this->main_window->getEditor()->getOnSelectionChanged()->run();
		}
}

TreeItemInput::TreeItemInput(TreeItemBase *parent, Anandamide::Logic *logic, Anandamide::Input *input) : TreeItemBase(parent) {
	this->logic = logic;
	this->input = input;
	setText(input->getName());
	setIcon(0, QIcon("://icons/input.png"));
	addRenameAction();
	addAction("Remove", Common::Action::create(this, &TreeItemInput::contextRemove), "://icons/remove.png");
	addAction("Set Type", Common::Action::create(this, &TreeItemInput::contextSetType), "://icons/variable.png");
}

bool TreeItemInput::setName(const char *name) {
	bool result = logic->renameInput(input->getName(), name);
	if(result) {
		this->main_window->repaintViewport();
		this->main_window->onChangeLogic();
	}
	return result;
}

const char *TreeItemInput::getName() {
	return input->getName();
}

void TreeItemInput::onDoubleClicked() {
	if(this->main_window->getEditor())
		this->main_window->getEditor()->focusNode(logic->getInputNode(input));
	this->main_window->repaintViewport();
}

//------------------------------------------------------------------------------
//
// TreeItemInputs
//
//------------------------------------------------------------------------------

void TreeItemInputs::contextAdd() {
	using namespace Anandamide;
	int index = 0;
	while(logic->getInput(format("input_%d", index)) != NULL) index++;
	new_node = new Anandamide::InputNeurone(format("input_%d", index), logic);

	bool first = true;
	int pos_x = -240;
	int max_y = -160;
	for(int i=0; i<logic->getNodesCount(); i++) {

		Anandamide::Neurone *node = &logic->getNode(i);

		if(node == new_node) continue;

		if (dynamic_cast <Anandamide::InputNeurone *> (node) == NULL)
			if (dynamic_cast <Anandamide::ActionNeurone *> (node) == NULL) continue;

		int pos_y = node->getPosY() + node->getHeight() + 8;
		if(first) {
			max_y = pos_y;
			pos_x = node->getPosX();
			first = false;
		} else {
			if(max_y < pos_y) {
				max_y = pos_y;
				pos_x = node->getPosX();
			}
		}
	}
	new_node->setPos(pos_x, max_y);
	updateChildren();
	this->main_window->repaintViewport();
	this->main_window->onChangeLogic();
}

TreeItemInputs::TreeItemInputs(TreeItemBase *parent, Anandamide::Logic *logic) : TreeItemBase(parent) {
	this->logic = logic;
	setText("Inputs");
	setIcon(0, QIcon("://icons/input.png"));
	addAction("Add Input", Common::Action::create(this, &TreeItemInputs::contextAdd), "://icons/add.png");
	new_node = NULL;
	updateChildren();
}

void TreeItemInputs::updateChildren() {
	clearChildren();
	for(int i = 0; i<logic->getInputsCount(); i++) {
		TreeItemInput *item = new TreeItemInput(this, logic, &logic->getInput(i));
		if(new_node && new_node->getLogicInput() == &logic->getInput(i)) {
			this->setExpanded(true);
			item->rename();
		}
	}
	new_node = NULL;
}

//------------------------------------------------------------------------------
//
// TreeItemEvent
//
//------------------------------------------------------------------------------

void TreeItemEvent::contextRemove() {
	logic->removeEvent(event);
	if(this->main_window->getEditor())
		this->main_window->getEditor()->selectNode(NULL);
	this->main_window->repaintViewport();
	this->main_window->onChangeLogic();
	parentTreeItem()->updateChildren();
}

TreeItemEvent::TreeItemEvent(TreeItemBase *parent, Anandamide::Logic *logic, Anandamide::Event *event) : TreeItemBase(parent) {
	this->logic = logic;
	this->event = event;
	setText(event->getName());
	setIcon(0, QIcon("://icons/event.png"));
	addRenameAction();
	addAction("Remove", Common::Action::create(this, &TreeItemEvent::contextRemove), "://icons/remove.png");
}

bool TreeItemEvent::setName(const char *name) {
	bool result = logic->renameEvent(event->getName(), name);
	if(result) {
		this->main_window->repaintViewport();
		this->main_window->onChangeLogic();
	}
	return result;
}

const char *TreeItemEvent::getName() {
	return event->getName();
}

void TreeItemEvent::onDoubleClicked() {
	if(this->main_window->getEditor())
		this->main_window->getEditor()->focusNode(logic->getEventNode(event));
	this->main_window->repaintViewport();
}

//------------------------------------------------------------------------------
//
// TreeItemEvents
//
//------------------------------------------------------------------------------


void TreeItemEvents::contextAdd() {
	using namespace Anandamide;
	int index = 0;
	while(logic->getEvent(format("event_%d", index)) != NULL) index++;
	new_node = new Anandamide::EventNeurone(format("event_%d", index), logic);
	bool first = true;
	int pos_x = 80;
	int max_y = -160;
	for(int i=0; i<logic->getNodesCount(); i++) {
		Anandamide::Neurone *node = &logic->getNode(i);

		if(node == new_node) continue;

		if (dynamic_cast <Anandamide::OutputNeurone *> (node) == NULL)
			if (dynamic_cast <Anandamide::EventNeurone *> (node) == NULL) continue;

		int pos_y = node->getPosY() + node->getHeight() + 8;
		if(first) {
			max_y = pos_y;
			pos_x = node->getPosX();
			first = false;
		} else {
			if(max_y < pos_y) {
				max_y = pos_y;
				pos_x = node->getPosX();
			}
		}
	}
	new_node->setPos(pos_x, max_y);
	updateChildren();
	this->main_window->repaintViewport();
	this->main_window->onChangeLogic();
}

TreeItemEvents::TreeItemEvents(TreeItemBase *parent, Anandamide::Logic *logic) : TreeItemBase(parent) {
	this->logic = logic;
	setText("Events");
	setIcon(0, QIcon("://icons/event.png"));
	addAction("Add Event", Common::Action::create(this, &TreeItemEvents::contextAdd), "://icons/add.png");
	new_node = NULL;
	updateChildren();
}

void TreeItemEvents::onDoubleClicked() {

}

void TreeItemEvents::onLeftClicked() {

}

void TreeItemEvents::updateChildren() {
	clearChildren();
	for(int i = 0; i<logic->getEventsCount(); i++) {
		TreeItemEvent *item = new TreeItemEvent(this, logic, &logic->getEvent(i));
		if(new_node && new_node->getLogicEvent() == &logic->getEvent(i)) {
			this->setExpanded(true);
			item->rename();
		}
	}
	new_node = NULL;
}

//------------------------------------------------------------------------------
//
// TreeItemOutput
//
//------------------------------------------------------------------------------


void TreeItemOutput::contextRemove() {
	logic->removeOutput(output);
	if(this->main_window->getEditor())
		this->main_window->getEditor()->selectNode(NULL);
	this->main_window->repaintViewport();
	this->main_window->onChangeLogic();
	parentTreeItem()->updateChildren();
}

void TreeItemOutput::contextSetType() {
	using namespace Anandamide;
	TypeInfo type;
	if(selectTypeDialog(type)) {
		Anandamide::Neurone *output_node = logic->getOutputNode(output);
		Parameter *type_param = output_node->getParameter("type");
		if(type_param) {
			type_param->setValue(Variable(type));
			this->main_window->onChangeLogic();
		}
	}

	// hack for updating properties when corresponding input neurone currently selected
	if(this->main_window->getEditor())
		if(this->main_window->getEditor()->getOnSelectionChanged()) {
			this->main_window->getEditor()->getOnSelectionChanged()->run();
		}
}

TreeItemOutput::TreeItemOutput(TreeItemBase *parent, Anandamide::Logic *logic, Anandamide::Output *output) : TreeItemBase(parent) {
	this->logic = logic;
	this->output = output;
	setText(output->getName());
	setIcon(0, QIcon("://icons/input.png"));
	addRenameAction();
	addAction("Remove", Common::Action::create(this, &TreeItemOutput::contextRemove), "://icons/remove.png");
	addAction("Set Type", Common::Action::create(this, &TreeItemOutput::contextSetType), "://icons/variable.png");
}

bool TreeItemOutput::setName(const char *name) {
	bool result = logic->renameOutput(output->getName(), name);
	if(result) {
		this->main_window->repaintViewport();
		this->main_window->onChangeLogic();
	}
	return result;
}

const char *TreeItemOutput::getName() {
	return output->getName();
}

void TreeItemOutput::onDoubleClicked() {
	if(this->main_window->getEditor())
		this->main_window->getEditor()->focusNode(logic->getOutputNode(output));
	this->main_window->repaintViewport();
}

//------------------------------------------------------------------------------
//
// TreeItemOutputs
//
//------------------------------------------------------------------------------

void TreeItemOutputs::contextAdd() {
	using namespace Anandamide;
	int index = 0;
	while(logic->getOutput(format("output_%d", index)) != NULL) index++;
	new_node = new Anandamide::OutputNeurone(format("output_%d", index), logic);

	bool first = true;
	int pos_x = 80;
	int max_y = -160;
	for(int i=0; i<logic->getNodesCount(); i++) {

		Anandamide::Neurone *node = &logic->getNode(i);

		if(node == new_node) continue;

		if (dynamic_cast <Anandamide::OutputNeurone *> (node) == NULL)
			if (dynamic_cast <Anandamide::EventNeurone *> (node) == NULL) continue;

		int pos_y = node->getPosY() + node->getHeight() + 8;
		if(first) {
			max_y = pos_y;
			pos_x = node->getPosX();
			first = false;
		} else {
			if(max_y < pos_y) {
				max_y = pos_y;
				pos_x = node->getPosX();
			}
		}
	}
	new_node->setPos(pos_x, max_y);
	updateChildren();
	this->main_window->repaintViewport();
	this->main_window->onChangeLogic();
}

TreeItemOutputs::TreeItemOutputs(TreeItemBase *parent, Anandamide::Logic *logic) : TreeItemBase(parent) {
	this->logic = logic;
	setText("Outputs");
	setIcon(0, QIcon("://icons/input.png"));
	addAction("Add Output", Common::Action::create(this, &TreeItemOutputs::contextAdd), "://icons/add.png");
	new_node = NULL;
	updateChildren();
}

void TreeItemOutputs::updateChildren() {
	clearChildren();
	for(int i = 0; i<logic->getOutputsCount(); i++) {
		TreeItemOutput *item = new TreeItemOutput(this, logic, &logic->getOutput(i));
		if(new_node && new_node->getLogicOutput() == &logic->getOutput(i)) {
			this->setExpanded(true);
			item->rename();
		}
	}
	new_node = NULL;
}

//------------------------------------------------------------------------------
//
// TreeItemVariable
//
//------------------------------------------------------------------------------


void TreeItemVariable::contextRemove() {
	logic->removeVariable(variable);
	if(this->main_window->getEditor())
		this->main_window->getEditor()->selectNode(NULL);
	this->main_window->repaintViewport();
	this->main_window->onChangeLogic();
	parentTreeItem()->updateChildren();
}

void TreeItemVariable::contextSetType() {
	using namespace Anandamide;
	TypeInfo type;
	if(selectTypeDialog(type)) {
		logic->setVariableType(variable, type);
		this->main_window->onChangeLogic();
	}
	this->main_window->repaintViewport();
}

TreeItemVariable::TreeItemVariable(TreeItemBase *parent, Anandamide::Logic *logic, Anandamide::Variable *variable) : TreeItemBase(parent) {
	this->logic = logic;
	this->variable = variable;

	setText(getName());
	setIcon(0, QIcon("://icons/variable.png"));

	addRenameAction();
	addAction("Remove", Common::Action::create(this, &TreeItemVariable::contextRemove), "://icons/remove.png");
	addAction("Set Type", Common::Action::create(this, &TreeItemVariable::contextSetType), "://icons/variable.png");
	setDragDataType("GetVariable");
	setDragDataInstance(getName());
	setDraggable(true);

	get_item = new TreeItemBase(this, "Get");
	get_item->setDragDataType("GetVariable");
	get_item->setDragDataInstance(getName());
	get_item->setDraggable(true);
	get_item->setIcon(0, QIcon("://icons/puzzle_yellow.png"));
	
	set_item = new TreeItemBase(this, "Set");
	set_item->setDragDataType("SetVariable");
	set_item->setDragDataInstance(getName());
	set_item->setDraggable(true);
	set_item->setIcon(0, QIcon("://icons/puzzle_yellow.png"));

	get_item_ptr = new TreeItemBase(this, "Ptr");
	get_item_ptr->setDragDataType("GetVariablePtr");
	get_item_ptr->setDragDataInstance(getName());
	get_item_ptr->setDraggable(true);
	get_item_ptr->setIcon(0, QIcon("://icons/puzzle_yellow.png"));
}

bool TreeItemVariable::setName(const char *name) {
	bool result = logic->renameVariable(getName(), name);
	setDragDataInstance(getName());
	set_item->setDragDataInstance(getName());
	get_item->setDragDataInstance(getName());
	get_item_ptr->setDragDataInstance(getName());
	if(result) {
		this->main_window->repaintViewport();
		this->main_window->onChangeLogic();
	}
	return result;
}

const char *TreeItemVariable::getName() {
	return logic->getVariableName(variable);
}

void TreeItemVariable::onDoubleClicked() {
	//this->main_window->getEditor().focusNode(logic->getEventNode(event));
	//this->main_window->repaintViewport();
}

//------------------------------------------------------------------------------
//
// TreeItemVariables
//
//------------------------------------------------------------------------------

void TreeItemVariables::contextAdd() {
	using namespace Anandamide;
	int index = 0;
	while(logic->getVariable(format("var_%d", index)) != NULL) index++;
	new_node = logic->addVariable(format("var_%d", index));
	updateChildren();
	this->main_window->onChangeLogic();
}

TreeItemVariables::TreeItemVariables(TreeItemBase *parent, Anandamide::Logic *logic) : TreeItemBase(parent) {
	this->logic = logic;
	setText("Variables");
	setIcon(0, QIcon("://icons/variable.png"));
	addAction("Add Variable", Common::Action::create(this, &TreeItemVariables::contextAdd), "://icons/add.png");
	new_node = NULL;
	updateChildren();
}

void TreeItemVariables::updateChildren() {
	clearChildren();

	for(int i = 0; i<logic->getVariablesCount(); i++) {
		TreeItemVariable *item = new TreeItemVariable(this, logic, &logic->getVariable(i));
		if(new_node && new_node == &logic->getVariable(i)) {
			this->setExpanded(true);
			item->rename();
		}
	}
	new_node = NULL;
}

//------------------------------------------------------------------------------
//
// TreeItemLogic
//
//------------------------------------------------------------------------------


TreeItemScript *TreeItemLogic::getSctiptItem() const
{
	TreeItemScript* itemScript = NULL;
	QTreeWidgetItem* item = const_cast<TreeItemLogic*>(this);
	while(!itemScript) {
		itemScript = dynamic_cast<TreeItemScript*>(item->parent());
		if(!itemScript)
			item = item->parent();
	}
	return itemScript;
}

void TreeItemLogic::contextRemove() {
//	this->main_window->removeLogic(logic);
	getSctiptItem()->removeLogic(this);
}

void TreeItemLogic::contextClone() {
	int index = 0;
	Anandamide::Logic *clone = (Anandamide::Logic *)logic->clone();
	do {
		clone->setName(format("%s_%d", logic->getName(), index++));
	} while (this->main_window->getScript()->addLogic(clone) == false);
	clone->setKind(logic->getKind());
	this->main_window->onChangeLogic();
	getSctiptItem()->updateChildren();
	//	parentTreeItem()->updateChildren();
}

void TreeItemLogic::contextMove()
{
	this->main_window->moveProjectBlock(this);
}

TreeItemLogic::TreeItemLogic(TreeItemBase *parent, Anandamide::Logic *logic) : TreeItemBase(parent) {
	this->logic = logic;
	setText(logic->getName());
	setIcon(0, QIcon("://icons/puzzle_yellow.png"));
	setEditable(false);
	setDraggable(true);
	setDragDataType(logic->getName());
	bool isMainBlock = QString(logic->getName()) == "Main";

	if(!isMainBlock) {
		addRenameAction();
		//Леха будет недоволен)
		addAction("Remove", Common::Action::create(this, &TreeItemLogic::contextRemove), "://icons/remove.png");
	}
	addAction("Clone", Common::Action::create(this, &TreeItemLogic::contextClone), "://icons/clone.png");
	if(!isMainBlock)
		addAction("Move to group", Common::Action::create(this, &TreeItemLogic::contextMove), "://icons/folder.png");

	updateChildren();
}

void TreeItemLogic::setSelected(bool state) {
	setExpanded(state);
	setChildrenHidden(!state);
	if(state) {
		this->setTextColor(0, QColor("red"));
	} else {
		this->setTextColor(0, QColor("black"));
	}

}

void TreeItemLogic::select() {
	this->main_window->setActiveLogic(logic);
	getSctiptItem()->unselectAll(this);
	setSelected(true);
}

void TreeItemLogic::onDoubleClicked() {
	select();
	this->setExpanded(!this->isExpanded());
}

bool TreeItemLogic::setName(const char *name) {
	if(Str(name) == logic->getName()) return false;
	Anandamide::Script *script = logic->getScript();
	if(script->findLogic(name) != NULL) {
		return false;
	}
	script->renameNodes(logic->getName(), name);
	logic->setName(name);
	setDragDataType(logic->getName());
	this->main_window->repaintViewport();
	this->main_window->onChangeLogic();
	return true;
}

const char *TreeItemLogic::getName() {
	return logic->getName();
}

void TreeItemLogic::updateChildren() {
	clearChildren();
	new TreeItemActions(this, logic);
	new TreeItemEvents(this, logic);
	new TreeItemInputs(this, logic);
	new TreeItemOutputs(this, logic);
	new TreeItemVariables(this, logic);
	setSelected(this->main_window->getActiveLogic() == logic);
}

void TreeItemLogic::setKindGroup(TreeItemGroup *group)
{
	QString newKind;
	if(group)
		newKind = group->getKind();
	setKind(newKind);
}

void TreeItemLogic::setKind(const QString &kind)
{
	logic->setKind(Anandamide::Str(kind.toLocal8Bit().constData()));
	this->main_window->onChangeLogic();
	getSctiptItem()->updateChildren();
}

//------------------------------------------------------------------------------
//
// TreeItemScript
//
//------------------------------------------------------------------------------

void TreeItemScript::getSubLogics(TreeItemGroup *group)
{
	for(int i = 0; i < group->childCount(); ++i) {
		TreeItemGroup* subGroup = dynamic_cast<TreeItemGroup*>(group->child(i));
		if(subGroup)
			getSubLogics(subGroup);
		else {
			TreeItemLogic* subLogic = dynamic_cast<TreeItemLogic*>(group->child(i));
			if(subLogic) {
				subLogics.append(subLogic->logic);
			}
		}
	}
}

void TreeItemScript::removeGroup(TreeItemGroup *group)
{
	subLogics.clear();
	getSubLogics(group);
	if(subLogics.size()) {
		foreach (Anandamide::Logic* logic, subLogics) {
			this->main_window->removeLogic(logic);
		}
		subLogics.clear();
	}
	group->parent()->removeChild(group);
	this->main_window->onChangeLogic();
}

void TreeItemScript::removeLogic(TreeItemLogic *itemLogic)
{
	this->main_window->removeLogic(itemLogic->logic);
	itemLogic->parent()->removeChild(itemLogic);
	this->main_window->onChangeLogic();
}

void TreeItemScript::actionAddBlock() {
	int index = 0;
	do {
		new_logic = script->addLogic(format("Block_%d", index));
		index++;
	} while(new_logic == NULL);
	updateChildren();
	this->main_window->onChangeLogic();
}

void TreeItemScript::actionAddGroup()
{
	int index = 0;
	bool existsFlag;
	QString blockName;
	do {
		existsFlag = false;
		blockName = QString("Group_%1").arg(index++);
		for(int i = 0; i < this->childCount(); ++i) {
			TreeItemBase* child = dynamic_cast<TreeItemBase*>(this->child(i));
			if(child) {
				if(child->getText() == blockName) {
					existsFlag = true;
					break;
				}
			}
		}
	} while(existsFlag);
	TreeItemGroup* group = new TreeItemGroup(this, blockName, script);
	group->rename();
	this->main_window->onChangeLogic();
}

TreeItemScript::TreeItemScript(QTreeWidget *parent, MainWindow *main_window, Anandamide::Script *script) : TreeItemBase(parent, main_window) {
	this->script = script;
	setText("Unnamed Script");
	setIcon(0, QIcon("://icons/blocks.png"));
	addRenameAction();
	addAction("Add Block", Common::Action::create(this, &TreeItemScript::actionAddBlock), "://icons/add.png");
	addAction("Add Group", Common::Action::create(this, &TreeItemScript::actionAddGroup), "://icons/add_folder.png");
	new_logic = NULL;
	updateChildren();
}

void TreeItemScript::updateChildren() {
	if(Str(getName()) == "") {
		setText("Unnamed Script");
	} else {
		setText(getName());
	}
	clearChildren();
	for(int i = 0; i<script->getLogicsCount(); i++) {
		QString kind(script->getLogic(i)->getKind().str());
		if(kind == QString()) {
			TreeItemLogic *item = new TreeItemLogic(this, script->getLogic(i));
			if(new_logic && new_logic == script->getLogic(i)) {
				this->setExpanded(true);
				item->rename();
			}
		}
		else {
			QStringList kindList = kind.split('/');
			TreeItemBase* base = dynamic_cast<TreeItemBase*>(this);
			for(int j = 0; j < kindList.size(); ++j) {
				TreeItemBase* child = base->getChildByText(kindList.at(j));
				if(child) {
					base = child;
				}
				else {
					TreeItemGroup* group = new TreeItemGroup(base, kindList.at(j), script);
					base = group;
				}
			}
			TreeItemLogic *item = new TreeItemLogic(base, script->getLogic(i));
			if(new_logic && new_logic == script->getLogic(i)) {
				this->setExpanded(true);
				item->rename();
			}
		}
	}
	new_logic = NULL;
}

bool TreeItemScript::setName(const char *name) {
	script->setName(name);
	this->main_window->onChangeLogic();
	return true;
}

const char *TreeItemScript::getName() {
	return script->getName();
}

void TreeItemScript::unselectAll(TreeItemLogic* current_logic)
{
	for(int i = 0; i < this->childCount(); ++i) {
		TreeItemLogic *child = dynamic_cast <TreeItemLogic *>(this->child(i));
		if(child) {
			if(child == current_logic) continue;
			child->setSelected(false);
		}
		else {
			TreeItemGroup *group = dynamic_cast <TreeItemGroup *>(this->child(i));
			if(group)
				group->unselectAll(current_logic);
		}
	}
}

//------------------------------------------------------------------------------
//
// TreeItemGroup
//
//------------------------------------------------------------------------------

TreeItemScript *TreeItemGroup::getSctiptItem() const
{
	TreeItemScript* itemScript = NULL;
	QTreeWidgetItem* item = const_cast<TreeItemGroup*>(this);
	while(!itemScript) {
		itemScript = dynamic_cast<TreeItemScript*>(item->parent());
		if(!itemScript)
			item = item->parent();
	}
	return itemScript;
}

void TreeItemGroup::contextRemove()
{
	getSctiptItem()->removeGroup(this);
}

void TreeItemGroup::actionAddBlock()
{
	int index = 0;
	do {
		new_logic = script->addLogic(format("Block_%d", index));
		index++;
	} while(new_logic == NULL);
	updateChildren();
	this->main_window->onChangeLogic();
}

void TreeItemGroup::actionAddGroup()
{
	int index = 0;
	bool existsFlag;
	QString blockName;
	do {
		existsFlag = false;
		blockName = QString("Group_%1").arg(index++);
		for(int i = 0; i < this->childCount(); ++i) {
			TreeItemBase* child = dynamic_cast<TreeItemBase*>(this->child(i));
			if(child) {
				if(child->getText() == blockName) {
					existsFlag = true;
					break;
				}
			}
		}	
	} while(existsFlag);
	TreeItemGroup* group = new TreeItemGroup(this, blockName, script);
	group->rename();
	this->main_window->onChangeLogic();
}

TreeItemGroup::TreeItemGroup(TreeItemBase *parent, const QString &name, Script *script) : 
	TreeItemBase(parent, name.toLocal8Bit().constData())
{
	this->script = script;
	setText(name.toLocal8Bit().constData());
	setIcon(0, QIcon("://icons/folder.png"));
	setExpanded(true);
	addRenameAction();
	addAction("Remove", Common::Action::create(this, &TreeItemGroup::contextRemove), "://icons/remove.png");
	addAction("Add Block", Common::Action::create(this, &TreeItemGroup::actionAddBlock), "://icons/add.png");
	addAction("Add Group", Common::Action::create(this, &TreeItemGroup::actionAddGroup), "://icons/add_folder.png");
	new_logic = NULL;
	updateChildren();
}

void TreeItemGroup::updateChildren()
{
	if(new_logic) {
		TreeItemLogic *item = new TreeItemLogic(this, new_logic);
		this->setExpanded(true);
		item->rename();
		new_logic->setKind(Anandamide::Str(getKind().toLocal8Bit().constData()));
		new_logic = NULL;
	}
}

bool TreeItemGroup::setName(const char *name)
{
	if(this->text(0) == QString::fromLocal8Bit(name)) return false;
	this->setText(name);
	this->main_window->onChangeLogic();
	return true;
}

const char *TreeItemGroup::getName()
{
	baGetText = this->getText().toLocal8Bit();
	return baGetText.constData();
}

QString TreeItemGroup::getKind() const
{
	TreeItemGroup* parentGroup = const_cast<TreeItemGroup*>(this);
	QStringList kindList;
	while(parentGroup) {
		kindList.prepend(parentGroup->getText());
		parentGroup = dynamic_cast<TreeItemGroup*>(parentGroup->parent());
	}
	return kindList.join('/');
}

void TreeItemGroup::unselectAll(TreeItemLogic *current_logic)
{
	for(int i = 0; i < this->childCount(); ++i) {
		TreeItemLogic *child = dynamic_cast <TreeItemLogic *>(this->child(i));
		if(child) {
			if(child == current_logic) continue;
			child->setSelected(false);
		}
		else {
			TreeItemGroup *group = dynamic_cast <TreeItemGroup *>(this->child(i));
			if(group)
				group->unselectAll(current_logic);
		}
	}
}

//------------------------------------------------------------------------------
//
// TreeItemProperty
//
//------------------------------------------------------------------------------


TreeItemProperty::TreeItemProperty(QTreeWidget *parent, MainWindow *main_window, const char *name, Anandamide::Variable *variable) : TreeItemBase(parent, main_window) {
	m_window = this->main_window;
	this->variable = variable;
	setEditable(true);
	setText(name);
	if(variable->getType() == TYPEID(bool)) {
		setCheckState(1, Str(variable->getStr()) == "true" ? Qt::Checked : Qt::Unchecked);
	}
	setValueText(variable->getStr());
//	if(variable->getType() == TYPEID(bool)) {
//		setCheckState(1, Str(variable->getStr()) == "true" ? Qt::Checked : Qt::Unchecked);
//		setValueText(variable->getStr());
//	}
//	else if(variable->getType() == TYPEID(int)) {
//		if(QString(name) != "type")
//			setData(1, Qt::DisplayRole, QString::fromLocal8Bit(variable->getStr()).toInt());
//		else
//			setValueText(variable->getStr());
//	}
//	else if(variable->getType() == TYPEID(unsigned int)) {
//		setData(1, Qt::DisplayRole, QString::fromLocal8Bit(variable->getStr()).toUInt());
//	}
//	else if(variable->getType() == TYPEID(long long int)) {
//		setData(1, Qt::DisplayRole, QString::fromLocal8Bit(variable->getStr()).toLongLong());
//	}
//	else if(variable->getType() == TYPEID(unsigned long long int)) {
//		setData(1, Qt::DisplayRole, QString::fromLocal8Bit(variable->getStr()).toULongLong());
//	}
//	else if(variable->getType() == TYPEID(float)) {
//		setData(1, Qt::DisplayRole, QString::fromLocal8Bit(variable->getStr()).toFloat());
//	}
//	else if(variable->getType() == TYPEID(double)) {
//		setData(1, Qt::DisplayRole, QString::fromLocal8Bit(variable->getStr()).toDouble());
//	}
//	else {
//		setValueText(variable->getStr());
//	}
	setIcon(0, QIcon("://icons/input.png"));
	if(variable->getType() == TYPEID(TypeInfo)) {
		this->treeWidget()->openPersistentEditor(const_cast<TreeItemProperty*>(this), 1);
	}
}

TreeItemProperty::TreeItemProperty(TreeItemBase *parent, const char *name, Variable *variable, bool readOnly) : TreeItemBase(parent)
{
	m_window = this->main_window;
	this->variable = variable;
	setEditable(!readOnly);
	setText(name);
	if(variable->getType() == TYPEID(bool)) {
		setCheckState(1, Str(variable->getStr()) == "true" ? Qt::Checked : Qt::Unchecked);
	}
	setValueText(variable->getStr());
	
	setIcon(0, QIcon("://icons/input.png"));
	if(variable->getType() == TYPEID(TypeInfo))
		this->treeWidget()->openPersistentEditor(const_cast<TreeItemProperty*>(this), 1);
}

void TreeItemProperty::setEditorData(QWidget *editor) {
	if(!editor)
		return;
	if(variable->getType().getEnum() != NULL) {
		QComboBox *combo = (QComboBox *)editor;
		combo->setCurrentText(QString::fromLocal8Bit(variable->getStr()));
		return;
	}
	if(variable->getType() == TYPEID(TypeInfo)) {
		DelegateTypeEditor *delg = (DelegateTypeEditor*)editor;
		delg->setStringData(QString::fromLocal8Bit(variable->getStr()));
	}
	else if(variable->getType() == TYPEID(int) || 
			variable->getType() == TYPEID(unsigned int) || 
			variable->getType() == TYPEID(long long int) || 
			variable->getType() == TYPEID(unsigned long long int)) {
		QSpinBox* sb = (QSpinBox*)editor;
		sb->setValue(QString::fromLocal8Bit(variable->getStr()).toInt());
	}
	else if(variable->getType() == TYPEID(float) || variable->getType() == TYPEID(double)) {
		QDoubleSpinBox* dsb = (QDoubleSpinBox*)editor;
		dsb->setValue(QString::fromLocal8Bit(variable->getStr()).toDouble());
	}
	else {
		QLineEdit *line_edit = (QLineEdit *)editor;
		line_edit->setText(QString::fromLocal8Bit(variable->getStr()));
	}
}

void TreeItemProperty::getEditorData(QWidget *editor) {
	if(!editor)
		return;
	bool change = false;
	if(variable->getType().getEnum() != NULL) {
		QComboBox *combo = (QComboBox *)editor;

		variable->setFromStr(combo->currentText().toLocal8Bit().data());

		setValueText(variable->getStr());
		this->main_window->repaintViewport();
		change = true;
		return;
	}
	if(variable->getType() == TYPEID(TypeInfo)) {
		DelegateTypeEditor *delg = (DelegateTypeEditor*)editor;
		variable->setFromStr(delg->getStringData().toLocal8Bit().data());
		setValueText(variable->getStr());
		this->main_window->repaintViewport();
		change = true;
	}
	else if(variable->getType() == TYPEID(int) || 
			variable->getType() == TYPEID(unsigned int) || 
			variable->getType() == TYPEID(long long int) || 
			variable->getType() == TYPEID(unsigned long long int)) {
		QSpinBox* sb = (QSpinBox*)editor;
		variable->setFromStr(QString::number(sb->value()).toLocal8Bit().data());
		setValueText(variable->getStr());
		this->main_window->repaintViewport();
		change = true;
	}
	else if(variable->getType() == TYPEID(float) || variable->getType() == TYPEID(double)) {
		QDoubleSpinBox* dsb = (QDoubleSpinBox*)editor;
		variable->setFromStr(QString::number(dsb->value()).toLocal8Bit().data());
		setValueText(variable->getStr());
		this->main_window->repaintViewport();
		change = true;
	}
	else {
		QLineEdit *line_edit = (QLineEdit *)editor;
		variable->setFromStr(line_edit->text().toLocal8Bit().data());
		setValueText(variable->getStr());
		this->main_window->repaintViewport();
		change = true;
	}
	if(change)
		this->main_window->onChangeLogic();
}

QWidget *TreeItemProperty::createEditor(QWidget *parent) {
	if(variable->getType().getEnum() != NULL) {
		QComboBox *combo = new QComboBox(parent);
		const Enum *e = variable->getType().getEnum();
		for(int i=0; i<e->getItemsCount(); i++) {
			combo->addItem(QString::fromLocal8Bit(e->getItemName(i)), QVariant(e->getItemId(i)));
		}
		combo->model()->sort(0);
		return combo;
	}
	if(variable->getType() == TYPEID(TypeInfo)) {
		DelegateTypeEditor* delg = new DelegateTypeEditor(const_cast<TreeItemProperty*>(this), this->variable, parent);
		QObject::connect(delg,
						 SIGNAL(changeVariablePressed(DelegateTypeEditor*)),
						 m_window,
						 SLOT(onDelegateChangeVariablePressed(DelegateTypeEditor*)));
		QObject::connect(delg,
						 SIGNAL(variableChanged(Anandamide::Variable*)),
						 m_window,
						 SLOT(onDelegateVariableChanged(Anandamide::Variable*)));
		return delg;
	}
	else if(variable->getType() == TYPEID(bool)) {
		return NULL;
	}
	else if(variable->getType() == TYPEID(int) || 
			variable->getType() == TYPEID(unsigned int) || 
			variable->getType() == TYPEID(long long int) || 
			variable->getType() == TYPEID(unsigned long long int)) {
		QSpinBox* sb = new QSpinBox(parent);
		sb->setMaximum(INT_MAX);
		sb->setMinimum((variable->getType() == TYPEID(int) || variable->getType() == TYPEID(long long int)) ? INT_MIN : 0);
		return sb;
	}
	else if(variable->getType() == TYPEID(float) || variable->getType() == TYPEID(double)) {
		QDoubleSpinBox* dsb = new QDoubleSpinBox(parent);
		dsb->setDecimals(8);
		dsb->setMaximum((variable->getType() == TYPEID(float)) ? (FLT_MAX) : DBL_MAX);
		dsb->setMinimum((variable->getType() == TYPEID(float)) ? (-FLT_MAX) : -DBL_MAX);
		return dsb;
	}
	else
		return new QLineEdit(parent);
}

void TreeItemProperty::toggleBoolValue()
{
	if(variable->getType() != TYPEID(bool))
		return;
	this->setCheckState(1, this->checkState(1) == Qt::Checked ? Qt::Unchecked : Qt::Checked);
	bool newState = this->checkState(1) == Qt::Checked;
	variable->setBool(newState);
	setValueText(variable->getStr());
	this->main_window->onChangeLogic();
}

//------------------------------------------------------------------------------
//
// TreeItemProperties
//
//------------------------------------------------------------------------------

void TreeItemProperties::onSelectionChanged() {
	if(this->main_window->getEditor() == NULL) return;
	Anandamide::Neurone *selected = this->main_window->getEditor()->getSelectedNode();
	setNeurone(selected);
}

TreeItemProperties::TreeItemProperties(QTreeWidget *parent, MainWindow *main_window) : TreeItemBase(parent, main_window) {
	neurone = NULL;
	setDisabled(true);
	setHidden(true);
	updateChildren();
	if(this->main_window->getEditor())
		this->main_window->getEditor()->setOnSelectionChanged(Common::Action::create(this, &TreeItemProperties::onSelectionChanged));
}

#define TREE_PARAMETERS_COLOR	QColor(131,188,248)
#define TREE_INPUTS_COLOR		QColor(122,249,181)
#define TREE_OUTPUTS_COLOR		QColor(255,199,125)

void TreeItemProperties::updateChildren() {
	clearChildren();
	if(neurone == NULL) return;
	Anandamide::Neurone* updNeurone = neurone;
	
	if(updNeurone->getParametersCount() > 0) {
		TreeItemBase* parametersItem = new TreeItemBase(treeWidget(), this->main_window);
		parametersItem->setText("Parameters");
		QFont f;
		f.setBold(true);
		parametersItem->setData(0, Qt::FontRole, f);
		parametersItem->setData(0, Qt::BackgroundRole, QBrush(TREE_PARAMETERS_COLOR));
		parametersItem->setData(1, Qt::BackgroundRole, QBrush(TREE_PARAMETERS_COLOR));
		for(int i=0; i<updNeurone->getParametersCount(); i++) {
			new TreeItemProperty(parametersItem, updNeurone->getParameterName(i), &updNeurone->getParameter(i).getVariable());		
		}
		parametersItem->setExpanded(true);
	}

	if(updNeurone->getInputsCount() > 0) {
		TreeItemBase* inputsItem = new TreeItemBase(treeWidget(), this->main_window);
		inputsItem->setText("Inputs");
		QFont f;
		f.setBold(true);
		inputsItem->setData(0, Qt::FontRole, f);
		inputsItem->setData(0, Qt::BackgroundRole, QBrush(TREE_INPUTS_COLOR));
		inputsItem->setData(1, Qt::BackgroundRole, QBrush(TREE_INPUTS_COLOR));
		for(int i=0; i<updNeurone->getInputsCount(); i++) {
			new TreeItemProperty(inputsItem, updNeurone->getInputName(i), &updNeurone->getInput(i).getVariable());
		}
		inputsItem->setExpanded(true);
	}
	
}

void TreeItemProperties::setNeurone(Anandamide::Neurone *neurone) {
	this->neurone = neurone;
	updateChildren();
}

Anandamide::Neurone *TreeItemProperties::getNeurone() {
	return neurone;
}

void TreeItemProperties::clearChildren() {
	QTreeWidgetItem *root = treeWidget()->invisibleRootItem();
	while(root->childCount() > 1) root->removeChild(root->child(1));
}
