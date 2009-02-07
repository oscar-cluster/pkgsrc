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
#include <QRegExp>
#include <QRegExpValidator>

using namespace xoscar;

XOSCAR_TabNetworkConfiguration::XOSCAR_TabNetworkConfiguration(ThreadHandlerInterface* handler, QWidget* parent)
    : QWidget(parent)
    , ThreadUserInterface(handler)
    , file_browser(NULL)
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

    connect(importmanualmac, SIGNAL(clicked()),
            this, SLOT(importmanualmac_clicked_handler()));
    connect(manualmac, SIGNAL(returnPressed()),
            importmanualmac, SIGNAL(clicked()));
    connect(importmacfile, SIGNAL(returnPressed()),
            importmacs, SIGNAL(clicked()));
    connect(clearmacs, SIGNAL(clicked()),
            listNoneAssignedMacWidget, SLOT(clear()));

    // *** Test Code ***
    stringToNodesConfig(tr("New_Partition_0\n\tMAC: \n\tIP: 172.20.0.2\n") +
                        tr("New_Partition_1\n\tMAC: 00:aa:bb:cc:dd:02\n\tIP: 172.20.0.3\n") +
                        tr("New_Partition_2\n\tMAC: 00:aa:bb:cc:dd:03\n\tIP: 172.20.0.4\n"));
    // *** End Test Code ***
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
    if(file_browser == NULL) {
        file_browser = new XOSCAR_FileBrowser("", this);
        connect(file_browser, SIGNAL(fileSelected(const QString)),
                this, SLOT(open_mac_file(const QString)));
    }
    file_browser->exec();
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
            QString line = in.readLine().trimmed();
            if(line.isEmpty()) return;
            // This part avoids duplicate MAC addresses in the unassigned list widget
            if(isValidMacAddress(line) && isMacUnassigned(line) == false && isMacAssigned(line) == false) {
                listNoneAssignedMacWidget->addItem(line);
                cout << "Line: " << line.toStdString() << endl;
            }
        }
    }
}

/**
 *  @author Robert Babilon
 *  
 *  Function to check if a given MAC address is valid.
 *
 *  @param mac The MAC address to validate
 *  @return true if the MAC address is valid; otherwise false.
 */
bool XOSCAR_TabNetworkConfiguration::isValidMacAddress(QString mac)
{
    QRegExp regex("^[0-9A-Fa-f]{2}:[0-9A-Fa-f]{2}:[0-9A-Fa-f]{2}:[0-9A-Fa-f]{2}:[0-9A-Fa-f]{2}:[0-9A-Fa-f]{2}$");
    QRegExpValidator macvalidator(regex, this);

    int pos = 0;
    return macvalidator.validate(mac, pos) == QValidator::Acceptable ? true : false;
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
        cerr << "ERROR: cannot assign mac address: nothing selected" << endl;
        return;
    }

    cout << "assigning mac address" << endl;

    QTreeWidgetItem* nodeItem = oscarNodesTreeWidget->currentItem();
    QString macAddress = listNoneAssignedMacWidget->currentItem()->text();

    if(assignMacAddress(nodeItem, macAddress)) {
        delete listNoneAssignedMacWidget->takeItem(listNoneAssignedMacWidget->currentRow());
    }
}

/**
 *  @author Robert Babilon
 *
 *  Slot called when the unassignmac button is clicked.
 */
void XOSCAR_TabNetworkConfiguration::unassignmac_clicked_handler()
{
    if(oscarNodesTreeWidget->currentItem() == NULL) {
        cerr << "ERROR: cannot unassign mac address: nothing selected in oscar nodes" << endl;
        return;
    }

    cout << "unassigning mac address" << endl;

    QString unassignedMac;
    if(unassignMacAddress(oscarNodesTreeWidget->currentItem(), unassignedMac)) {
        listNoneAssignedMacWidget->addItem(unassignedMac);
    }
}

/**
 *  @author Robert Babilon
 *
 *  Slot called when the assignallmacs button is clicked.
 */
void XOSCAR_TabNetworkConfiguration::assignallmacs_clicked_handler()
{
    cout << "auto assigning mac addresses" << endl;

    int topLevelCount = oscarNodesTreeWidget->topLevelItemCount();
    // loop through the top level items
    for(int i = 0; i < topLevelCount; i++) {
        QTreeWidgetItem *item = oscarNodesTreeWidget->topLevelItem(i);
        int childCount = item->childCount();
        // loop through the child nodes for each top level item
        for(int j = 0; j < childCount; j++) {
            QString assignedMac;
            // check if child node is a MAC address node
            if(isItemMacAddress(item->child(j), assignedMac)) {
                // check if no MAC address is already assigned
                if(assignedMac.isEmpty()) {
                    if(listNoneAssignedMacWidget->count() == 0) {
                        // we ran out of available MAC addresses
                        return;
                    }
                    QString mac = listNoneAssignedMacWidget->item(0)->text();
                    // assign the MAC address to the node
                    if(assignMacAddress(item->child(j), mac)) {
                        delete listNoneAssignedMacWidget->takeItem(0);
                    }
                }
                // break from inner loop (over child nodes) since only one child
                // can be a MAC address node
                break;
            }
        }
    }
}

/**
 *  @author Robert Babilon
 *
 *  Slot called when the importmanualmac button is clicked.
 */
void XOSCAR_TabNetworkConfiguration::importmanualmac_clicked_handler()
{
    QString mac = manualmac->text();

    if(isValidMacAddress(mac) && isMacUnassigned(mac) == false && isMacAssigned(mac) == false) {
        listNoneAssignedMacWidget->addItem(mac);
    }
}

/**
 *  @author Robert Babilon
 *
 *  Function checks if the given MAC address is in the unassigned pool.
 *
 *  @param mac The MAC address to lookup in the pool of unassigned MAC addresses
 *  @return true if the MAC address is in the unassigned pool; otherwise false.
 */
bool XOSCAR_TabNetworkConfiguration::isMacUnassigned(QString& mac)
{
    return listNoneAssignedMacWidget->findItems(mac, Qt::MatchFixedString).count() == 0 ? false : true;
}

/**
 *  @author Robert Babilon
 *
 *  Function checks if the given MAC address is assigned to one of the nodes in
 *  the oscar nodes list widget.
 *
 *  @param mac The MAC address to lookup in the list of oscar nodes.
 *  @return true if the MAC address is assigned to one of the oscar nodes;
 *  otherwise false.
 */
bool XOSCAR_TabNetworkConfiguration::isMacAssigned(QString& mac)
{
    // get top level items, check the child items that have MAC in them?
    int topLevelCount = oscarNodesTreeWidget->topLevelItemCount();

    for(int i = 0; i < topLevelCount; i++) {
        QTreeWidgetItem *item = oscarNodesTreeWidget->topLevelItem(i);

        int childCount = item->childCount();
        for(int j = 0; j < childCount; j++) {
            QString assignedMac;
            if(isItemMacAddress(item->child(j), assignedMac)) {
                if(mac == assignedMac) {
                    return true;
                }
                break;
            }
        }
    }

    return false;
}

/**
 *  @author Robert Babilon
 *
 *  Function to assign a given MAC address to a given node.
 *
 *  @param item The QTreeWidgetItem to assign the MAC address to.
 *  @param mac The MAC address to be assigned to the item.
 *  @return true if the MAC address was assigned to the node; otherwise false.
 */
bool XOSCAR_TabNetworkConfiguration::assignMacAddress(QTreeWidgetItem* item, QString& mac)
{
    if(item == NULL) { 
        return false;
    }

    QString oldMac;
    if(isItemMacAddress(item, oldMac)) {
        if(oldMac.isEmpty()) {
            item->setText(0, item->text(0) + mac);
            return true;
        }
    }

    return false;
}

/**
 *  @author Robert Babilon
 *
 *  Function to unassign a MAC address from a given node.
 *
 *  @param item The QTreeWidgetItem to remove the MAC address from.
 *  @param [out] mac The MAC address that has been removed. 
 *  @return true if the MAC address was unassigned from the node; otherwise
 *  false.
 */
bool XOSCAR_TabNetworkConfiguration::unassignMacAddress(QTreeWidgetItem* item, QString& mac)
{
    if(item == NULL) {
        return false;
    }

    if(isItemMacAddress(item, mac)) {
        if(mac.isEmpty()) {
            return false;
        }

        item->setText(0, macChildNodeName + oscarChildNodeSplitter);
        return true;
    }

    return false;
}

/**
 *  @author Robert Babilon
 *
 *  @param item The QTreeWidgetItem to check if it is holding a MAC address
 *  @param [out] mac The MAC address the item is holding. If the item is not holding a
 *  MAC address, this param holds the empty string. Otherwise it holds the MAC
 *  address. 
 *  @return true if item is holding a MAC address; otherwise false.
 */
bool XOSCAR_TabNetworkConfiguration::isItemMacAddress(QTreeWidgetItem* item, QString& mac)
{
    mac = tr("");

    QString text = item->text(0);
    if(text.indexOf(oscarChildNodeSplitter) == -1) {
        return false;
    }

    QStringList list = text.split(oscarChildNodeSplitter, QString::SkipEmptyParts);
    if(list.count() == 0) {
        return false;
    }

    if(list.first() != macChildNodeName) {
        return false;
    }
    else {
        if(list.count() == 2) {
            mac = list.last();
        }
        return true;
    }
}

/**
 *  @author Robert Babilon
 *
 *  Slot called when the selected partition has changed.
 *
 *  @param name The name of the partition that has been selected.
 */
void XOSCAR_TabNetworkConfiguration::partition_selection_changed(QString name)
{
    partition_name = name;
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
    if(partition_name.isEmpty()) {
        return;
    }

    // We clean up the list of nodes
    oscarNodesTreeWidget->clear();

    threadHandler->enqueue_command_task(CommandTask(xoscar::DISPLAY_DETAILS_PARTITION_NODES, 
                        QStringList(partition_name), dynamic_cast<ThreadUserInterface*>(this)));
}

/**
 *  @author Robert Babilon
 *
 *  Slot called when the command thread has finished executing a command.
 *  Calls CommandExecutionThread::wakeThread() before returning to ensure the
 *  thread exits CommandExecutionThread::run().
 *
 *  @param command_id The command that has completed. The list of values
 *  are in CommandTask.h.
 *
 *  @param result Holds the return value of the command.
 *
 */
int XOSCAR_TabNetworkConfiguration::handle_thread_result (xoscar::CommandId command_id, 
    const QString result)
{
    QStringList list;
    cout << "NetworkConfiguration: result from cmd exec thread received: "
         << command_id
         << endl;

    if (command_id == xoscar::DISPLAY_DETAILS_PARTITION_NODES) {
        string res = result.toStdString();
        // Now we have a big string with the config and we need to parse it.
        stringToNodesConfig (result);
    }
    return 0;
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
                QString mac = macChildNodeName + oscarChildNodeSplitter + nodeInfo.at(1);
                subitem_mac->setText(0, mac);
            } else if (nodeInfo.at(0) == QString("\tIP")) {
                oscarNodesTreeWidget->expandItem (item);
                QTreeWidgetItem *subitem_mac = new QTreeWidgetItem(item);
                QString mac = ipChildNodeName + oscarChildNodeSplitter + nodeInfo.at(1);
                subitem_mac->setText(0, mac);
            }
        }
    }
    oscarNodesTreeWidget->update();
    return 0;
}
