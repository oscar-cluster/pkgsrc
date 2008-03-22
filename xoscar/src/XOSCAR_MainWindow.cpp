/*
 *  Copyright (c) 2007 Oak Ridge National Laboratory, 
 *                     Geoffroy Vallee <valleegr@ornl.gov>
 *                     All rights reserved
 *  This file is part of the xorm software, part of the OSCAR software.
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
    QString home_path = getenv("HOME");
    QDir dir (home_path);
    if ( !dir.exists() ) {
        cout << "ERROR: Impossible to find the home directory" << endl;
        return;
    }
    home_path = home_path + "/.xoscar.conf";
    SimpleConfigFile confFile = SimpleConfigFile (home_path.toStdString());

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
    command_thread.init ("", GET_SETUP_DISTROS);
    command_thread.run();


    /* Add the default OSCAR repositories */
    cout << "Get list of of default repos" << endl;
    command_thread.init("", GET_LIST_DEFAULT_REPOS);
    command_thread.run();
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
 */
void XOSCAR_MainWindow::kill_popup(QString list_repos, QString list_opkgs)
{
    /* We update the list of available OPKGs for the selected OSCAR repo */
    string str = list_opkgs.toStdString();
    if (str.compare ("") != 0) {
        listOPKGsWidget->clear();
        vector<string> opkgs;
        Tokenize(str, opkgs, " ");
        vector<string>::iterator item;
        for(item = opkgs.begin(); item != opkgs.end(); item++) {
            string strD = *(item);
            this->listOPKGsWidget->addItem (strD.c_str());
        }
        listOPKGsWidget->update();
    }

    /* We update the list of available OSCAR repos */
    string str2 = list_repos.toStdString ();
    if (str2.compare ("") != 0) {
        listReposWidget->clear();
        vector<string> repos;
        Tokenize(str2, repos, " ");
        vector<string>::iterator item;
        for(item = repos.begin(); item != repos.end(); item++) {
            string strD = *(item);
            this->listReposWidget->addItem (strD.c_str());
        }
        listReposWidget->update();
    }

    /* We close the popup window that asks the user to wait */
    wait_popup->close();
}

/**
 * Equilvalent to the slip function in Perl: slip a string up, based on a 
 * delimiter which is a space by default.
 *
 * @param str String to slip up.
 * @param tokens Vector of string used to store the slit string.
 * @param delimiters Character used to split the string up. By default a space.
 * @todo Avoid the code duplication with the ORMAddRepoDialog class.
 */
void XOSCAR_MainWindow::Tokenize(const string& str,
                      vector<string>& tokens,
                      const string& delimiters = " ")
{
    // Skip delimiters at beginning.
    string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
    string::size_type pos     = str.find_first_of(delimiters, lastPos);

    while (string::npos != pos || string::npos != lastPos)
    {
        // Found a token, add it to the vector.
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiters, pos);
        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
    }
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
    repo = i.next()->text();
    wait_popup = new ORMWaitDialog(0, repo);
    wait_popup->show();
    update();

    command_thread.init(repo, GET_LIST_OPKGS);
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

    command_thread.init(repo_url, GET_LIST_REPO);
    command_thread.init(repo_url, GET_LIST_OPKGS);
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
    /* We update the list of setup Linux distros */
    string str = list_distros.toStdString();
    if (str.compare ("") != 0) {
        listSetupDistrosWidget->clear();
        vector<string> distros;
        Tokenize(str, distros, " ");
        vector<string>::iterator item;
        for(item = distros.begin(); item != distros.end(); item++) {
            string strD = *(item);
            this->listSetupDistrosWidget->addItem (strD.c_str());
        }
        listSetupDistrosWidget->update();
    }
    command_thread.init("", INACTIVE);
}

void XOSCAR_MainWindow::refresh_list_setup_distros()
{
    listSetupDistrosWidget->clear();
    command_thread.init ("", GET_SETUP_DISTROS);
    command_thread.run();
}

void XOSCAR_MainWindow::do_system_sanity_check()
{
    sanityCheckTextWidget->clear();
    command_thread.init ("", DO_SYSTEM_SANITY_CHECK);
    command_thread.run();
}

void XOSCAR_MainWindow::do_oscar_sanity_check()
{
    sanityCheckTextWidget->clear();
    command_thread.init ("", DO_OSCAR_SANITY_CHECK);
    command_thread.run();
}

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

    command_thread.init("", DISPLAY_PARTITIONS);
    command_thread.run();
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

    QList<QListWidgetItem *> list =
        listClusterPartitionsWidget->selectedItems();
    QListIterator<QListWidgetItem *> i(list);
    QString current_partition = i.next()->text();
    partitonNameEditWidget->setText(current_partition);

    /* We display the number of nodes composing the partition */
        /** @TODO this should be executed in a thread in order to ease the first
       implementation of a remote manegement mechanism. */
    char *ohome = getenv ("OSCAR_HOME");
    const string cmd = (string) ohome 
                        + "/scripts/oscar --display-partition-nodes "
                        + current_partition.toStdString();
    pstream command(cmd, pstreambuf::pstdout);
    std::string s, tmp;
    while (std::getline(command, tmp)) {
        s += tmp;
    }
    int n = 0;
    if (s.compare ("") != 0) {
        vector<string> nodes;
        Tokenize(s, nodes, " ");
        vector<string>::iterator v_item;
        for(v_item = nodes.begin(); v_item != nodes.end(); v_item++) {
            n++;
        }
    }
    PartitionNumberNodesSpinBox->setMinimum(n);
    PartitionNumberNodesSpinBox->setValue(n);

    /* We get the list of supported distros */
    /** @TODO this should be executed in a thread in order to ease the first
       implementation of a remote manegement mechanism. */
    const string cmd2 = (string) ohome 
        + "/scripts/oscar-config --list-setup-distros";
    ipstream proc2(cmd2);
    string buf, tmp_list;
    while (proc2 >> buf) {
        tmp_list += buf;
        tmp_list += " ";
    }
    if (tmp_list.compare ("") != 0) {
        vector<string> distros;
        Tokenize(tmp_list, distros, " ");
        vector<string>::iterator v_item;
        for(v_item = distros.begin(); v_item != distros.end(); v_item++) {
            string strD = *(v_item);
            partitionDistroComboBox->addItem (strD.c_str());
        }
    }

    /* We get the Linux distribution on which the partition is based */
    /** @TODO this should be executed in a thread in order to ease the first
       implementation of a remote manegement mechanism. */
    const string cmd3 = (string) ohome 
            + "/scripts/oscar --display-partition-distro "
            + current_partition.toStdString();
    pstream command3(cmd3, pstreambuf::pstdout);
    std::string s2;
    while (std::getline(command3, tmp)) {
        s2 += tmp;
    }
    QString distro_name = s2.c_str();
    int index = partitionDistroComboBox->findText(distro_name);
    partitionDistroComboBox->setCurrentIndex(index);
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
        char *ohome = getenv ("OSCAR_HOME");
        string cmd = (string) ohome 
                        + "/scripts/oscar"
                        + " --add-partition " + partition_name.toStdString()
                        + " --cluster oscar"
                        + " --distro " + partition_distro.toStdString();
        /* We had now the compute nodes, giving them a default name */
        for (int i=0; i < nb_nodes; i++) {
            cmd += " --client ";
            cmd += partition_name.toStdString() + "_node" + intToStdString(i);
        }
        if (system (cmd.c_str())) {
            cerr << "ERROR executing: " << cmd << endl;
        }
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
 * @param tab_num Index of the activated tab.
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

    // First we get the list of node names
    /** @TODO this should be executed in a thread in order to ease the first
       implementation of a remote manegement mechanism. */
    char *ohome = getenv ("OSCAR_HOME");
    const string cmd = (string) ohome 
                        + "/scripts/oscar --display-partition-nodes "
                        + partition_name.toStdString();
    ipstream proc (cmd);
    string buf;
    while (proc >> buf) {
        QString nodeName = buf.c_str();
        oscarNodesTreeWidget->setColumnCount(1);
        QTreeWidgetItem *item = new QTreeWidgetItem(oscarNodesTreeWidget);

        // Then we display node information if available
    /** @TODO this should be executed in a thread in order to ease the first
       implementation of a remote manegement mechanism. */
        const string cmd2 = (string) ohome 
                            + "/scripts/oscar --display-node-info "
                            + buf
                            + " --partition "
                            + partition_name.toStdString();
        ipstream proc2 (cmd2);
        string buf2;
        // This variable is used to know which word we are actually getting
        // at a given time during output analysis. In other terms, we use
        // that to skip stuff we are not interesting in (we exactly know the
        // output format).
        int i=0;
        QString hostname;
        QString mac;
        QString ip;
        while (proc2 >> buf2) {
            if (i == 3) {
                hostname = buf2.c_str();
            }
            if (i == 7) {
                ip = buf2.c_str();
            }
            if (i == 9) {
                mac = buf2.c_str();
            }
            i++;
        }

        if (hostname.compare ("") != 0) {
            item->setText(0, hostname);
        } else {
            item->setText(0, nodeName);
        }
        oscarNodesTreeWidget->expandItem (item);
        QTreeWidgetItem *subitem_mac = new QTreeWidgetItem(item);
        subitem_mac->setText(0, "MAC @: " + mac);

        QTreeWidgetItem *subitem_ip = new QTreeWidgetItem(item);
        subitem_ip->setText(0, "IP @: " + ip);
    }

    oscarNodesTreeWidget->update();
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

int XOSCAR_MainWindow::handle_thread_result (int command_id, QString result)
{
    QStringList list;
    if (command_id == GET_LIST_DEFAULT_REPOS) {
        // We parse the result: one URL per line.
        list = result.split("\n");
        for (int i = 0; i < list.size(); ++i){
            listReposWidget->addItem (list.at(i));
        }
        listReposWidget->update();
    } else if (command_id == DISPLAY_PARTITIONS) {
        // We parse the result: one partition name per line.
        list = result.split("\n");
        listClusterPartitionsWidget->clear();
        for (int i = 0; i < list.size(); ++i){
            listClusterPartitionsWidget->addItem (list.at(i));
        }
        listClusterPartitionsWidget->update();
    } else {
        cerr << "ERROR: Unsupported command id: " << command_id << endl;
        return -1;
    }
    return 0;
}
