/*
 *  Copyright (c) 2007-2008 Oak Ridge National Laboratory, 
 *                          Geoffroy Vallee <valleegr@ornl.gov>
 *                          All rights reserved
 *  This file is part of the xoscar software, part of the OSCAR software.
 *  For license information, see the COPYING file in the top level directory
 *  of the OSCAR source.
 */

/**
 * @file XOSCAR_MainWindow.cpp
 * @brief Actual implementation of the XOSCAR_MainWindow class.
 * @author Geoffroy Vallee
 */

#include <QDir>
#include "XOSCAR_MainWindow.h"
#include "SimpleConfigFile.h"
#include "Hash.h"
#include "utilities.h"
#include <QMetaType>

using namespace xoscar;

Q_DECLARE_METATYPE(xoscar::CommandId)

/**
 * @author Geoffroy Vallee
 *
 * Class constructor: 
 * - initializes the widget,
 * - connects signals and slots,
 * - read the configuration file (~/.xoscar.conf); the file is created with
 *   default values if it does not already exist,
 * - gets the list of default OSCAR repositories.
 */
XOSCAR_MainWindow::XOSCAR_MainWindow(QMainWindow *parent)
    : QMainWindow(parent) 
    , add_oscar_repo_widget(dynamic_cast<ThreadHandlerInterface*>(this))
    , add_distro_widget(dynamic_cast<ThreadHandlerInterface*>(this), this)
    , widgetPendingChanges(NULL)
    , oscarOptionsRowPendingChanges(-1)
    , wait_dialog(this)
    , closeRequest(false)
{
    // register this enum so it can be used in the queued connections
    qRegisterMetaType<xoscar::CommandId>();

    setupUi(this);

    wait_dialog.setModal(true);

    // The QTabWidget widget will not allow 0 tabs via the designer method so
    // it will have a "dummy" tab that needs to be removed.
    networkConfigurationTabWidget->removeTab(0);

    giTab = new XOSCAR_TabGeneralInformation(dynamic_cast<ThreadHandlerInterface*>(this), networkConfigurationTabWidget);
    networkTab = new XOSCAR_TabNetworkConfiguration(dynamic_cast<ThreadHandlerInterface*>(this), networkConfigurationTabWidget);
    softwareTab = new XOSCAR_TabSoftwareConfiguration(dynamic_cast<ThreadHandlerInterface*>(this), networkConfigurationTabWidget);

    // Add the tabs dynamically
    networkConfigurationTabWidget->insertTab(0, giTab, tr("General Information"));
    networkConfigurationTabWidget->insertTab(1, networkTab, tr("Network Configuration"));
    networkConfigurationTabWidget->insertTab(2, softwareTab, tr("Software Configuration"));

    networkConfigurationTabWidget->setCurrentIndex(0);

    connect(giTab, SIGNAL(partition_selection_changed(QString)),
            networkTab, SLOT(partition_selection_changed(QString)));
    connect(giTab, SIGNAL(partition_selection_changed(QString)),
            softwareTab, SLOT(partition_selection_changed(QString)));
    connect(giTab, SIGNAL(cluster_selection_changed(QString)),
            softwareTab, SLOT(cluster_selection_changed(QString)));

    connect(giTab, SIGNAL(widgetContentsModified(QWidget*)),
            this, SLOT(widgetContentsChanged_handler(QWidget*)));

    /* Connect slots and signals */
    connect(AddOSCARRepoButton, SIGNAL(clicked()),
                    this, SLOT(create_add_repo_window()));
    connect(addDistroButton, SIGNAL(clicked()),
                    this, SLOT(create_add_distro_window()));
    connect(listReposWidget, SIGNAL(itemSelectionChanged ()),
                    this, SLOT(display_opkgs_from_repo()));
    connect(refreshListOPKGsButton, SIGNAL(clicked()),
                    this, SLOT(refresh_display_opkgs_from_repo()));
    connect(refreshListSetupDistrosButton, SIGNAL(clicked()),
                    this, SLOT(refresh_list_setup_distros()));
    connect(systemSanityCheckButton, SIGNAL(clicked()),
                    this, SLOT(do_system_sanity_check()));
    connect(oscarSanityCheckButton, SIGNAL(clicked()),
                    this, SLOT(do_oscar_sanity_check()));

    /* Connect button sinals */
    connect(actionAboutXOSCAR, SIGNAL(triggered()),
                    this, SLOT(handle_about_authors_action()));
    connect(actionAbout_OSCAR, SIGNAL(triggered()),
                    this, SLOT(handle_about_oscar_action()));

    /* signals related to tabs */
    connect(networkConfigurationTabWidget, SIGNAL(currentChanged (int)),
                    this, SLOT(networkConfigTab_currentChanged_handler(int)));

    // TODO remove/replace these
    connect(&command_thread, SIGNAL(opd_done(QString, QString)),
        this, SLOT(kill_popup(QString, QString)));

    connect(&command_thread, SIGNAL(sanity_command_done(QString)),
        this, SLOT(update_check_text_widget(QString)));
    // END

    /* signals related to the CommandExecutionThread */
    connect(&command_thread, SIGNAL(thread_terminated(xoscar::CommandId, QString, ThreadUserInterface*)),
        this, SLOT(handle_thread_result (xoscar::CommandId, QString, ThreadUserInterface*)));
    connect(&command_thread, SIGNAL(finished()),
            this, SLOT(command_thread_finished()));

    connect (this->listOscarOptionsWidget, 
             SIGNAL(itemSelectionChanged()),
             this,
             SLOT(newOscarOptionSelected()));

    connect (&add_distro_widget, SIGNAL (refreshListDistros()),
            this, SLOT(refresh_list_setup_distros()));

    /* Get the list of Linux distributions that are already setup. */
    cout << "Get the setup distros" << endl;
    enqueue_command_task(CommandTask(xoscar::GET_SETUP_DISTROS, QStringList(""), dynamic_cast<ThreadUserInterface*>(this)));
    cout << "Init done" << endl;
}

XOSCAR_MainWindow::~XOSCAR_MainWindow() 
{
}

/**
 * @author Geoffroy Vallee.
 *
 * Trigger the selection of the different OSCAR options (options on the 
 * left-hand side of the main GUI). The selection will activate the proper
 * widget page, giving the impression of using a standard "configuration 
 * widget".
 */
void XOSCAR_MainWindow::newOscarOptionSelected() 
{
    if(isWidgetContentsModified(widgetPendingChanges) && 
        listOscarOptionsWidget->currentRow() == oscarOptionsRowPendingChanges) {
        return;
    }

    XOSCAR_TabWidgetInterface::SaveResult result = prompt_save_changes();
    // if user canceled or the save failed, show previous oscar option
    if (result == XOSCAR_TabWidgetInterface::UserCanceled || 
        result == XOSCAR_TabWidgetInterface::SaveFailed) {
        listOscarOptionsWidget->setCurrentRow(oscarOptionsRowPendingChanges);
        return;
    } 

    QString option;

    /* We get the selected repo. Note that the selection widget supports 
       currently a unique selection. */
    QList<QListWidgetItem *> list = listOscarOptionsWidget->selectedItems();

    if(list.count() == 0) {
        return;
    }

    QListIterator<QListWidgetItem *> i(list);
    option = i.next()->text();

    if (option == "OSCAR Configuration") {
        stackedOptionsWidget->setCurrentIndex (0);
    } else if (option == "Cluster Management") {
        stackedOptionsWidget->setCurrentIndex (1);
    } else {
        cerr << "ERROR: Unknown option (" << option.toStdString() << ")" 
             << endl;
    }
}

/**
 * @author Geoffroy Vallee.
 *
 * Slot executed when a OPD2 command ends (executed by the 
 * CommandExecutionThread). In this case, we need to update the list of OPKGs or
 * the list of repos, and we also need to close the dialog that asks to user to
 * wait while we are executing a OPD2 command.
 * Note the return depends on the query mode (see CommandExecutionThread.h file
 * for the list of supported mode). It means that the thread used for the 
 * execution of OPD2 commands can only do one query at a time. The mode defines
 * the type of query that as to be done (for example get the list of available
 * OPKGs for a specific repo or get the list of available repo).
 * Based on the mode only one result is returned for the OPD2 command execution
 * thread, others are empty QStrings.
 * Also note that we return QString because Qt signals can only deal by default
 * with very specific types. Therefore we use QString to simplify the 
 * implementation (natively supported).
 *
 * @param list_repos List of OSCAR repositories; result of the OPD2 command. The
 *                   list is empty is users request something else than the list
 *                   of repos.
 * @param list_opkgs List of OSCAR packages available via a specific OSCAR 
 *                   repository; result of the OPD2 command. The list is empty 
 *                   is users request something else than the OPKGs list.
 * @todo This function can be simplified using the split function from Qt
 */
void XOSCAR_MainWindow::kill_popup(QString list_repos, QString list_opkgs)
{
    /* We update the list of available OPKGs for the selected OSCAR repo */
    QStringList list = list_opkgs.split (" ");
    listOPKGsWidget->clear();
    for(int i = 0; i < list.size(); i++) {
        this->listOPKGsWidget->addItem (list.at(i));
    }
    listOPKGsWidget->update();

    /* We update the list of available OSCAR repos */
    list = list_repos.split (" ");
    for(int i = 0; i < list.size(); i++) {
        this->listReposWidget->addItem (list.at(i));
    }
    listReposWidget->update();

    /* We close the popup window that asks the user to wait */
    wait_popup->close();
}


/**
 * @author Geoffroy Vallee.
 *
 * Slot that handles the click on the "Add repo" button: it creates a dialog 
 * that allows users to enter a repository URL (with the yume or RAPT syntax).
 */
void XOSCAR_MainWindow::create_add_repo_window() 
{
    QObject::connect(add_oscar_repo_widget.buttonBox, SIGNAL(accepted()),
                     this, SLOT(add_repo_to_list()));
    add_oscar_repo_widget.show();
}

/**
 * @author Geoffroy Vallee.
 *
 * Slot that handles the click on the "Add distro" button: it creates a dialog 
 * that allows users to select distros that are available but not yet setup for
 * OSCAR.
 */
void XOSCAR_MainWindow::create_add_distro_window() 
{
    add_distro_widget.show();
    add_distro_widget.refresh_list_distros();
}


/**
 * @author Geoffroy Vallee
 *
 * Slot that handles the selection of a specific repository in the list. For
 * that, we get the repository URL from the widget and we execute the opd2 
 * command execution thread. We also display the please wait dialog during the
 * execution of the thread.
 */
void XOSCAR_MainWindow::display_opkgs_from_repo()
{
    QString repo;

    /* We get the selected repo. Note that the selection widget supports 
       currently a unique selection. */
    QList<QListWidgetItem *> list = listReposWidget->selectedItems();
    QListIterator<QListWidgetItem *> i(list);
    if (list.size() > 0) {
        repo = i.next()->text();
        wait_popup = new ORMWaitDialog(0, repo);
        wait_popup->show();
        update();

        enqueue_command_task(CommandTask(xoscar::GET_LIST_OPKGS, QStringList(repo)));
    }
}

/**
 * @author Geoffroy Vallee
 *
 * Slot for the signal emitted when clicking on the buttun that refreshes the
 * list of OPKGs available from a given repository.
 */
void XOSCAR_MainWindow::refresh_display_opkgs_from_repo()
{
    listOPKGsWidget->clear();
    display_opkgs_from_repo();
}

/**
 * @author Geoffroy Vallee.
 *
 * Slot that handles the click of the ok button of the widget used to enter the
 * URL of a new repository. We get the URL and display the "Please wait" dialog,
 * the time to execute the OPD2 command (the actual query).
 */
void XOSCAR_MainWindow::add_repo_to_list()
{
    QString repo_url = add_oscar_repo_widget.lineEdit->text();

    wait_popup = new ORMWaitDialog(0, repo_url);
    wait_popup->show();
    update();

    enqueue_command_task(CommandTask(xoscar::GET_LIST_REPO, QStringList(repo_url)));
    enqueue_command_task(CommandTask(xoscar::GET_LIST_OPKGS, QStringList(repo_url)));
}

/**
 * @author Geoffroy Vallee
 *
 * Display the list of setup distros. For that we get the list via a string, 
 * each distribution identifier being separated by a space. Therefore, we parse
 * the string and update the widget that shows the setup distros.
 *
 * @param list_distros String that represents the list of setup Linux 
 *                     distributions. Each distribution identifier is separated
 *                     by a space.
 */
void XOSCAR_MainWindow::handle_oscar_config_result(QString list_distros)
{
    cout << list_distros.toStdString () << endl;
    QStringList list = list_distros.split (" ", QString::SkipEmptyParts);
    listSetupDistrosWidget->clear();
    for(int i = 0; i < list.size(); i++) {
        this->listSetupDistrosWidget->addItem (list.at(i));
    }
    listSetupDistrosWidget->update();
    enqueue_command_task(CommandTask(xoscar::INACTIVE, QStringList("")));
}

void XOSCAR_MainWindow::refresh_list_setup_distros()
{
    listSetupDistrosWidget->clear();
    enqueue_command_task(CommandTask(xoscar::GET_SETUP_DISTROS, QStringList("")));
}

void XOSCAR_MainWindow::do_system_sanity_check()
{
    sanityCheckTextWidget->clear();
    enqueue_command_task(CommandTask(xoscar::DO_SYSTEM_SANITY_CHECK, QStringList("")));
}

void XOSCAR_MainWindow::do_oscar_sanity_check()
{
    sanityCheckTextWidget->clear();
    enqueue_command_task(CommandTask(xoscar::DO_OSCAR_SANITY_CHECK, QStringList("")));
}

/**
 * @author Geoffroy Vallee
 *
 * Slot called in order to update the text of the widget that gives the output
 * of the sanity check command.
 *
 * @param text Text with which we have update the widget.
 */
void XOSCAR_MainWindow::update_check_text_widget(QString text)
{
    sanityCheckTextWidget->setText(text);
}

/**
 * @author Geoffroy Vallee
 *
 * Slot called when the Quit button of the main window is clicked. This slot
 * makes sure that all xoscar related dialog are closed before exiting the
 * application.
 */
void XOSCAR_MainWindow::closePopups()
{
    add_distro_widget.close();
    add_oscar_repo_widget.close();
    about_authors_widget.close();
    about_oscar_widget.close();
}

/**
 *  @author Robert Babilon
 *
 *  Qt function called when the application is about to close.
 *  Allows overriding to continue with the close event or ignore it.
 *  The idea is to check one last time if any of the tabs were
 *  modified before closing the application.
 */
void XOSCAR_MainWindow::closeEvent(QCloseEvent* event)
{
    if(isWidgetContentsModified(widgetPendingChanges) == true) { 
        prompt_save_changes();
    }
    
    if(isWidgetContentsModified(widgetPendingChanges) == false) { 
        closeRequest = true;
        closePopups();
        event->accept();
    }
    else {
        event->ignore();
    }
}

/**
 * @author Geoffroy Vallee
 *
 * Slot called when the menu item to have information of authors is clicked.
 * The widget displaying authors is then shown.
 */
void XOSCAR_MainWindow::handle_about_authors_action()
{
    about_authors_widget.show();
}

/**
 * @author Geoffroy Vallee
 *
 * Slot called when the menu item to have OSCAR information is clicked.
 * The widget displaying OSCAR info is then shown.
 */
void XOSCAR_MainWindow::handle_about_oscar_action()
{
    about_oscar_widget.show();
}

/**
 *  @author Robert Babilon
 *
 *  Slot called when a tab that implements the XOSCAR_TabWidgetInterface
 *  has been modified by the user.
 *
 *  @param widget Tab that has been modified.
 *
 */
void XOSCAR_MainWindow::widgetContentsChanged_handler(QWidget* widget)
{
    if(widget == NULL) {
        cout << "ERROR: widget is NULL" << endl;
        return;
    }

    if(widgetPendingChanges == widget || isWidgetContentsModified(widgetPendingChanges) == false) {
        widgetPendingChanges = widget;
        oscarOptionsRowPendingChanges = listOscarOptionsWidget->currentRow();
    }
    else {
        cout << "ERROR: a previous tab has not been saved." << endl;
    }
}

/**
 * @author Geoffroy Vallee.
 *
 * Slot called when a tab from "Cluster Management" is activated. This function
 * then called specific functions for each tab.
 *
 * @param tab_num Index of the activated tab.
 */
void XOSCAR_MainWindow::networkConfigTab_currentChanged_handler(int tab_num) 
{
    if(networkConfigurationTabWidget->currentWidget() == widgetPendingChanges) {
        // ignore
        return;
    }
    XOSCAR_TabWidgetInterface::SaveResult result = prompt_save_changes();
    // if user canceled or save fails, show last tab widget
    if(result == XOSCAR_TabWidgetInterface::UserCanceled || 
       result == XOSCAR_TabWidgetInterface::SaveFailed) {
        // if cancel changes, revert to last known tab
        networkConfigurationTabWidget->setCurrentWidget(widgetPendingChanges);
    }
    else {
        activate_tab(tab_num);
    }
}

/** 
 * @author Robert Babilon
 *
 * This function calls XOSCAR_TabWidgetInterface::save() or
 * XOSCAR_TabWidgetInterface::undo() on the widget that has changes pending.  If
 * the widget does not implement XOSCAR_TabWidgetInterface, then the function
 * ignores it and returns XOSCAR_TabWidgetInterface::NoChange.
 *
 * @return XOSCAR_TabWidgetInterface::SaveResult
 */
XOSCAR_TabWidgetInterface::SaveResult XOSCAR_MainWindow::prompt_save_changes()
{
    XOSCAR_TabWidgetInterface::SaveResult result = XOSCAR_TabWidgetInterface::NoChange;

    if(isWidgetContentsModified(widgetPendingChanges)) {
        QMessageBox msg(QMessageBox::NoIcon, tr("Save changes?"), tr("The previous tab has been modified.\n")
                                                                + tr("Would you like to save your changes?"),
                        QMessageBox::Save|QMessageBox::No|QMessageBox::Cancel, this);

        XOSCAR_TabWidgetInterface* tab = dynamic_cast<XOSCAR_TabWidgetInterface*>(widgetPendingChanges);

        switch(msg.exec()) {
            case QMessageBox::Save: 
                result = tab->save();
                break;
            case QMessageBox::No:
                result = tab->undo();
                break;
            case QMessageBox::Cancel:
                result = XOSCAR_TabWidgetInterface::UserCanceled;
                break;
        }
    }
    
    return result;
}

void XOSCAR_MainWindow::activate_tab(int tab_num)
{
    switch (tab_num) {
        case (1) : networkTab->network_configuration_tab_activated();
            break;
        case (2) : softwareTab->software_configuration_tab_activated();
            break;
    }
}

/**
 * @author Robert Babilon
 *
 * Slot called when the command thread has finished executing a command.
 * Calls CommandExecutionThread::wakeThread() before returning to ensure the
 * thread exits CommandExecutionThread::run().
 *
 * @param command_id The command that has completed. The list of values
 * are in CommandTask.h.
 * @param result Holds the return value of the command.
 */
int XOSCAR_MainWindow::handle_thread_result (xoscar::CommandId command_id, 
    QString result, ThreadUserInterface* threaduser)
{
    if(threaduser != NULL) {
        cout << "DEBUG: Calling threaduser: " << threaduser << endl;
        threaduser->handle_thread_result(command_id, result);
        //TODO call any other thread users that want to know about this command id
    }

    QStringList list;
    cout << "MainWindow: result from cmd exec thread received: "
         << command_id
         << endl;
    if (command_id == xoscar::GET_LIST_DEFAULT_REPOS) {
        // We parse the result: one URL per line.
        if (listReposWidget == NULL) {
            cerr << "ERROR: Impossible to update the widget that gives "
                    << "the list of repos, it seems the widget does not exist"
                    << endl;
            return -1;
        }
        list = result.split("\n", QString::SkipEmptyParts);
        for (int i = 0; i < list.size(); ++i){
            listReposWidget->addItem (list.at(i));
        }
        listReposWidget->update();
    } else if (command_id == xoscar::SETUP_DISTRO) {
        // We could here try to see if the command was successfully executed or
        // not. Otherwise, nothing to do here.
    } else if (command_id == xoscar::GET_SETUP_DISTROS) {
        handle_oscar_config_result(result);
        enqueue_command_task(CommandTask(xoscar::GET_LIST_DEFAULT_REPOS, QStringList ("")));
    }

    command_thread.wakeThread();
    return 0;
}

/**
 * @author Robert Babilon
 *
 * Slot called when the QThread signal finished() is emitted.
 * Starts the command thread again only if it has tasks left.
 */
void XOSCAR_MainWindow::command_thread_finished()
{
    if(!command_thread.isEmpty()) { 
        command_thread.start();
    }
    else {
        wait_dialog.threadNotify();
        emit command_thread_tasks_done();

        if(closeRequest) {
            wait_dialog.hide();
            close();
        }
    }
}

/**
 * @author Robert Babilon
 *
 * Checks if the QWidget is modified. If the QWidget does not implement
 * the XOSCAR_TabWidgetInterface interface, this function returns false.
 *
 * @param widget QWidget pointer to check for modifications.
 * @return true if the widget is modified; otherwise false.
 */
bool XOSCAR_MainWindow::isWidgetContentsModified(QWidget* widget)
{
    XOSCAR_TabWidgetInterface* tab = dynamic_cast<XOSCAR_TabWidgetInterface*>(widget);
    return (tab != NULL && tab->isModified()) ? true : false; 
}

/**
 * @author Robert Babilon
 * Method to show the generic wait dialog when processing command tasks.
 * @param message The initial message to set to the dialogs label.
 */
void XOSCAR_MainWindow::show_generic_wait_dialog(QString message)
{
    wait_dialog.setLabelText(message);
    wait_dialog.show();
    wait_dialog.startTimer();
}

/**
 * @author Robert Babilon
 * Method to add a CommandTask to the CommandExecutionThread's queue.
 * @param message The initial message to set to the dialogs label.
 */
void XOSCAR_MainWindow::enqueue_command_task(CommandTask task, QString message)
{
    show_generic_wait_dialog(message != tr("") ? message : tr("Please wait for XOSCAR to process command(s)."));
    command_thread.init(task);
}
