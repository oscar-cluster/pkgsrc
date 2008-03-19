######################################################################
# Automatically generated by qmake (2.01a) Sat Nov 17 16:29:42 2007
######################################################################

TEMPLATE = app
TARGET = 
DEPENDPATH += . src
INCLUDEPATH += .

# Input
FORMS +=    src/xoscar_mainwindow.ui \
            src/AddRepoWidget.ui \
            src/AddDistroWidget.ui \
            src/WaitDialog.ui \
            src/AboutAuthorsDialog.ui \
            src/AboutOscarDialog.ui \
            src/FileBrowser.ui

SOURCES +=  src/main.cpp \
            src/XOSCAR_MainWindow.cpp \
            src/ORM_AddRepoGUI.cpp \
            src/ORM_AddDistroGUI.cpp \
            src/ORM_WaitDialog.cpp \
            src/CommandExecutionThread.cpp \
            src/XOSCAR_AboutAuthorsDialog.cpp \
            src/XOSCAR_AboutOscarDialog.cpp \
            src/XOSCAR_FileBrowser.cpp \
            src/Hash.cpp \
            src/SimpleConfigFile.cpp

HEADERS +=  src/XOSCAR_MainWindow.h \
            src/ORM_AddRepoGUI.h \
            src/ORM_AddDistroGUI.h \
            src/ORM_WaitDialog.h \
            src/CommandExecutionThread.h \
            src/XOSCAR_AboutAuthorsDialog.h \
            src/XOSCAR_AboutOscarDialog.h \
            src/XOSCAR_FileBrowser.h \
            src/Hash.h \
            src/SimpleConfigFile.h

RESOURCES += xoscar_resource.qrc
