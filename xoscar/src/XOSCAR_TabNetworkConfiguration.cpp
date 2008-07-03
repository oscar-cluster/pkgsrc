/*
 *  Copyright (c) 2007-2008 Oak Ridge National Laboratory, 
 *                          Geoffroy Vallee <valleegr@ornl.gov>
 *                          All rights reserved
 *  This file is part of the xoscar software, part of the OSCAR software.
 *  For license information, see the COPYING file in the top level directory
 *  of the OSCAR source.
 */

/**
 * @file XOSCAR_TabNetworkConfiguration.cpp
 * @brief Actual implementation of the XOSCAR_TabNetworkConfiguration class.
 * @author Robert Babilon
 */

#include "XOSCAR_TabNetworkConfiguration.h"

#include <QDir>

using namespace xoscar;

XOSCAR_TabNetworkConfiguration::XOSCAR_TabNetworkConfiguration(QWidget* parent)
    : QWidget(parent)
{
    setupUi(this);

    connect(importfilebrowse, SIGNAL(clicked()),
                    this, SLOT(open_file()));
    connect(importmacs, SIGNAL(clicked()),
                    this, SLOT(import_macs_from_file()));

    connect(assignmac, SIGNAL(clicked()),
            this, SLOT(assignmac_clicked_handler()));
    connect(unassignmac, SIGNAL(clicked()),
            this, SLOT(unassignmac_clicked_handler()));
    connect(assignallmacs, SIGNAL(clicked()),
            this, SLOT(assignallmacs_clicked_handler()));

    connect(clearmacs, SIGNAL(clicked()),
            listNoneAssignedMacWidget, SLOT(clear()));
}

XOSCAR_TabNetworkConfiguration::~XOSCAR_TabNetworkConfiguration()
{
}

/**
 * @author Geoffroy Vallee.
 *
 * Slot called when the user click the "browse" button when the user wants to
 * import MAC addresses from a file. This slot creates a XOSCAR_FileBrowser
 * widget.
 */
void XOSCAR_TabNetworkConfiguration::open_file()
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
void XOSCAR_TabNetworkConfiguration::open_mac_file(const QString file_path)
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
void XOSCAR_TabNetworkConfiguration::import_macs_from_file ()
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
            // This part avoids duplicate MAC addresses in the unassigned list widget
            if(listNoneAssignedMacWidget->findItems(line, Qt::MatchFixedString).count() == 0) {
                listNoneAssignedMacWidget->addItem (line);
                cout << "Line: " << line.toStdString() << endl;
            }
        }
    }
}

/**
 *  @author Robert Babilon
 *
 *  Slot called when the assignmac button is clicked.
 */
void XOSCAR_TabNetworkConfiguration::assignmac_clicked_handler()
{
    if(listNoneAssignedMacWidget->currentItem() == NULL || 
        oscarNodesTreeWidget->currentItem() == NULL) {
        cout << "ERROR: cannot assign mac address: nothing selected" << endl;
        return;
    }

    cout << "assigning mac address" << endl;
}

/**
 *  @author Robert Babilon
 *
 *  Slot called when the unassignmac button is clicked.
 */
void XOSCAR_TabNetworkConfiguration::unassignmac_clicked_handler()
{
    if(oscarNodesTreeWidget->currentItem() == NULL) {
        cout << "ERROR: cannot unassign mac address: nothing selected in oscar nodes" << endl;
        return;
    }

    cout << "unassigning mac address" << endl;
}

/**
 *  @author Robert Babilon
 *
 *  Slot called when the assignallmacs button is clicked.
 */
void XOSCAR_TabNetworkConfiguration::assignallmacs_clicked_handler()
{
    cout << "auto assigning mac addresses" << endl;
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
void XOSCAR_TabNetworkConfiguration::network_configuration_tab_activated() 
{
    /*if(listOscarClustersWidget->currentRow() == -1
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
                        QStringList(partition_name));*/
}

/**
 *  @author Robert Babilon
 *
 *  Slot called when the command thread has finished executing a command.
 *
 *  @param command_id The command that has completed. The list of values
 *  are in CommandExecutionThread.h.
 *
 *  @param result Holds the return value of the command.
 *
 */
int XOSCAR_TabNetworkConfiguration::handle_thread_result (int command_id, 
    const QString result)
{
    QStringList list;
    cout << "MainWindow: result from cmd exec thread received: "
         << command_id
         << endl;

    if (command_id == DISPLAY_DETAILS_PARTITION_NODES) {
        string res = result.toStdString();
        // Now we have a big string with the config and we need to parse it.
        stringToNodesConfig (result);
    }
}

int XOSCAR_TabNetworkConfiguration::stringToNodesConfig (QString cmd_result)
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
