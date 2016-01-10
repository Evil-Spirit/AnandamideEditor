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

#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QPushButton>
#include <QDebug>
#include <QStyledItemDelegate>
#include <QTreeWidget>
#include <QLineEdit>
#include <QToolButton>
#include <QFileInfo>
#include <QDir>
#include <QtGui>
#include <QLabel>
#include <QFileDialog>
#include <QTextEdit>
#include <QGroupBox>
#include <QUndoView>
#include <QToolButton>
#include <QKeyEvent>
#include <QUndoStack>
#include <QCompleter>
#include <QCheckBox>
#include <AnandamideLibrary.h>

#include <QFocusEvent>

#include "TreeItemBase.h"
#include "Str.h"
#include "XmlSettings.h"
#include "Timer.h"

#include "SetLogicKindDialog.h"
#include "AnandamideStdLib.h"
#include "Renderer.h"
#include "AnandamideViewport.h"
#include "AnandamideLogWidget.h"
#include "AnandamideLibraryWidget.h"
#include "ConsoleOutWidget.h"

//#include <QUndoView>
//#include <QUndoStack>

MainWindow *main_window = NULL;
LibraryDialog *library_dialog = NULL;
TypeDialog *type_dialog = NULL;
ErrorSearch *errors_search = nullptr;
SearchWidget *search = nullptr;

//------------------------------------------------------------------------------
//
// class MainLogicEditor
//
//------------------------------------------------------------------------------

void MainLogicEditor::onLinkCreateBlock(float x, float y) {
	if(library_dialog == NULL) {
		library_dialog = new LibraryDialog();
	}
	
	Anandamide::Input *input = NULL;
	if(link_input != NULL) input = &link_input->getInput(link_input_index);
	
	Anandamide::Output *output = NULL;
	if(link_output != NULL) output = &link_output->getOutput(link_output_index);
	
	library_dialog->search("", input, output);
	
	QPoint pos = QCursor::pos();
	QRect rect = library_dialog->rect();
	library_dialog->setGeometry(pos.x() - rect.width() / 2, pos.y() - rect.height() / 2, rect.width(), rect.height());
	
	if(library_dialog->exec()) {
		const Anandamide::NeuroneDef *def = library_dialog->getSelectedDef();
		if(def != NULL) {
			const char *lib_name = "";
			if(def->getLibrary() != NULL) lib_name = def->getLibrary()->getName();
			if(main_window->getEditor()) {
				Anandamide::Neurone *neurone = main_window->getEditor()->create(def->getName(), lib_name);
				neurone->setPos(0, 0);
				if(output != NULL) {
					for(int i=0; i<neurone->getInputsCount(); i++) {
						Anandamide::Input *neurone_input = &neurone->getInput(i);
						if(neurone_input->getValue().getType() != output->getValue().getType()) continue;
						neurone_input->setSource(output);
						Anandamide::vec3 link_pos = neurone->getInputPos(i);
						neurone->setPos(x - link_pos.x, y - link_pos.y);
						main_window->onChangeLogic();
						break;
					}
				} else
					if(input != NULL) {
						for(int i=0; i<neurone->getOutputsCount(); i++) {
							Anandamide::Output *neurone_output = &neurone->getOutput(i);
							if(neurone_output->getValue().getType() != input->getValue().getType()) continue;
							input->setSource(neurone_output);
							Anandamide::vec3 link_pos = neurone->getOutputPos(i);
							neurone->setPos(x - link_pos.x, y - link_pos.y);
							main_window->onChangeLogic();
							break;
						}
					} else {
						neurone->setPos(x, y);
					}
			}
		}
	}
	
	if(main_window->getEditor())
		main_window->getEditor()->undoLinks();
	main_window->onChangeLogic();
}

//------------------------------------------------------------------------------
//
// class DelegateTypeEditor
//
//------------------------------------------------------------------------------

DelegateTypeEditor::DelegateTypeEditor(QTreeWidgetItem *item, Anandamide::Variable *variable, QWidget *parent)
	: QWidget(parent)
{
	m_variable = variable;
	m_item = item;
	
	editor = new QLineEdit(this);
	btn = new QToolButton(this);
	btn->setIcon(QIcon(":/icons/variable.png"));
	QHBoxLayout* lay = new QHBoxLayout(this);
	lay->setSpacing(0);
	lay->setMargin(0);
	lay->addWidget(editor);
	lay->addWidget(btn);
	this->setLayout(lay);
	
	connect(btn, SIGNAL(clicked()), SLOT(onBtnPressed()));
	connect(editor, SIGNAL(returnPressed()), SLOT(onReturnPressed()));
}

void DelegateTypeEditor::setStringData(const QString &data)
{
	editor->setText(data);
}

const QString DelegateTypeEditor::getStringData() const
{
	return editor->text();
}

void DelegateTypeEditor::updateStringData(const QString &data)
{
	editor->setText(data);
	m_variable->setFromStr(data.toLocal8Bit().data());
	m_item->setData(1, Qt::DisplayRole, editor->text());
	emit variableChanged(m_variable);
}

void DelegateTypeEditor::onBtnPressed()
{
	emit changeVariablePressed(const_cast<DelegateTypeEditor*>(this));
}

void DelegateTypeEditor::onReturnPressed()
{
	m_variable->setFromStr(editor->text().toLocal8Bit().data());
	m_item->setData(1, Qt::DisplayRole, editor->text());
	emit variableChanged(m_variable);
}

//------------------------------------------------------------------------------
//
// Messager
//
//------------------------------------------------------------------------------

class TextEditMessager : public Anandamide::Messager {
	
	QTextEdit *text_edit;
	
public:
	
	TextEditMessager(QTextEdit *text_edit) {
		this->text_edit = text_edit;	
	}
	
	void setTextEdit(QTextEdit *text_edit) {
		this->text_edit = text_edit;
	}
	
	void message(const char *text) {
		if(text_edit == NULL) return;
		QString str = QString::fromLocal8Bit(text);
		str = "<font color=\"#000000\">" + str + "</font>";
		text_edit->append(str);
	}
	
	void warning(const char *text){
		if(text_edit == NULL) return;
		QString str = QString::fromLocal8Bit(text);
		str = "<font color=\"#909020\">" + str + "</font>";
		text_edit->append(str);
		if(text_edit->parentWidget())
			if(text_edit->parentWidget()->parentWidget())
				text_edit->parentWidget()->parentWidget()->setVisible(true);
	}
	
	void error(const char *text) {
		if(text_edit == NULL) return;
		QString str = QString::fromLocal8Bit(text);
		str = "<font color=\"#902020\">" + str + "</font>";
		text_edit->append(str);
		if(text_edit->parentWidget())
			if(text_edit->parentWidget()->parentWidget())
				text_edit->parentWidget()->parentWidget()->setVisible(true);
	}
};

//------------------------------------------------------------------------------
//
// MainWindow
//
//------------------------------------------------------------------------------


void messageFunction(const char *message)
{
	main_window->message(message);
}

void messageExtFunction(const char *message, bool endl, Qt::GlobalColor color, bool bold, bool italic)
{
	main_window->messageExt(message, endl, color, bold, italic);
}

void setDockSize(QDockWidget* dock, int width, int height) {
	//qApp->processEvents();
	//dock->resize(width, height);
	QSize old_max_size = dock->maximumSize();
	QSize old_min_size = dock->minimumSize();
	
	if(width >= 0) {
		if(dock->width() < width) {
			dock->setMinimumWidth(width);
		} else {
			dock->setMaximumWidth(width);
		}
	}
	if (height >= 0) {
		if (dock->height() < height) {
			dock->setMinimumHeight(height);
		} else {
			dock->setMaximumHeight(height);
		}
	}
	
	//qApp->processEvents();
	
	//dock->setMinimumSize(old_min_size);
	//	dock->setMaximumSize(old_max_size);
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
	
	Anandamide::Renderer::initStaticImages();
	main_window = this;
	script = NULL;
	currentCollection = NULL;
	
	debugStepIntoMode = false;
	setMsgFunction(&messageFunction);
	setMsgExtFunction(&messageExtFunction);
	editableFlag = true;
	
	debugCameraAnimation = NULL;
	
	ui->setupUi(this);
	
	//Undo framework
	this->tabifyDockWidget(ui->LogDock, ui->dockWidgetConsole);
	
	// messager
	messager = new TextEditMessager(NULL);
	Anandamide::setMessager(messager);
	
	//setDockSize(ui->LogDock, -1, 50);
	ui->LogDock->setVisible(false);
	
#ifndef ANANDAMIDE_EDITOR_LIBRARY
	on_actionNew_triggered();
#endif
	
	QList<QDockWidget*> dockList = this->findChildren<QDockWidget*>();
	foreach (QDockWidget* dock, dockList) {
		ui->menuView->addAction(dock->toggleViewAction());
	}
	
	defaultSettings = saveEditorSettings();
	readSettings();
	
	connect(ui->tabWidgetViewports->tabBar(),
			SIGNAL(tabMoved(int,int)),
			SLOT(onTabMoved(int,int)));
	
	ui->workspace_tree->clearChanges();
	scriptEngine = new Anandamide::AnandamideEngine(this);
	connect(scriptEngine, SIGNAL(started()), SLOT(onScriptStarted()));
	
	
	//Search
	errors_search = new ErrorSearch(0);
	search = new SearchWidget(0);
	ui->errorsDockWidget->setWidget(errors_search);
	ui->searchDockWidget->setWidget(search);
	
	searchLine = new QLineEdit(0);
	QPushButton *searchBtn = new QPushButton("Search");
	
	QToolButton *backBtn = new QToolButton(0);
	QToolButton *fwdBtn = new QToolButton(0);
	
	searchLine->setFixedSize(200, 23);
	searchBtn->setFixedSize(60, 25);
	
	backBtn->setFixedSize(23, 23);
	fwdBtn->setFixedSize(23, 23);
	
	backBtn->setIcon(QIcon(":/icons/back.ico"));
	fwdBtn->setIcon(QIcon(":/icons/fwd.ico"));
	
	getToolBar()->insertWidget(0, searchLine);
	getToolBar()->insertWidget(0, backBtn);
	getToolBar()->insertWidget(0, fwdBtn);
	getToolBar()->insertWidget(0, searchBtn);
	
	//connect(searchLine, SIGNAL(textChanged(QString)), ((SearchWidget*)search), SLOT(setQuery(QString)));
    connect(searchBtn, SIGNAL(clicked()), this, SLOT(onSearchBtnClicked()));
	connect(searchBtn, SIGNAL(clicked()), ui->searchDockWidget, SLOT(show()));
	
	
    connect(backBtn, SIGNAL(clicked()), this, SLOT(onSearchGoBackBthClicked()));
    connect(fwdBtn, SIGNAL(clicked()), this, SLOT(onSearchGoFwdBtnClicked()));
}

MainWindow::~MainWindow() {
	writeSettings();
	delete ui;
	Anandamide::TypeDef::clearDefImages();
	delete messager;
//	delete andDebugger;
	Anandamide::Renderer::doneStaticImages();
}

void MainWindow::setActiveLogic(Anandamide::Logic *logic) {
	if(currentCollection == NULL) return;
	if(!currentCollection->isCreated()) return;
	
	for(int i=0; i<logic->getNodesCount(); i++) {
		logic->getNode(i).invalidate();
	}
	currentCollection->editor->setLogic(logic);
	repaintViewport();
//	currentCollection->viewport->repaint();
}

Anandamide::Logic *MainWindow::getActiveLogic() {
	if(currentCollection == NULL) return NULL;
	if(!currentCollection->isCreated()) return NULL;
	
	return currentCollection->editor->getLogic();
}

void MainWindow::updateProjectTree() {
	if(script == NULL) return;
	if(currentCollection == NULL) return;
	if(!currentCollection->isCreated()) return;
	currentCollection->project_tree->clear();
	TreeItemScript *script_item = new TreeItemScript(currentCollection->project_tree, this, script);
//	currentCollection->project_tree->addTopLevelItem(script_item);
	script_item->setExpanded(true);
}

void MainWindow::focusInEvent(QFocusEvent *ev)
{
	qDebug() << "In focus";
}

void MainWindow::updateTitle() {
	QString file = getFileName();
	if(file == "") file = "untitled";
	this->setWindowTitle("Anandamide Editor [" + file + "]");
}

bool MainWindow::openFile(const QString &name, bool newAnyway) {
	if(!QFileInfo(name).exists())
		return false;
	if(!newAnyway) {
		if(ui->workspace_tree->setScriptProject(name))
			return true;
	}
	
	EditorWidgetsCollection* back = currentCollection;
	currentCollection = new EditorWidgetsCollection(this);
	currentCollection->createCollection(this);
	currentCollection->viewport->shiftFlag = ui->actionShiftOnWheel->isChecked();
	TextEditMessager* mss = dynamic_cast<TextEditMessager*>(messager);
	if(mss)
		mss->setTextEdit(currentCollection->logWidget->getLogTextEdit());
	
	this->setCursorShape(Qt::WaitCursor);
	Anandamide::Script* scr = createScript();
	scr->load(name.toLocal8Bit().constData());
	this->setCursorShape(Qt::ArrowCursor);
	
	if(scr->getLogicsCount() <= 0) {
		qDebug() << "MainWindow::openFile(no logics) = " << name;
		Anandamide::errorMessage(Anandamide::format("MainWindow::openFile(no logics) = ", name.toLocal8Bit().constData()));
		scr->destroy();
		scr = NULL;
		delete currentCollection;
		currentCollection = back;
		if(mss && currentCollection && currentCollection->logWidget)
			mss->setTextEdit(currentCollection->logWidget->getLogTextEdit());
		
		script = NULL;
		return false;
	}
	if(!ui->workspace_tree->addScriptProject(scr, name, ui->tabWidgetViewports->count(), currentCollection)) {
		qDebug() << "MainWindow::openFile(cant open) = " << name;
		Anandamide::errorMessage(Anandamide::format("MainWindow::openFile(cant open) = ", name.toLocal8Bit().constData()));
		scr->destroy();
		scr = NULL;
		delete currentCollection;
		currentCollection = back;
		if(mss && currentCollection && currentCollection->logWidget)
			mss->setTextEdit(currentCollection->logWidget->getLogTextEdit());
		
		script = NULL;
		return false;
	}
	script = scr;
//	createTestLibraryForScript(script);
	
	QString scriptName = QString::fromLocal8Bit( script->getName() );
	if(scriptName == QString())
		scriptName = "Unnamed Script";
	int tabIdx = ui->tabWidgetViewports->addTab(currentCollection->viewport, QIcon(":/icons/blocks.png"), scriptName);
	ui->tabWidgetViewports->setCurrentIndex(tabIdx);
	
	Anandamide::Logic *logic = script->getLogic(0);
	currentCollection->editor->setLogic(logic);
	currentCollection->editor->reset();
	updateProjectTree();
	currentCollection->library_widget->invalidate();
	repaintViewport();
	
	updateTitle();
	
	loadProjectEditorSettings(name);
	
	setActionsEnabled(true);
	setCurrentLogic("Main");
	
	messager->message("Welcome");
	messager->error("To The");
	messager->warning("Anandamide");	
	
	errors_search->search();
	search->search();
	
	return true;
}

bool MainWindow::openScript(Anandamide::Script *scr, const QString &name) {
	EditorWidgetsCollection* back = currentCollection;
	currentCollection = new EditorWidgetsCollection(this);
	currentCollection->createCollection(this);
	currentCollection->viewport->shiftFlag = ui->actionShiftOnWheel->isChecked();
	TextEditMessager* mss = dynamic_cast<TextEditMessager*>(messager);
	if(mss)
		mss->setTextEdit(currentCollection->logWidget->getLogTextEdit());
	
	this->setCursorShape(Qt::WaitCursor);
	//scr->load(name.toLocal8Bit().constData());
	this->setCursorShape(Qt::ArrowCursor);
	
	if(!ui->workspace_tree->addScriptProject(scr, name, ui->tabWidgetViewports->count(), currentCollection)) {
//		qDebug() << "MainWindow::openFile(cant open) = " << name;
//		Anandamide::errorMessage(Anandamide::format("MainWindow::openFile(cant open) = ", name.toLocal8Bit().constData()));
//		scr->destroy();
//		scr = NULL;
		delete currentCollection;
		currentCollection = back;
		if(mss && currentCollection && currentCollection->logWidget)
			mss->setTextEdit(currentCollection->logWidget->getLogTextEdit());
		//script = NULL;
		return false;
	}
	script = scr;
//	createTestLibraryForScript(script);
	
	QString scriptName = QString::fromLocal8Bit( script->getName() );
	if(scriptName == QString())
		scriptName = "Unnamed Script";
	int tabIdx = ui->tabWidgetViewports->addTab(currentCollection->viewport, QIcon(":/icons/blocks.png"), scriptName);
	ui->tabWidgetViewports->setCurrentIndex(tabIdx);
	
	if(script->getLogicsCount() > 0) {
		Anandamide::Logic *logic = script->getLogic(0);
		currentCollection->editor->setLogic(logic);
	}
	currentCollection->editor->reset();
	updateProjectTree();
	currentCollection->library_widget->invalidate();
	repaintViewport();
	
	updateTitle();
	
	loadProjectEditorSettings(name);
	
	setActionsEnabled(true);
	setCurrentLogic("Main");
	
	messager->message("Welcome");
	messager->error("To The");
	messager->warning("Anandamide");	
	
	errors_search->search();
	search->search();
		
	return true;
}

bool MainWindow::openFile_Libraries(const QString &name, bool newAnyway)
{
	if(!newAnyway) {
		if(ui->workspace_tree->setScriptProject(name))
			return true;
	}
	
	EditorWidgetsCollection* back = currentCollection;
	currentCollection = new EditorWidgetsCollection(this);
	currentCollection->createCollection(this);
	currentCollection->viewport->shiftFlag = ui->actionShiftOnWheel->isChecked();
	TextEditMessager* mss = dynamic_cast<TextEditMessager*>(messager);
	if(mss)
		mss->setTextEdit(currentCollection->logWidget->getLogTextEdit());
	
	this->setCursorShape(Qt::WaitCursor);
	Anandamide::Script* scr = createScript();
	scr->loadLibraries(name.toLocal8Bit().constData());
	this->setCursorShape(Qt::ArrowCursor);
	
	if(!ui->workspace_tree->addScriptProject(scr, name, ui->tabWidgetViewports->count(), currentCollection)) {
		qDebug() << "MainWindow::openFile(cant open) = " << name;
		Anandamide::errorMessage(Anandamide::format("MainWindow::openFile(cant open) = ", name.toLocal8Bit().constData()));
		scr->destroy();
		delete currentCollection;
		currentCollection = back;
		if(mss)
			mss->setTextEdit(currentCollection->logWidget->getLogTextEdit());
		return false;
	}
	
	script = scr;
	
	QString scriptName = QString::fromLocal8Bit( script->getName() );
	if(scriptName == QString())
		scriptName = "Unnamed Script";
	int tabIdx = ui->tabWidgetViewports->addTab(currentCollection->viewport, QIcon(":/icons/blocks.png"), scriptName);
	ui->tabWidgetViewports->setCurrentIndex(tabIdx);
		
	return true;
}

bool MainWindow::openFile_Logics(const QString &name)
{
	script->loadLogics(name.toLocal8Bit().constData());
	
	QString scriptName = QString::fromLocal8Bit( script->getName() );
	if(scriptName == QString())
		scriptName = "Unnamed Script";
	
	Anandamide::Logic *logic = script->getLogic(0);
	currentCollection->editor->setLogic(logic);
	currentCollection->editor->reset();
	updateProjectTree();
	currentCollection->library_widget->invalidate();
	repaintViewport();
	
	updateTitle();
	
	loadProjectEditorSettings(name);
	
	setActionsEnabled(true);
	setCurrentLogic("Main");
	
	messager->message("Welcome");
	messager->error("To The");
	messager->warning("Anandamide");	
	return true;
}

bool MainWindow::setCurrentLogic(const QString &name)
{
	if(currentCollection == NULL) return false;
	if(!currentCollection->isCreated()) return false;
	
	if(script == NULL) return false;
	Anandamide::Logic *logic = script->findLogic(name.toLocal8Bit().constData());
	if(logic == NULL)
		return false;
	
	if(currentCollection->editor->getLogic() == logic) return true;
	currentCollection->editor->setLogic(logic);
	currentCollection->editor->reset();
	updateProjectTree();
	repaintViewport();
	return true;
}

void MainWindow::closeScript()
{
	if(script == NULL) return;
	ui->workspace_tree->removeScriptProject(script);
	
	errors_search->clear();
	search->clear();
	
}

void MainWindow::removeScriptProject(Anandamide::Script *script)
{
	ui->workspace_tree->removeScriptProject(script);
}

void MainWindow::closeScript(Anandamide::Script *script)
{
	if(script == NULL) return;
	removeScriptProject(script);
	errors_search->clear();
	search->clear();
}

void MainWindow::updateInWorkspace()
{
	if(script == NULL) return;
	ui->workspace_tree->updateProject(script);
}

QToolBar *MainWindow::getToolBar()
{
	return ui->toolBar;
}

QDockWidget *MainWindow::getProjectDockWidget()
{
	return ui->dockWidget;
}

QDockWidget *MainWindow::getPropertiesDockWidget()
{
	return ui->dockWidget_3;
}

QDockWidget *MainWindow::getLibraryDockWidget()
{
	return ui->dockWidget_2;
}

QDockWidget *MainWindow::getLogDockWidget()
{
	return ui->LogDock;
}

QDockWidget *MainWindow::getConsoleDockWidget()
{
	return ui->dockWidgetConsole;
}


QDockWidget *MainWindow::getErrorsSearchDockWidget()
{
	return ui->errorsDockWidget;
}

QTabWidget *MainWindow::getTabViewports()
{
	return ui->tabWidgetViewports;
}

QAction *MainWindow::getRunAction()
{
	return ui->actionRun;
}

void MainWindow::updateLibrariesTree()
{
	if(currentCollection == NULL) return;
	if(!currentCollection->isCreated()) return;
	currentCollection->library_widget->invalidate();
}

//void MainWindow::setRuntimeWindow(RuntimeWindow *rw)
//{
//	runtime = rw;
//}

bool MainWindow::setFileName(const QString &fileName)
{
	return ui->workspace_tree->setFileName(fileName, ui->tabWidgetViewports->currentIndex());
}

QString MainWindow::getFileName()
{
	return ui->workspace_tree->getFileName(ui->tabWidgetViewports->currentIndex());
}

void MainWindow::disconnectRunAction()
{
	disconnect(ui->actionRun, SIGNAL(triggered()), this, SLOT(on_actionRun_triggered()));
}

void MainWindow::runScript()
{
	if(script == NULL) return;
	try
	{
		script->compile();
		qDebug("Script compiled.");
		qDebug("Script start.");
		//Timer timer;
		//timer.start();
		script->run();
		//double time = timer.elapsed() * 1000.0;
		//qDebug()<<"Script end ("<<QString::number(time)<<")";
		this->script = script;
	}
	catch (const char *mes)
	{
		qDebug()<<mes;
	}
}

void MainWindow::moveProjectBlock(TreeItemLogic *itemLogic)
{
	SetLogicKindDialog* dlg = new SetLogicKindDialog(itemLogic->getSctiptItem(), this);
	if(dlg->exec() == QDialog::Accepted) {
		itemLogic->setKind(dlg->getKind());
	}
	delete dlg;
}

void MainWindow::saveAsFile(const QString &name)
{
	if(script == NULL) return;
	Anandamide::Str oldName(script->getName());
	script->save(name.toLocal8Bit().constData());
	saveProjectEditorSettings(name);
	setFileName(name);
	if(oldName != Anandamide::Str(script->getName())) {
		updateProjectTree();
		setTabText(QString::fromLocal8Bit(script->getName()));
	}
	updateTitle();
}

void MainWindow::saveFile() {
	QString fn = getFileName();
	if(fn == "")	{
		on_actionSaveAsProject_triggered();
	} else {
		saveAsFile(fn);
	}
}

void MainWindow::newFile() {
	currentCollection = new EditorWidgetsCollection(this);
	currentCollection->createCollection(this);
	currentCollection->viewport->shiftFlag = ui->actionShiftOnWheel->isChecked();
	
	script = createScript();
	script->newScript();
	
	ui->workspace_tree->addScriptProject(script, "", ui->tabWidgetViewports->count(), currentCollection);
	
	int tabIdx = ui->tabWidgetViewports->addTab(currentCollection->viewport, QIcon(":/icons/blocks.png"), "Unnamed Script");
	
	Anandamide::Logic *logic = script->getLogic(0);
	currentCollection->editor->setLogic(logic);
	currentCollection->editor->reset();
	updateProjectTree();
	currentCollection->library_widget->invalidate();
	repaintViewport();
	updateTitle();

	setActionsEnabled(true);
	
	setCurrentLogic("Main");
	ui->tabWidgetViewports->setCurrentIndex(tabIdx);
	
	messager->message("Welcome");
	messager->error("To The");
	messager->warning("Anandamide");
}

void MainWindow::loadProjectEditorSettings(const QString &project_name)
{
	if(currentCollection == NULL) return;
	if(!currentCollection->isCreated()) return;
	
	QString setsName = QFileInfo(project_name).absoluteDir().absolutePath() + "/" + QFileInfo(project_name).completeBaseName() + ".andedit";
	if(!QFileInfo(setsName).exists())
		return;
	QFile file( setsName );
	if ( !file.open(QIODevice::ReadOnly) )
		return;
	CXml_Settings sets;
	sets.loadFromIO(&file);
	file.close();
	currentCollection->library_widget->loadSearchTemplates(&sets);
}


void MainWindow::saveProjectEditorSettings(const QString &project_name)
{
	if(currentCollection == NULL) return;
	if(!currentCollection->isCreated()) return;
	
	QString setsName = QFileInfo(project_name).absoluteDir().absolutePath() + "/" + QFileInfo(project_name).completeBaseName() + ".andedit";
	CXml_Settings sets;
	
	currentCollection->library_widget->saveSearchTemplates(&sets);
	
	QFile file(setsName);
	if ( !file.open(QIODevice::WriteOnly) )
		return;
	sets.saveToIO(&file);
	file.close();
}

bool MainWindow::readSettings(QIODevice *dev)
{
	CXml_Settings sets;
	if(!sets.loadFromIO(dev))
		return false;

	sets.beginGroup("MainWindow");
	this->restoreGeometry(sets.value("Geometry").toByteArray());
	this->restoreState(sets.value("State").toByteArray());
	ui->actionShiftOnWheel->setChecked(sets.value("shiftFlag").toBool());
	ui->workspace_tree->setShiftOnWheel(ui->actionShiftOnWheel->isChecked());
	if(sets.value("cameraAnimation").isValid())
		ui->actionCameraAnimation->setChecked(sets.value("cameraAnimation").toBool());
	sets.endGroup();
	return true;
}

bool MainWindow::writeSettings(QIODevice *dev) const
{
	CXml_Settings sets;
	sets.beginGroup("MainWindow");
	sets.setValue("Geometry", this->saveGeometry());
	sets.setValue("State", this->saveState());
	sets.setValue("shiftFlag", ui->actionShiftOnWheel->isChecked());
	sets.setValue("cameraAnimation", ui->actionCameraAnimation->isChecked());
	sets.endGroup();
	return sets.saveToIO(dev);
}

void MainWindow::readSettings()
{
	QFile file( "AnandamideEditor.settings" );
	if ( !file.open(QIODevice::ReadOnly) )
		return;
	readSettings(&file);
	file.close();
}

void MainWindow::writeSettings() const
{
	QFile file( "AnandamideEditor.settings" );
	if ( !file.open(QIODevice::WriteOnly) )
		return;
	writeSettings(&file);
	file.close();
}

bool MainWindow::restoreEditorSettings(const QByteArray &sets)
{
	QDataStream stream(sets);
	stream.device()->reset();
	return readSettings(stream.device());
}

const QByteArray MainWindow::saveEditorSettings() const
{
	QByteArray res;
	QDataStream stream(&res, QIODevice::WriteOnly);
	stream.device()->reset();
	writeSettings(stream.device());
	return res;
}

void MainWindow::connectToCollection(EditorWidgetsCollection *collection)
{
	disconnectFromCollection();
	if(collection == NULL) return;
	if(!collection->isCreated()) return;
	collection->project_tree->setVisible(true);
	ui->tabWidget->widget(1)->layout()->addWidget(collection->project_tree);
	ui->LogDock->setWidget(collection->logWidget);
	ui->dockWidget_2->setWidget(collection->library_widget);
	ui->dockWidget_3->setWidget(collection->properties_tree);
	if(!ui->actionStopScript->isEnabled())
		ui->dockWidgetConsole->setWidget(collection->consoleOutWidget);
	TextEditMessager* mss = dynamic_cast<TextEditMessager*>(messager);
	if(mss)
		mss->setTextEdit(collection->logWidget->getLogTextEdit());
}

void MainWindow::disconnectFromCollection()
{
	QLayoutItem *item;
	while(item = ui->tabWidget->widget(1)->layout()->takeAt(0)) {
		if(item->widget())
			item->widget()->setVisible(false);
	}
	ui->LogDock->setWidget(NULL);
	ui->dockWidget_2->setWidget(NULL);
	ui->dockWidget_3->setWidget(NULL);
	if(!ui->actionStopScript->isEnabled())
		ui->dockWidgetConsole->setWidget(NULL);
	TextEditMessager* mss = dynamic_cast<TextEditMessager*>(messager);
	if(mss)
		mss->setTextEdit(NULL);
}

void MainWindow::setActionsEnabled(bool enabled)
{
	ui->actionSaveProject->setEnabled(enabled);
	ui->actionShowScriptTypes->setEnabled(enabled);
	if(enabled) {
		if(!ui->actionStopScript->isEnabled()) {
			ui->actionRun->setEnabled(true);
		}
	}
	else {
		ui->actionRun->setEnabled(false);
	}
	if(enabled) {
		bool prg = ui->workspace_tree->isScriptProject(script);
		ui->actionSaveAsProject->setEnabled(prg);
		ui->actionCloseScript->setEnabled(prg);
	}
	else {
		ui->actionSaveAsProject->setEnabled(false);
		ui->actionCloseScript->setEnabled(false);
	}
}

void MainWindow::closeAllTabs(bool removeInWorkspace, int exludeIndex)
{
	emit allTabClosed();
	for(int i = 0; i < ui->tabWidgetViewports->count(); ++i) {
		if(i == exludeIndex) continue;
		if(exludeIndex > i)
			exludeIndex--;
		if(removeInWorkspace)
			ui->workspace_tree->removeScriptProjectByIndex(i);
		on_tabWidgetViewports_tabCloseRequested(i);
		i--;
	}

}

void MainWindow::removeLibrary(Anandamide::Library *lib)
{
	if(script == NULL) return;
	if(script) {
		script->removeLibraryNodes(lib);
		script->getLibraries()->unloadLibrary(lib->getName());
	}
	if(currentCollection == NULL) return;
	if(!currentCollection->isCreated()) return;
	currentCollection->editor->clearSelection();
}

void MainWindow::setTabIndex(int index)
{
	ui->tabWidgetViewports->setCurrentIndex(index);
}

void MainWindow::setTabText(const QString &text)
{
	ui->tabWidgetViewports->setTabText(ui->tabWidgetViewports->currentIndex(), text);
}

int MainWindow::getTabsCount()
{
	return ui->tabWidgetViewports->count();
}

int MainWindow::getCurrentTab()
{
	return ui->tabWidgetViewports->currentIndex();
}

bool MainWindow::getShiftFlag()
{
	return ui->actionShiftOnWheel->isChecked();
}

int MainWindow::addCollection(EditorWidgetsCollection *collection, const char *title, bool topProject)
{
	QString scriptName = QString::fromLocal8Bit( title );
	if(scriptName == QString())
		scriptName = "Unnamed Script";
	int tabIdx = ui->tabWidgetViewports->addTab(collection->viewport, QIcon(topProject ? ":/icons/blocks.png" : ":/icons/library.png"), scriptName);
	ui->tabWidgetViewports->setCurrentIndex(tabIdx);
	return tabIdx;
}

Anandamide::Script *MainWindow::createScript()
{
	Anandamide::Script* res = Anandamide::Script::create();
	return res;
}

void MainWindow::setMessangerView(EditorWidgetsCollection *collection)
{
	if(collection == NULL) return;
	if(!collection->isCreated()) return;
	TextEditMessager* mss = dynamic_cast<TextEditMessager*>(messager);
	if(mss)
		mss->setTextEdit(collection->logWidget->getLogTextEdit());
}

bool MainWindow::openLibrary(const QString &libraryName)
{
	return ui->workspace_tree->openLibrary(libraryName);
}

void MainWindow::setCursorShape(Qt::CursorShape shape)
{
	QCursor cursor = this->cursor();
	cursor.setShape(shape);
	this->setCursor(cursor);
	QApplication::processEvents();
}

void MainWindow::animationCameraPos(const Anandamide::vec3 &newPos)
{
	if(!ui->actionCameraAnimation->isChecked())
		return;
	
	if(currentCollection == NULL) return;
	if(!currentCollection->isCreated()) return;
	
	if(debugCameraAnimation != NULL) {
		debugCameraAnimation->stop();
		delete debugCameraAnimation;
		debugCameraAnimation = NULL;
	}
		
	debugCameraAnimation = new QVariantAnimation(this);
	debugCameraAnimation->setEasingCurve(QEasingCurve::OutQuad);
	connect(debugCameraAnimation, 
			SIGNAL(valueChanged(QVariant)),
			SLOT(onDebugCameraAnimation(QVariant)));
	
	Anandamide::vec3 cameraPos = currentCollection->editor->getCameraPos();
	debugCameraAnimation->setStartValue(QPointF(cameraPos.x, cameraPos.y));
	debugCameraAnimation->setEndValue(QPointF(newPos.x, newPos.y));
	debugCameraAnimation->start();
}

void MainWindow::message(const char *message)
{
	ConsoleOutWidget* console = dynamic_cast<ConsoleOutWidget*>(ui->dockWidgetConsole->widget());
	if(console == NULL) return;
	console->message(message);
//	if(currentCollection == NULL) return;
//	if(!currentCollection->isCreated()) return;
//	currentCollection->consoleOutWidget->message(message);
}

void MainWindow::messageExt(const char *message, bool endl, Qt::GlobalColor color, bool bold, bool italic)
{
	ConsoleOutWidget* console = dynamic_cast<ConsoleOutWidget*>(ui->dockWidgetConsole->widget());
	if(console == NULL) return;
	console->messageExt(message, endl, color, bold, italic);
//	if(currentCollection == NULL) return;
//	if(!currentCollection->isCreated()) return;
//	currentCollection->consoleOutWidget->messageExt(message, endl, color, bold, italic);
}

int MainWindow::setCurrentTabTextColor(const QColor &color)
{
	int idx = ui->tabWidgetViewports->currentIndex();
	if(idx >= 0) {
		ui->tabWidgetViewports->tabBar()->setTabTextColor(idx, color);
	}
	return idx;
}

void MainWindow::setCurrentScript(QStringList &logicLibList)
{
	if(logicLibList.isEmpty()) return;
	if(logicLibList.last() != QString()) return;
	ui->workspace_tree->setCurrentScript(logicLibList);
}

void MainWindow::removeLogic(Anandamide::Logic *logic) {
	if(script == NULL) return;
	if(currentCollection == NULL) return;
	if(!currentCollection->isCreated()) return;
	
	if(currentCollection->editor->getLogic() == logic) {
		currentCollection->editor->setLogic(NULL);
		repaintViewport();
	}
	script->removeLogic(logic);
}

Anandamide::Script *MainWindow::getScript() {
	return script;
}

Anandamide::LogicEditor *MainWindow::getEditor() {
	if(currentCollection == NULL) return NULL;
	if(!currentCollection->isCreated()) return NULL;
	
	return currentCollection->editor;
}

void MainWindow::repaintViewport() {
	if(currentCollection == NULL) return;
	if(!currentCollection->isCreated()) return;
//	currentCollection->viewport->update();
	currentCollection->viewport->repaint();
}

void MainWindow::showEvent(QShowEvent *) {
	
}

void MainWindow::keyPressEvent(QKeyEvent *event) {

	if(currentCollection == NULL) return;
	if(!currentCollection->isCreated()) return;
	
	if(event->key() == Qt::Key_Escape) {
		if(!editableFlag) return;
		if(currentCollection->editor->isAddToGroupMode()) {
			currentCollection->editor->addToGroupCancel();
			this->statusBar()->showMessage("Addition to group was cancelled", 3000);
		}
		searchLine->clearFocus();
		event->accept();
	}
	if(event->key() == Qt::Key_Delete) {
		if(!editableFlag) return;
		if(currentCollection->editor->removeSelected())
			onChangeLogic();
		event->accept();
	}
	if(event->key() == Qt::Key_D) {
		if(!editableFlag) return;
		if(currentCollection->editor->cloneSelected())
			onChangeLogic();
		event->accept();
	}
	if(event->key() == Qt::Key_C) {
		Anandamide::Str str;
		currentCollection->editor->copyAsXml(str);
		QApplication::clipboard()->setText(QString::fromLocal8Bit(str.str()));
//		qDebug() << "copy to clipboard:\n" << QString::fromLocal8Bit(str.str());
		event->accept();
	}
	if(event->key() == Qt::Key_V) {
		if(!editableFlag) return;
		Anandamide::Str str = QApplication::clipboard()->text().toLocal8Bit().constData();
//		qDebug() << "paste from clipboard:\n" << QString::fromLocal8Bit(str.str());
		if(currentCollection->editor->pasteAsXml(str, -currentCollection->editor->getCameraPosX(), -currentCollection->editor->getCameraPosY())) {
			updateProjectTree();
			onChangeLogic();
		}
		event->accept();
	}
	
	//F
	if(event->key() == Qt::Key_F) {
		searchLine->setFocus();
		searchLine->selectAll();
		event->accept();
	}
	
	//F3
	if(event->key() == 0x01000032) {
		search->goForward();
		event->accept();
	}
	
	//Enter
	if(event->key() == 0x01000004) {
		if (search->lastQuery() != searchLine->text()) {
			search->setQuery(searchLine->text());
			search->search();
			search->goForward();
		} else {
			search->goForward();
		}
		event->accept();
	}	
	
	//Up
	if(event->key() == 0x01000013) {
		search->goBack();
		event->accept();
	}
	//Down
	if(event->key() == 0x01000015) {
		search->goForward();
		event->accept();
	}
	repaintViewport();
}

void MainWindow::closeEvent(QCloseEvent *) {
//	runtime->close();
	if (script)
		script->destroy();
}

void MainWindow::onDelegateChangeVariablePressed(DelegateTypeEditor *delegate)
{
	TypeDialog* dialog = new TypeDialog();
	dialog->invalidate();
	if(dialog->exec() == QDialog::Accepted) {
		Anandamide::TypeInfo type = dialog->getType();
		delegate->updateStringData(QString::number(type.id()));
	}
	delete dialog;
}

void MainWindow::onDelegateVariableChanged(Anandamide::Variable *variable)
{
	repaintViewport();
}

//событие изменения логики сценария
void MainWindow::onChangeLogic()
{
	if(currentCollection == NULL) return;
	if(!currentCollection->isCreated()) return;
	
	currentCollection->editor->onChangeLogic();

	errors_search->search();
	search->search();
	emit changed();
}

//событие изменения месторасположения блоков
void MainWindow::onChangeLocation()
{
	if(currentCollection == NULL) return;
	if(!currentCollection->isCreated()) return;
	
	currentCollection->editor->onChangeLocation();
	emit changed();
}

void MainWindow::on_actionSaveProject_triggered() {
	saveFile();
}

void MainWindow::on_actionOpenProject_triggered() {
	try {
//		bool closedState = ui->project_tree->topLevelItemCount() == 0;
//		if(closedState)
//			this->newFile();
		QString selfilter = tr("Anandamide (*.and)");
		QString str = QFileDialog::getOpenFileName(0, "Open", "../examples/", selfilter);
		if(str.length() > 0) {
			openFile(str);
		}
//		else if(closedState)
//			this->closeScript();
	} catch(const char *message) {
		qDebug() << message;
		Anandamide::errorMessage(message);
	}
}

void MainWindow::on_actionNew_triggered() {
	newFile();
}

void MainWindow::on_actionSaveAsProject_triggered() {
	QString selfilter = tr("Anandamide (*.and)");
	QString str = QFileDialog::getSaveFileName(0, "Save", "../examples/", selfilter);
	if(str.length() > 0) {
		saveAsFile(str);
	}
}

bool selectTypeDialog(Anandamide::TypeInfo &type) {
	if(type_dialog == NULL) {
		type_dialog = new TypeDialog();
	}
	type_dialog->invalidate();
	if(type_dialog->exec()) {
		type = type_dialog->getType();
		return true;
	}
	return false;
}

void MainWindow::on_actionRestore_default_view_triggered()
{
	restoreEditorSettings(defaultSettings);
}

void MainWindow::on_actionShowScriptTypes_triggered()
{
	TypeDialog* dialog = new TypeDialog();
	dialog->invalidate();
	dialog->exec();
	delete dialog;
}

void MainWindow::on_actionShiftOnWheel_triggered(bool checked)
{
	ui->workspace_tree->setShiftOnWheel(checked);
}

void MainWindow::on_actionCloseScript_triggered()
{
    closeScript();
}

void MainWindow::on_tabWidgetViewports_tabCloseRequested(int index)
{
	if(index < 0 || index >= ui->tabWidgetViewports->count())
		return;
	ui->workspace_tree->resetTabIndex(index);
	ui->tabWidgetViewports->removeTab(index);
	tabClosed(index);
}

void MainWindow::onTabMoved(int from, int to)
{
	ui->workspace_tree->tabMoved(from, to);
	emit tabMoved(from, to);
}

void MainWindow::on_tabWidgetViewports_currentChanged(int index)
{
	script = ui->workspace_tree->getScript(index);
	currentCollection = ui->workspace_tree->getCollection(index);
	connectToCollection(currentCollection);
	ui->workspace_tree->selectByTabIndex(index);
	updateTitle();
	setActionsEnabled(index >= 0);
	updateProjectEditable();
	
	if (search && errors_search) {
		errors_search->search();
		search->search();
	}
}

void MainWindow::on_actionNew_Workspace_triggered()
{
    ui->workspace_tree->newWorkspace();
}

void MainWindow::on_actionOpen_Workspace_triggered()
{
	try {
		QString selfilter = tr("Anandamide Workspace (*.aws)");
		QString str = QFileDialog::getOpenFileName(0, "Open", "../data/scripts/", selfilter);
		if(str.length() > 0) {
			ui->workspace_tree->loadWorkspace(str);
		}
	} catch(const char *message) {
		qDebug() << message;
		Anandamide::errorMessage(message);
	}
}

void MainWindow::on_actionSave_Workspace_triggered()
{
	if(!ui->workspace_tree->saveWorkspace())
		on_actionSave_Workspace_As_triggered();
}

void MainWindow::on_actionSave_Workspace_As_triggered()
{
	QString selfilter = tr("Anandamide Workspace (*.aws)");
	QString str = QFileDialog::getSaveFileName(0, "Save", "../data/scripts/", selfilter);
	if(str.length() > 0) {
		ui->workspace_tree->saveWorkspace(str);
	}
}

void MainWindow::on_actionSave_All_triggered()
{

}

void MainWindow::onDebugCameraAnimation(const QVariant &value)
{
	if(currentCollection == NULL) return;
	if(!currentCollection->isCreated()) return;
	QPointF pt = value.toPointF();
	currentCollection->editor->setCameraPos(Anandamide::vec3(pt.x(), pt.y(), 0.0));
	repaintViewport();
}

//Запуск выполнения скрипта
void MainWindow::on_actionRun_triggered()
{
	if(script == NULL) return;
	runOnSelection();
	if(currentCollection != NULL) 
		if(currentCollection->isCreated()) {
			currentCollection->consoleOutWidget->onRunScript();
			setCurrentTabTextColor(QColor(Qt::red));
		}
//	runtime->run(script);
	emit runScriptPrepare();
	updateProjectEditable();
	debugPauseMode = false;
	debugForwardMode = false;
	bool res = scriptEngine->run(script);
	if(!res && currentCollection != NULL) 
		if(currentCollection->isCreated()) {
			setCurrentTabTextColor(QColor(Qt::black));
		}
}


//Останов выполнения/отладки скрипта
void MainWindow::on_actionStopScript_triggered()
{
	for(int i = 0; i < ui->tabWidgetViewports->count(); ++i) {
		ui->tabWidgetViewports->tabBar()->setTabTextColor(i, QColor(Qt::black));
	}
	repaintViewport();
	ui->workspace_tree->onStopExecuting();
	onScriptFinished();
	runOnSelection();
	updateProjectEditable();
	emit stopScriptPrepare();
}
void MainWindow::onScriptStarted()
{
	ui->actionRun->setEnabled(false);
	ui->actionStopScript->setEnabled(true);
	
}

void MainWindow::onScriptFinished()
{
	ui->actionRun->setEnabled(true);
	ui->actionStopScript->setEnabled(false);
	
}

void MainWindow::onSearchGoBackBthClicked()
{
    search->goBack();
}

void MainWindow::onSearchGoFwdBtnClicked()
{
    search->goForward();
}

void MainWindow::onSearchBtnClicked()
{
    if (search->lastQuery() != searchLine->text()) {
        search->setQuery(searchLine->text());
        search->search();
        search->goForward();
    } else {
        search->goForward();
    }
}

void MainWindow::openScriptView(Anandamide::Script *script)
{
	ui->workspace_tree->openScriptView(script);
}

void MainWindow::runOnSelection()
{
	if(currentCollection == NULL) return;
	if(!currentCollection->isCreated()) return;
	currentCollection->editor->runOnSelection();
}

void MainWindow::updateProjectEditable()
{
	bool editable = true;
	editableFlag = editable;
	if(currentCollection == NULL) return;
	if(!currentCollection->isCreated()) return;
	for(int i = 0; i < currentCollection->project_tree->topLevelItemCount(); ++i) {
		TreeItemBase* child = dynamic_cast<TreeItemBase*>(currentCollection->project_tree->topLevelItem(i));
		if(child == NULL) continue;
		child->setActionsEnabled(editable);
	}
	currentCollection->library_widget->setEditable(editable);
	currentCollection->viewport->setEditable(editable);
}

bool MainWindow::reloadCurrentProject()
{
	if(!ui->actionCloseScript->isEnabled() || !ui->actionSaveProject->isEnabled()) return false;
	QString project_fn = getFileName();
	if(project_fn == QString()) return false;
	on_actionSaveProject_triggered();
	on_actionCloseScript_triggered();
	openFile(project_fn, true);
	return true;
}

SearchBaseWidget::SearchBaseWidget(QWidget *parent) : QWidget(parent)
{
	layout = new QGridLayout(this);
	list = new QTreeWidget(0);
	setLayout(layout);
	list->setStyleSheet(\
				"QTreeWidget::item:hover { background: rgb(50,142,254); }"\
				"QTreeWidget::item:selected { background: rgb(100,170,254); }"\
				);
	
	connect(list, SIGNAL(itemClicked(QTreeWidgetItem*,int)), this, SLOT(onListClicked(QTreeWidgetItem*,int)));
}

Anandamide::Script *SearchBaseWidget::getLastScript()
{
	return lastScript;
}

QTreeWidget *SearchBaseWidget::getList()
{
	return list;
}

QGridLayout *SearchBaseWidget::getLayout()
{
	return layout;
}

Q_DECLARE_METATYPE(Anandamide::Neurone*)

void SearchBaseWidget::addItem(Anandamide::Neurone *neurone)
{
	QTreeWidgetItem *item = new QTreeWidgetItem;
	item->setData(0, Qt::BackgroundRole, QVariant::fromValue(neurone));
	setItemInfo(item, neurone);
	list->addTopLevelItem(item);
}

void SearchBaseWidget::inplaceClear()
{
	
}

void SearchBaseWidget::search()
{
	if (!main_window) return;
	if (!main_window->getScript()) return;
	lastScript = main_window->getScript();
	clear();
	bool hasOne = false;
	for (int i = 0; i < main_window->getScript()->getLogicsCount(); i++) {
		for (int j = 0; j < main_window->getScript()->getLogic(i)->getNodesCount(); j++) {
			if (algorithm(&main_window->getScript()->getLogic(i)->getNode(j))) {
				addItem(&main_window->getScript()->getLogic(i)->getNode(j));
				hasOne = true;
			}
		}
	}
//	if (hasOne) {
//		onListClicked(list->topLevelItem(0), 0);
//		list->setItemSelected(list->topLevelItem(0), true);
//	}
}

void SearchBaseWidget::clear()
{
	list->clear();
	inplaceClear();
}

void SearchBaseWidget::onListClicked(QTreeWidgetItem *item, int column)
{
	Anandamide::Neurone *neurone = item->data(0, Qt::BackgroundRole).value <Anandamide::Neurone*>();
	main_window->setCurrentLogic(neurone->getLogic()->getName());
	main_window->getEditor()->focusNode(neurone);
	main_window->getEditor()->selectNode(neurone);
	main_window->repaintViewport();
}

ErrorSearch::ErrorSearch(QWidget *parent) : SearchBaseWidget(parent)
{
	getLayout()->addWidget(getList(), 0, 0, 1, 1);
	
	getList()->setColumnCount(4);
	
	QStringList labels;
	labels << "Error" << "Type" << "Library" << "Logic";
	
	getList()->setHeaderLabels(labels);
	
	errors[1] = "Bad type or library";
	errors[2] = "Bad action/event/input/output (not exist or renamed)";
		
	getList()->setColumnWidth(0, 100);
	getList()->setColumnWidth(1, 50);
	getList()->setColumnWidth(2, 50);
	getList()->setColumnWidth(3, 50);
}

void ErrorSearch::setItemInfo(QTreeWidgetItem *item, Anandamide::Neurone *neurone)
{
	item->setText(0, QString("[%1]: %2")\
				  .arg(neurone->getError())\
				  .arg(errors.contains(neurone->getError()) ? \
					errors[neurone->getError()] : "Unknown error"));
	item->setText(1, neurone->getType());
	item->setText(2, neurone->getLibraryName());
	item->setText(3, neurone->getLogic()->getName());
	item->setIcon(0, QIcon(":/icons/error.png"));
}

bool ErrorSearch::algorithm(Anandamide::Neurone *neurone)
{
	if (neurone->getError() == 0) return false;
	return true;
}

SearchWidget::SearchWidget(QWidget *parent) : SearchBaseWidget(parent)
{
	QGroupBox *box = new QGroupBox(0);
	box->setLayout(new QHBoxLayout());
		
	QCheckBox *cb_completeEnabled = new QCheckBox("Complete words", 0);
	QCheckBox *cb_caseEnabled = new QCheckBox("Case sense", 0);
	QCheckBox *cb_regexEnabled = new QCheckBox("Regex enabled", 0);
	
	box->layout()->addWidget(cb_completeEnabled);
	box->layout()->addWidget(cb_caseEnabled);
	box->layout()->addWidget(cb_regexEnabled);

	getLayout()->addWidget(box, 0, 0, 1, 3);
	
	getLayout()->addWidget(getList(), 1, 0, 1, 3);
	getList()->setColumnCount(3);
	
	QStringList labels;
	labels << "Type" << "Library" << "Logic";
	
	getList()->setHeaderLabels(labels);
	
	setCompleteWordsEnabled(false);
	setCaseEnabled(false);
	setRegExpEnabled(false);
	
	cb_completeEnabled->setChecked(false);
	cb_caseEnabled->setChecked(false);
	cb_regexEnabled->setChecked(false);
	
	connect(cb_completeEnabled, SIGNAL(toggled(bool)), this, SLOT(setCompleteWordsEnabled(bool)));
	connect(cb_caseEnabled, SIGNAL(toggled(bool)), this, SLOT(setCaseEnabled(bool)));
	connect(cb_regexEnabled, SIGNAL(toggled(bool)), this, SLOT(setRegExpEnabled(bool)));
	
	getList()->setColumnWidth(0, 100);
	getList()->setColumnWidth(1, 50);
	getList()->setColumnWidth(2, 50);
}

void SearchWidget::setItemInfo(QTreeWidgetItem *item, Anandamide::Neurone *neurone)
{
	item->setText(0, neurone->getType());
	item->setText(1, neurone->getLibraryName());
	item->setText(2, neurone->getLogic()->getName());
	item->setIcon(0, QIcon(":/icons/search.png"));
}

bool SearchWidget::algorithm(Anandamide::Neurone *neurone)
{
	if (query.isEmpty()) return false;
	if (inAlgoritm(neurone->getType())) return true;
	if (inAlgoritm(neurone->getCaption())) return true;
	return false;
}

bool SearchWidget::inAlgoritm(QString str)
{
	if (regExpEnabled) {
		if (QRegExp(query).exactMatch(str)) return true;
		return false;
	} else {
		if (completeWordsEnabled) {
			if (query == str) return true;
			return false;
		} else {
			if (caseEnabled) {
				if (str.contains(query)) return true;
				return false;
			} else {
				if (str.toLower().contains(query.toLower())) return true;
				return false;
			}
		}
	}
}

QString &SearchWidget::lastQuery()
{
	return query;
}

void SearchWidget::setQuery(QString query)
{
	this->query = query;
}

void SearchWidget::setCompleteWordsEnabled(bool enabled)
{
	completeWordsEnabled = enabled;
}

void SearchWidget::setCaseEnabled(bool enabled)
{
	caseEnabled = enabled;
}

void SearchWidget::setRegExpEnabled(bool enabled)
{
	regExpEnabled = enabled;
}

void SearchWidget::goForward()
{
	if (main_window->getScript() != getLastScript()) return search();
	if (getList()->topLevelItemCount() == 0) return;
	int index = -1;
	if (getList()->selectedItems().count() > 0) {
		index = getList()->indexOfTopLevelItem(getList()->selectedItems().at(0));
	}
	getList()->clearSelection();
	if (getList()->topLevelItemCount() > (index + 1)) {
		getList()->setItemSelected(getList()->topLevelItem(index + 1), true);
		onListClicked(getList()->topLevelItem(index + 1), 0);
	} else {
		getList()->setItemSelected(getList()->topLevelItem(0), true);
		onListClicked(getList()->topLevelItem(0), 0);
	}
	main_window->repaintViewport();
}

void SearchWidget::goBack()
{
	if (getList()->topLevelItemCount() == 0) return;
	int index = getList()->topLevelItemCount();
	if (getList()->selectedItems().count() > 0) {
		index = getList()->indexOfTopLevelItem(getList()->selectedItems().at(0));
	}
	getList()->clearSelection();
	if ((index - 1) > -1) {
		getList()->setItemSelected(getList()->topLevelItem(index - 1), true);
		onListClicked(getList()->topLevelItem(index - 1), 0);
	} else {
		getList()->setItemSelected(getList()->topLevelItem(getList()->topLevelItemCount() - 1), true);
		onListClicked(getList()->topLevelItem(getList()->topLevelItemCount() - 1), 0);
	}
}

