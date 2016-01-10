#-------------------------------------------------
#
# Project created by QtCreator 2014-10-21T17:07:52
#
#-------------------------------------------------

QT += core gui help xml opengl

#CONFIG+=APP
#CONFIG+=x64

win32:RC_ICONS += ../data/icons/app.ico

INCLUDEPATH += ../depend/AnandamideAPI/include
INCLUDEPATH += ../depend/AnandamideAPI/src/Common
INCLUDEPATH += ../include

ANANDAMIDE_API_BIN = $$shell_path("../depend/AnandamideAPI/bin/*")

LIBS += -L"../depend/AnandamideAPI/lib"

DESTDIR += ../bin/

QMAKE_CXXFLAGS += -std=c++11

CONFIG(debug, debug|release) {
                TARGET = AnandamideEditor_x86d
		LIBS += -lAnandamideAPI_x86d
                #LIBS += -lopengl32
        message($${TARGET})
} else {
                TARGET = AnandamideEditor_x86
		LIBS += -lAnandamideAPI_x86
                #LIBS += -lopengl32
        message($${TARGET})
	#QMAKE_CXXFLAGS += /Ot /Oy- /GL /MP
}

QMAKE_POST_LINK += $$QMAKE_COPY $${ANANDAMIDE_API_BIN} $$shell_path("../bin/*.*")

FORMS += \
    ../data/MainWindow.ui \
    ../data/RuntimeWindow.ui \
    ../data/LibraryDialog.ui \
    ../data/TypeDialog.ui \
    ../data/SetLogicKindDialog.ui \
    ../data/AnandamideLibraryWidget.ui \
    ../data/ConsoleOutWidget.ui \
    ../data/AnandamideLogWidget.ui

RESOURCES += \
    ../data/icons.qrc

HEADERS += \
    ../include/AnandamideViewport.h \
    ../include/MainWindow.h \
    ../include/RuntimeWindow.h \
    ../include/Timer.h \
    ../include/AnandamideLibraryTree.h \
    ../include/TreeItemBase.h \
    ../include/LibraryDialog.h \
    ../include/TypeDialog.h \
    ../include/AnandamideEditorGlobal.h \
    ../include/XmlSettings.h \
    ../include/TreeItemClasses.h \
    ../include/SetLogicKindDialog.h \
    ../include/AnandamideWorkspaceTree.h \
    ../include/AnandamideLibraryWidget.h \
    ../include/EditorWidgetsCollection.h \
    ../include/ConsoleOutWidget.h \
    ../include/AnandamideLogWidget.h \
    ../include/AnandamideEngine.h

SOURCES += \
    ../src/AnandamideViewport.cpp \
    ../src/main.cpp \
    ../src/MainWindow.cpp \
    ../src/RuntimeWindow.cpp \
    ../src/Timer.cpp \
    ../src/AnandamideLibraryTree.cpp \
    ../src/TreeItemBase.cpp \
    ../src/LibraryDialog.cpp \
    ../src/TypeDialog.cpp \
    ../src/XmlSettings.cpp \
    ../src/TreeItemClasses.cpp \
    ../src/SetLogicKindDialog.cpp \
    ../src/AnandamideWorkspaceTree.cpp \
    ../src/AnandamideLibraryWidget.cpp \
    ../src/EditorWidgetsCollection.cpp \
    ../src/ConsoleOutWidget.cpp \
    ../src/AnandamideLogWidget.cpp \
    ../src/AnandamideEngine.cpp
