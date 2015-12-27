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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

//#include "GlobeXY.h"

#include <QMainWindow>
#include <QAction>
#include <QVariantAnimation>

#include "AnandamideAPI.h"
#include "AnandamideEngine.h"
#include "RuntimeWindow.h"
#include "LibraryDialog.h"
#include "TypeDialog.h"

#include "treeitemclasses.h"

#include "EditorWidgetsCollection.h"

class QTreeWidgetItem;
class DelegateTypeEditor;
class CXml_Settings;
class AnandamideLibraryTree;
class AnandamideViewport;
class QUndoCommand;
class QCompleter;
class QGridLayout;
class RenameCommand;
class QToolButton;

#include "AnandamideEditorGlobal.h"

namespace Ui {
	class MainWindow;
	class AnandamideEngine;
}

namespace Anandamide {
	class Script;
	class vec3;
}

class ANANDAMIDE_EDITOR_API MainWindow : public QMainWindow
{
		Q_OBJECT
		
		friend class AnandamideViewport;
		friend class RenameCommand;
		
		Anandamide::Messager *messager;
		
	public:
		
		explicit MainWindow(QWidget *parent = 0);
		~MainWindow();
		void repaintViewport();
		
		void setActiveLogic(Anandamide::Logic *logic);
		Anandamide::Logic *getActiveLogic();
		void removeLogic(Anandamide::Logic *logic);
		
		Anandamide::Script *getScript();
		Anandamide::LogicEditor *getEditor();
		
		void newFile();
		bool openFile(const QString &name, bool newAnyway = true);
		bool openScript(Anandamide::Script *script, const QString &name);
		
		bool openFile_Libraries(const QString &name, bool newAnyway = true);
		bool openFile_Logics(const QString &name);
		void saveAsFile(const QString &name);
		void saveFile();
		
		bool setCurrentLogic(const QString &name);
		void closeScript();

		void removeScriptProject(Anandamide::Script *script);
		void closeScript(Anandamide::Script *script);
		
		void updateInWorkspace();
		
		// Для дизайнера форм
		QToolBar *getToolBar();
		QDockWidget *getProjectDockWidget();
		QDockWidget *getPropertiesDockWidget();
		QDockWidget *getLibraryDockWidget();
		QDockWidget *getLogDockWidget();
		QDockWidget *getConsoleDockWidget();
		QDockWidget *getErrorsSearchDockWidget();
		QDockWidget *getHelpDock();
		QDockWidget *getUndoDock();
		QTabWidget *getTabViewports();
		QAction *getRunAction();
		void updateLibrariesTree();
//		void setRuntimeWindow(RuntimeWindow *rw);
		bool setFileName(const QString &fileName);
		QString getFileName();
		void disconnectRunAction();
		void runScript();
		
		void moveProjectBlock(TreeItemLogic* itemLogic);
		
		void removeLibrary(Anandamide::Library* lib);
		
		void setTabIndex(int index);
		void setTabText(const QString& text);
		int getTabsCount();
		int getCurrentTab();
		bool getShiftFlag();
		
		int addCollection(EditorWidgetsCollection* collection, const char* title, bool topProject);
		void connectToCollection(EditorWidgetsCollection* collection);
		void disconnectFromCollection();
		void setActionsEnabled(bool enabled);
		
		void closeAllTabs(bool removeInWorkspace, int exludeIndex); 
		EditorWidgetsCollection* currentCollection;
		
		bool debugStepIntoMode;
		
		Anandamide::Script* createScript();
		void setMessangerView(EditorWidgetsCollection* collection);
		bool openLibrary(const QString& libraryName);
		
		void setCursorShape(Qt::CursorShape shape);
		
		void animationCameraPos(const Anandamide::vec3 &newPos);
		
		void message(const char *message);
		void messageExt(const char *message, bool endl, Qt::GlobalColor color, bool bold, bool italic);
		
		int setCurrentTabTextColor(const QColor& color);
		void setCurrentScript(QStringList &logicLibList);
		
		bool debugPauseMode;
		bool debugForwardMode;
		
		void openScriptView(Anandamide::Script* script);
		void runOnSelection();
		void updateProjectEditable();
		
		bool reloadCurrentProject();
		void updateProjectTree();
	private:
		
		Ui::MainWindow *ui;
		Anandamide::Script *script;
		Anandamide::AnandamideEngine *scriptEngine;
		bool editableFlag;
		
		QVariantAnimation* debugCameraAnimation;
		
		void focusInEvent(QFocusEvent *ev);
		
		void updateTitle();

		void saveProjectEditorSettings(const QString &project_name);
		void loadProjectEditorSettings(const QString &project_name);

		//Сохраняет настройки в поток ввода-вывода. Сохраняемые настройки определяются здесь
		bool readSettings(QIODevice* dev);
		//Загружает настройки из потока ввода-вывода. Загружаемые настройки определяются здесь
		bool writeSettings(QIODevice* dev) const;

		//Сохраняет настройки в файл "AnandamideEditor.settings"
		void readSettings();
		//Загружает настройки из файла "AnandamideEditor.settings"
		void writeSettings() const;

		//Загружает настройки из буфера
		bool restoreEditorSettings(const QByteArray &sets);
		//Сохраняет настройки в буфер
		const QByteArray saveEditorSettings() const;

		QByteArray defaultSettings;
		QLineEdit *searchLine;

	private slots:


		
		void on_actionSaveProject_triggered();
		void on_actionOpenProject_triggered();
		
		void on_actionNew_triggered();
		
		void on_actionSaveAsProject_triggered();

		void on_actionRestore_default_view_triggered();

		void on_actionShowScriptTypes_triggered();

		void on_actionShiftOnWheel_triggered(bool checked);

		void on_actionCloseScript_triggered();
		
		void onTabMoved(int from, int to);
		
		void on_tabWidgetViewports_currentChanged(int index);
		
		void on_actionNew_Workspace_triggered();
		
		void on_actionOpen_Workspace_triggered();
		
		void on_actionSave_Workspace_triggered();
		
		void on_actionSave_Workspace_As_triggered();
		
		void on_actionSave_All_triggered();
		
		void onDebugCameraAnimation(const QVariant& value);
		
		void on_actionRun_triggered();
		void on_actionStopScript_triggered();
		
		void onScriptStarted();
		void onScriptFinished();

        void onSearchGoBackBthClicked();
        void onSearchGoFwdBtnClicked();
        void onSearchBtnClicked();
		
public slots:
		void keyPressEvent(QKeyEvent *event);
		void showEvent(QShowEvent *);
		void closeEvent(QCloseEvent *);
		void onDelegateChangeVariablePressed(DelegateTypeEditor* delegate);
		void onDelegateVariableChanged(Anandamide::Variable* variable);
		
		//событие изменения логики сценария
		void onChangeLogic();
		//событие изменения месторасположения блоков
		void onChangeLocation();
		//Закрытие вкладки со скриптом
		void on_tabWidgetViewports_tabCloseRequested(int index);
		
signals:
		void runScriptPrepare();
		void stopScriptPrepare();
		void tabClosed(QVariant);
		void allTabClosed();
		void tabMoved(QVariant, QVariant);
		void changed();
};

bool selectTypeDialog(Anandamide::TypeInfo &type);

extern MainWindow *main_window;
extern LibraryDialog *library_dialog;
extern TypeDialog *type_dialog;

//------------------------------------------------------------------------------
//
// class DelegateTypeEditor
//
//------------------------------------------------------------------------------

class DelegateTypeEditor : public QWidget
{
	Q_OBJECT

	QLineEdit* editor;
	QToolButton* btn;
	Anandamide::Variable *m_variable;
	QTreeWidgetItem* m_item;

public:
	DelegateTypeEditor(QTreeWidgetItem* item, Anandamide::Variable *variable, QWidget* parent = 0);

	void setStringData(const QString& data);

	const QString getStringData() const;

	void updateStringData(const QString& data);

private slots:
	void onBtnPressed();

	void onReturnPressed();

signals:
	void changeVariablePressed(DelegateTypeEditor* delegate);
	void variableChanged(Anandamide::Variable *variable);
};

//------------------------------------------------------------------------------
//
// class MainLogicEditor
//
//------------------------------------------------------------------------------

class MainLogicEditor : public Anandamide::LogicEditor {
	
protected:
	
	void onLinkCreateBlock(float x, float y);
	
public:
	
	
};


class SearchBaseWidget : public QWidget
{
	QTreeWidget *list;
	QGridLayout *layout;
	Anandamide::Script *lastScript;
	Q_OBJECT
public:
	SearchBaseWidget(QWidget *parent = 0);
	Anandamide::Script *getLastScript();
protected:
	QTreeWidget *getList();
	
	QGridLayout *getLayout();
	void addItem(Anandamide::Neurone *neurone);
	
	virtual void setItemInfo(QTreeWidgetItem *item, Anandamide::Neurone *neurone) = 0;
	virtual bool algorithm(Anandamide::Neurone *neurone) = 0;
	virtual void inplaceClear();
	
public slots:
	void search();
	void clear();
	void onListClicked(QTreeWidgetItem *item, int column);
};

class ErrorSearch : public SearchBaseWidget
{
	Q_OBJECT
	QMap <int, QString> errors;
public:
	ErrorSearch(QWidget *parent = 0);
protected:
	void setItemInfo(QTreeWidgetItem *item, Anandamide::Neurone *neurone);
	bool algorithm(Anandamide::Neurone *neurone);
};

class QLabel;

class SearchWidget : public SearchBaseWidget
{
	Q_OBJECT
	QLabel *sprite;
	bool regExpEnabled;
	bool completeWordsEnabled;
	bool caseEnabled;
	QString query;
public:
	SearchWidget(QWidget *parent = 0);
protected:
	void setItemInfo(QTreeWidgetItem *item, Anandamide::Neurone *neurone);
	bool algorithm(Anandamide::Neurone *neurone);
	bool inAlgoritm(QString str);
public slots:
	QString &lastQuery();
	void setQuery(QString query);
	void setCompleteWordsEnabled(bool enabled);
	void setCaseEnabled(bool enabled);
	void setRegExpEnabled(bool enabled);
	void goForward();
	void goBack();
};

#endif // MAINWINDOW_H
