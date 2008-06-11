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

using namespace xoscar;

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
{
    setupUi(this);

    // We read the xoscar configuration file (~/.xoscar.conf). Note that if the
    // file does not exist, a default configuration file is created.
//     QString home_path = getenv("HOME");
//     QDir dir (home_path);
//     if ( !dir.exists() ) {
//         cout << "ERROR: Impossible to find the home directory" << endl;
//         return;
//     }
//     home_path = home_path + "/.xoscar.conf";
//     SimpleConfigFile confFile = SimpleConfigFile (home_path.toStdString());

    /* Connect slots and signals */
    connect(AddOSCARRepoButton, SIGNAL(clicked()),
                    this, SLOT(create_add_repo_window()));
    connect(addDistroButton, SIGNAL(clicked()),
                    this, SLOT(create_add_distro_window()));
    connect(listReposWidget, SIGNAL(itemSelectionChanged ()),
                    this, SLOT(display_opkgs_from_repo()));
    connect(listOscarClustersWidget, SIGNAL(itemSelectionChanged ()),
                    this, SLOT(refresh_list_partitions()));
    connect(listClusterPartitionsWidget, SIGNAL(itemSelectionChanged ()),
                    this, SLOT(refresh_partition_info()));
    connect(refreshListOPKGsButton, SIGNAL(clicked()),
                    this, SLOT(refresh_display_opkgs_from_repo()));
    connect(refreshListSetupDistrosButton, SIGNAL(clicked()),
                    this, SLOT(refresh_list_setup_distros()));
    connect(systemSanityCheckButton, SIGNAL(clicked()),
                    this, SLOT(do_system_sanity_check()));
    connect(oscarSanityCheckButton, SIGNAL(clicked()),
                    this, SLOT(do_oscar_sanity_check()));

    /* Connect button sinals */
    connect(QuitButton, SIGNAL(clicked()),
                    this, SLOT(destroy()));
    connect(addPartitionButton, SIGNAL(clicked()),
                    this, SLOT(add_partition_handler()));
    connect(saveClusterInfoButton, SIGNAL(clicked()),
                    this, SLOT(save_cluster_info_handler()));
    connect(saveClusterInfoButton, SIGNAL(clicked()),
                    this, SLOT(refresh_list_partitions()));
    connect(importfilebrowse, SIGNAL(clicked()),
                    this, SLOT(open_file()));
    connect(importmacs, SIGNAL(clicked()),
                    this, SLOT(import_macs_from_file()));

    connect(actionAboutXOSCAR, SIGNAL(triggered()),
                    this, SLOT(handle_about_authors_action()));
    connect(actionAbout_OSCAR, SIGNAL(triggered()),
                    this, SLOT(handle_about_oscar_action()));

    /* signals related to tabs */
    connect(networkConfigurationTabWidget, SIGNAL(currentChanged (int)),
                    this, SLOT(tab_activated(int)));

    connect(&command_thread, SIGNAL(opd_done(QString, QString)),
        this, SLOT(kill_popup(QString, QString)));

    connect(&command_thread, SIGNAL(oscar_config_done(QString)),
        this, SLOT(handle_oscar_config_result(QString)));

    connect(&command_thread, SIGNAL(sanity_command_done(QString)),
        this, SLOT(update_check_text_widget(QString)));

    connect(&command_thread, SIGNAL(thread_terminated(int, QString)),
        this, SLOT(handle_thread_result (int, QString)));

    connect (this->listOscarOptionsWidget, 
             SIGNAL(itemSelectionChanged()),
             this,
             SLOT(newOscarOptionSelected()));

    connect (&add_distro_widget, SIGNAL (refreshListDistros()),
            this, SLOT(refresh_list_setup_distros()));

    /* Get the list of Linux distributions that are already setup. */
    cout << "Get the setup distros" << endl;
    command_thread.init (GET_SETUP_DISTROS, QStringList(""));
    command_thread.wait();
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
    QString option;

    /* We get the selected repo. Note that the selection widget supports 
       currently a unique selection. */
    QList<QListWidgetItem *> list = listOscarOptionsWidget->selectedItems();
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

        command_thread.init(GET_LIST_OPKGS, QStringList(repo));
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

    command_thread.init(GET_LIST_REPO, QStringList(repo_url));
    command_thread.init(GET_LIST_OPKGS, QStringList(repo_url));
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
    QStringList list = list_distros.split (" ");
    for(int i = 0; i < list.size(); i++) {
        this->listSetupDistrosWidget->addItem (list.at(i));
        partitionDistroComboBox->addItem (list.at(i));
    }
    listSetupDistrosWidget->update();
    command_thread.init(INACTIVE, QStringList(""));
}

void XOSCAR_MainWindow::refresh_list_setup_distros()
{
    listSetupDistrosWidget->clear();
    command_thread.init (GET_SETUP_DISTROS, QStringList(""));
}

void XOSCAR_MainWindow::do_system_sanity_check()
{
    sanityCheckTextWidget->clear();
    command_thread.init (DO_SYSTEM_SANITY_CHECK, QStringList(""));
}

void XOSCAR_MainWindow::do_oscar_sanity_check()
{
    sanityCheckTextWidget->clear();
    command_thread.init (DO_OSCAR_SANITY_CHECK, QStringList(""));
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
void XOSCAR_MainWindow::destroy()
{
    add_distro_widget.close();
    add_oscar_repo_widget.close();
    about_authors_widget.close();
    about_oscar_widget.close();
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
 * @author Geoffroy Vallee.
 *
 * This function handles the update of OSCAR cluster information when a new 
 * cluster is selected in the "General Information" widget. It displays the list
 * of partitions within the cluster.
 */
void XOSCAR_MainWindow::refresh_list_partitions ()
{
    if(listOscarClustersWidget->currentRow() == -1) {
        return;
    }

	// oscar does not (currently) support multiple clusters so the Perl
	// scripts have the cluster hard coded. This argument is ignored, but in
	// the future would be used to indicate which cluster we are requesting
	// partitions for.
    command_thread.init(DISPLAY_PARTITIONS, QStringList(listOscarClustersWidget->currentItem()->text()));
}

/**
 * @author Geoffroy Vallee.
 *
 * This function handles the update of partition information when a new 
 * partition is selected in the "General Information" widget.
 */
void XOSCAR_MainWindow::refresh_partition_info ()
{
    if(listClusterPartitionsWidget->currentRow() == -1) {
        return;
    }

    // do we really need to call selectedItems if the control
    // only allows one item to be selected at a time anyway?
    QList<QListWidgetItem *> list =
        listClusterPartitionsWidget->selectedItems();

    // if nothing is in the list, we cannot select the next one
    if (list.count() == 0) {
        return;
    }

    QListIterator<QListWidgetItem *> i(list);
    QString current_partition = i.next()->text();
    partitonNameEditWidget->setText(current_partition);

    /* We display the number of nodes composing the partition */
    command_thread.init(DISPLAY_PARTITION_NODES, 
                        QStringList(current_partition));
    command_thread.wait();

    /* We get the list of supported distros */
    command_thread.init(GET_SETUP_DISTROS, QStringList(""));
    command_thread.wait();

    /* We get the Linux distribution on which the partition is based */
    command_thread.init(DISPLAY_PARTITION_DISTRO, QStringList(""));
    command_thread.wait();
}

/**
 * @author Geoffroy Vallee.
 *
 * Slot that handles the click on the "add partition" button.
 *
 * @todo Find a good name by default that avoids conflicts if the user does not
 * change it.
 * @todo Check if a cluster is selected.
 */
void XOSCAR_MainWindow::add_partition_handler()
{
    if (listOscarClustersWidget->currentRow() == -1)
        return;

    listClusterPartitionsWidget->addItem ("New_Partition");
    listClusterPartitionsWidget->update ();
}

/**
 * @author Geoffroy Vallee.
 *
 * Slot that handles the click on the "Save Cluster Configuration" button.
 *
 * @todo Check if the partition name already exists or not.
 * @todo Display a dialog is partition information are not valid.
 * @todo Check the return value of the command to add partition information
 *       in the database.
 */
void XOSCAR_MainWindow::save_cluster_info_handler()
{
    int nb_nodes = PartitionNumberNodesSpinBox->value();
    QString partition_name = partitonNameEditWidget->text();
    QString partition_distro = partitionDistroComboBox->currentText();

    if (partition_name.compare("") == 0 || nb_nodes == 0 
        || partition_distro.compare("") == 0) {
        cerr << "ERROR: invalid partition information" << endl;
    } else {
        QStringList args;
        args << partition_name << partition_distro;

        /* We had now the compute nodes, giving them a default name */
        string tmp;
        for (int i=0; i < nb_nodes; i++) {
            tmp = partition_name.toStdString() + "_node" + intToStdString(i);
            args << tmp.c_str();
        }
        command_thread.init (ADD_PARTITION, args);
    }
    /* We unset the selection of the partition is order to be able to update
       the widget. If we do not do that, a NULL pointer is used and the app
       crashes. */
    listClusterPartitionsWidget->setCurrentRow(-1);
}

/**
 * @author Geoffroy Vallee.
 *
 * Utility function: convert an integer to a standard string.
 *
 * @param i Integer to convert in string.
 * @return Standard string representing the integer.
 */
string XOSCAR_MainWindow::intToStdString (int i)
{
    std::stringstream ss;
    std::string str;
    ss << i;
    ss >> str;
    return str;
}

/**
 * @author Geoffroy Vallee.
 *
 * Slot called when a tab from "Cluster Management" is activated. This function
 * then called specific functions for each tab.
 *
 * @param tab_num Index of the activated tab.
 */
void XOSCAR_MainWindow::tab_activated(int tab_num) 
{
    switch (tab_num) {
        case (1) : network_configuration_tab_activated();
    }
}

/**
 * @author Geoffroy Vallee.
 *
 * Slot called when the "Network Configuration" tab is activated.
 * This function grabs the name of the selected cluster and the selected 
 * partition and populate the list of nodes based on that. If no cluster and no
 * partition is selected in the "General Information" tab, we do nothing.
 *
 * @todo OSCAR cmds should be executed in a thread in order to ease the first
 * implementation of a remote manegement mechanism. For that, we have to
 * have an option to the OSCAR script in order to get the detailed info
 * about node of a given partition.
 * For instance, it can be: "oscar--display-partition-nodes partition1 -v"
 * The "-v" options makes that we get all detailed info for all the nodes.
 */
void XOSCAR_MainWindow::network_configuration_tab_activated() 
{
    if(listOscarClustersWidget->currentRow() == -1
        || listClusterPartitionsWidget->currentRow() == -1) {
        cout << "No specific partition selected, nothing to do\n" << endl;
        return;
    }
    QString partition_name = partitonNameEditWidget->text();
    cout << "Display nodes info of the partition: " 
         << partition_name.toStdString() << endl;

    // We clean up the list of nodes
    oscarNodesTreeWidget->clear();

    command_thread.init(DISPLAY_DETAILS_PARTITION_NODES, 
                        QStringList(partition_name));
}

/**
 * @author Geoffroy Vallee.
 *
 * Slot called when the user click the "browse" button when the user wants to
 * import MAC addresses from a file. This slot creates a XOSCAR_FileBrowser
 * widget.
 */
void XOSCAR_MainWindow::open_file()
{
    cout << "File selection" << endl;
    XOSCAR_FileBrowser *file_browser = new XOSCAR_FileBrowser("");
    connect(file_browser, SIGNAL(fileSelected(const QString)),
            this, SLOT(open_mac_file(const QString)));
    file_browser->show();
}

/**
 * @author Geoffroy Vallee.
 *
 * Slot handling the selection of a file for MAC addresses importation.
 *
 * @param file_path Path of the file from which MAC addresses have to be 
 *                  imported. Note that this path is provided by a signal from
 *                  XOSCAR_FileBrowser.
 */
void XOSCAR_MainWindow::open_mac_file(const QString file_path)
{
    cout << "We need to open: " << file_path.toStdString() << endl;
    importmacfile->setText(file_path);
}

/**
 * @author Geoffroy Vallee.
 *
 * This slot is called when the user clicks the "import MAC addresses from file"
 * button. The file path is supposed to be in the importmacfile widget (if empty
 * we do nothing).
 *
 * @todo Avoid MAC addresses duplication in the widget which grathers all 
 *       unassigned MAC addresses, when importing MAC addresses.
 * @todo Avoid MAC addresses duplication in the widget which grathers all 
 *       unassigned MAC addresses, when MAC addresses are already assigned to
 *       nodes.
 */
void XOSCAR_MainWindow::import_macs_from_file ()
{
    QString file_path = importmacfile->text();
    if (file_path.compare("") != 0) {
        cout << "Importing MACs from " << file_path.toStdString() << endl;
        QFile file (file_path);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            return;
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine();
            listNoneAssignedMacWidget->addItem (line);
            cout << "Line: " << line.toStdString() << endl;
        }
    }
}

int XOSCAR_MainWindow::handle_thread_result (int command_id, 
    const QString result)
{
    QStringList list;
    cout << "MainWindow: result from cmd exec thread received: "
         << command_id
         << endl;
    if (command_id == GET_LIST_DEFAULT_REPOS) {
        // We parse the result: one URL per line.
        if (listReposWidget == NULL) {
            cerr << "ERROR: Impossible to update the widget that gives "
                    << "the list of repos, it seems the widget does not exist"
                    << endl;
            return -1;
        }
        list = result.split("\n");
        for (int i = 0; i < list.size(); ++i){
            listReposWidget->addItem (list.at(i));
        }
        listReposWidget->update();
    } else if (command_id == DISPLAY_PARTITIONS) {
        // We parse the result: one partition name per line.
        // skip empty strings? otherwise we have extra partitions added
        // could also check result for empty string
        list = result.split("\n", QString::SkipEmptyParts);
        listClusterPartitionsWidget->clear();
        for (int i = 0; i < list.size(); ++i){
            listClusterPartitionsWidget->addItem (list.at(i));
        }
        listClusterPartitionsWidget->update();
    } else if (command_id == DISPLAY_PARTITION_NODES) {
        list = result.split(" ");
        PartitionNumberNodesSpinBox->setMinimum(list.size());
        PartitionNumberNodesSpinBox->setValue(list.size());
    } else if (command_id == DISPLAY_PARTITION_DISTRO) {
        cerr << "ERROR: Not yet implemented" << endl;
/*        int index = partitionDistroComboBox->findText(distro_name);
        partitionDistroComboBox->setCurrentIndex(index);*/
    } else if (command_id == DISPLAY_DETAILS_PARTITION_NODES) {
        string res = result.toStdString();
        // Now we have a big string with the config and we need to parse it.
        stringToNodesConfig (result);
    } else if (command_id == SETUP_DISTRO) {
        // We could here try to see if the command was successfully executed or
        // not. Otherwise, nothing to do here.
    } else if (command_id == GET_SETUP_DISTROS) {
        command_thread.init (GET_LIST_DEFAULT_REPOS, QStringList (""));
    }
    return 0;
}

int XOSCAR_MainWindow::stringToNodesConfig (QString cmd_result)
{
    QStringList lines = cmd_result.split ("\n");
    for (int i=0; i < lines.size(); i++) {
        QString line = lines.at(i);
        QTreeWidgetItem *item;
        if (line.size() > 0 && line.at(0) != QChar('\t')) {
            cout << "New node configuration: " << line.toStdString() << endl;
            oscarNodesTreeWidget->setColumnCount(1);
            item = new QTreeWidgetItem(oscarNodesTreeWidget);
            item->setText(0, line);
            oscarNodesTreeWidget->expandItem (item);
        } else if (line.size() > 0 && line.at(0) == QChar('\t')) {
            cout << "Analyzing line: " << line.toStdString() << endl;
            QStringList nodeInfo = line.split (": ");
            if (nodeInfo.at(0) == QString("\tMAC")) {
                oscarNodesTreeWidget->expandItem (item);
                QTreeWidgetItem *subitem_mac = new QTreeWidgetItem(item);
                QString mac = "MAC @: " + nodeInfo.at(1);
                subitem_mac->setText(0, mac);
            } else if (nodeInfo.at(0) == QString("\tIP")) {
                oscarNodesTreeWidget->expandItem (item);
                QTreeWidgetItem *subitem_mac = new QTreeWidgetItem(item);
                QString mac = "IP @: " + nodeInfo.at(1);
                subitem_mac->setText(0, mac);
            }
        }
    }
    oscarNodesTreeWidget->update();
    return 0;
}
