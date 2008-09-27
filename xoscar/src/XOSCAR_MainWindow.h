/*
 *  Copyright (c) 2007-2008 Oak Ridge National Laboratory, 
 *                          Geoffroy Vallee <valleegr@ornl.gov>
 *                          All rights reserved
 *  This file is part of the xoscar software, part of the OSCAR software.
 *  For license information, see the COPYING file in the top level directory
 *  of the OSCAR source.
 */

/**
 * @file XOSCAR_MainWindow.h
 * @brief Defines the class XOSCAR_MainWindow for the main xoscar widget
 * @author Geoffroy Vallee
 *
 */

#ifndef XOSCAR_MAINWINDOW_H
#define XOSCAR_MAINWINDOW_H

#include <QApplication>
#include <QPushButton>
#include <QLabel>
#include <QWidget>
#include <QString>
#include <QMainWindow>
#include <QTextStream>
#include <QCloseEvent>
#include <QMessageBox>

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <fstream>
#include <unistd.h>

#include "ui_xoscar_mainwindow.h"
#include "ORM_AddRepoGUI.h"
#include "ORM_AddDistroGUI.h"
#include "CommandExecutionThread.h"
#include "ORM_WaitDialog.h"
#include "XOSCAR_AboutAuthorsDialog.h"
#include "XOSCAR_AboutOscarDialog.h"
#include "XOSCAR_FileBrowser.h"
#include "XOSCAR_TabWidgetInterface.h"
#include "XOSCAR_TabGeneralInformation.h"
#include "XOSCAR_TabNetworkConfiguration.h"
#include "XOSCAR_TabSoftwareConfiguration.h"

using namespace Ui; 
using namespace std;

/**
 * @namespace xoscar
 * @author Geoffroy Vallee
 * @brief The xoscar namespace gathers all classes needed for XOSCAR.
 */
namespace xoscar {

/**
 * @class XOSCAR_MainWindow
 * @author Geoffroy Vallee
 * Class implementing the main window of xoscar. Note that currently xoscar
 * assumes that the environment variable OSCAR_HOME is set and allows one to
 * access to the OSCAR code. Also note that a configuration file is used by
 * xoscar (~/.xoscar.conf).
 */
class XOSCAR_MainWindow : public QMainWindow, public MainWindow
{
Q_OBJECT

public:
    XOSCAR_MainWindow(QMainWindow *parent = 0);
    ~XOSCAR_MainWindow();

public slots:
    void add_repo_to_list ();
    void create_add_distro_window ();
    void create_add_repo_window ();
    void destroy();
    void display_opkgs_from_repo ();
    void do_oscar_sanity_check();
    void do_system_sanity_check();
    void handle_about_authors_action();
    void handle_about_oscar_action();
    void handle_oscar_config_result (QString);
     int handle_thread_result (CommandTask::CommandTasks, QString);
    void kill_popup (QString, QString);
    void newOscarOptionSelected ();
    void refresh_display_opkgs_from_repo();
    void refresh_list_setup_distros();
    void networkConfigTab_currentChanged_handler(int);
    void update_check_text_widget(QString);
	void activate_tab(int tab_num);
	void widgetContentsChanged_handler(QWidget*);
    void command_thread_finished();

protected:
	void closeEvent(QCloseEvent* event);

private:
    XOSCAR_TabWidgetInterface::SaveResult prompt_save_changes();
    bool isWidgetContentsModified(QWidget* widget);

    XOSCAR_AboutAuthorsDialog about_authors_widget;
    XOSCAR_AboutOscarDialog about_oscar_widget;
    ORMAddRepoDialog add_oscar_repo_widget;
    ORMAddDistroDialog add_distro_widget;
    ORMWaitDialog*  wait_popup;
    CommandExecutionThread command_thread;

    XOSCAR_TabGeneralInformation* giTab;
    XOSCAR_TabNetworkConfiguration* networkTab;
    XOSCAR_TabSoftwareConfiguration* softwareTab;

    QWidget* widgetPendingChanges;
    int oscarOptionsRowPendingChanges;
};

} // namespace xoscar

/**
 * @mainpage XOSCAR Reference
 * @htmlinclude mainpage.html
 */


#endif // XOSCAR_MAINWINDOW_H
