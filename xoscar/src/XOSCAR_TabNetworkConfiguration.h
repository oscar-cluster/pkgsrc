/*
 *  Copyright (c) 2007-2008 Oak Ridge National Laboratory, 
 *                          Geoffroy Vallee <valleegr@ornl.gov>
 *                          All rights reserved
 *  This file is part of the xoscar software, part of the OSCAR software.
 *  For license information, see the COPYING file in the top level directory
 *  of the OSCAR source.
 */

/**
 * @file XOSCAR_TabNetworkConfiguration.h
 * @brief Defines the class XOSCAR_TabNetworkConfiguration for the network
 * configuration tab.
 * @author Robert Babilon
 */
#ifndef XOSCAR_TABNETWORKCONFIGURATION_H
#define XOSCAR_TABNETWORKCONFIGURATION_H

#include <QTextStream>

#include <iostream>

#include "XOSCAR_TabWidgetInterface.h"
#include "ui_xoscar_networkconfiguration.h"
#include "CommandExecutionThread.h"
#include "XOSCAR_FileBrowser.h"

using namespace std;
using namespace Ui;

namespace xoscar {

const QString oscarChildNodeSplitter = QString(" @: ");
const QString macChildNodeName = QString("MAC");
const QString ipChildNodeName = QString("IP");

class XOSCAR_TabNetworkConfiguration : public QWidget, public NetworkConfigurationForm
{
Q_OBJECT

public:
    XOSCAR_TabNetworkConfiguration(QWidget* parent=0);
    ~XOSCAR_TabNetworkConfiguration();

public slots:
    int stringToNodesConfig(QString);
    void open_mac_file(const QString);
    void import_macs_from_file();
    void partition_name_changed(QString);
    void network_configuration_tab_activated();
     int handle_thread_result (int command_id, const QString result);
    void open_file();
    void assignmac_clicked_handler();
    void unassignmac_clicked_handler();
    void assignallmacs_clicked_handler();
    void importmanualmac_clicked_handler();

protected:
    bool isMacUnassigned(QString & mac);
    bool isMacAssigned(QString & mac);
    bool isItemMacAddress(QTreeWidgetItem* item, QString& mac);
    bool assignMacAddress(QTreeWidgetItem*, QString&);
    bool unassignMacAddress(QTreeWidgetItem*, QString&);
    bool isValidMacAddress(QString mac);

private:
   CommandExecutionThread command_thread;
   QString partition_name;
};

}
#endif //XOSCAR_TABNETWORKCONFIGURATION_H
