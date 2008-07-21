/*
 *  Copyright (c) 2007-2008 Oak Ridge National Laboratory, 
 *                          Geoffroy Vallee <valleegr@ornl.gov>
 *                          All rights reserved
 *  This file is part of the xoscar software, part of the OSCAR software.
 *  For license information, see the COPYING file in the top level directory
 *  of the OSCAR source.
 */

/**
 * @file XOSCAR_TabSoftwareConfiguration.cpp
 * @brief Actual implementation of the XOSCAR_TabSoftwareConfiguration class.
 * @author Robert Babilon
 */

#include "XOSCAR_TabSoftwareConfiguration.h"

#include <iostream>

using namespace std;
using namespace xoscar;

XOSCAR_TabSoftwareConfiguration::XOSCAR_TabSoftwareConfiguration(QWidget* parent)
    : QWidget(parent)
{
    setupUi(this);

    connect(&command_thread, SIGNAL(thread_terminated(CommandTask::CommandTasks, QString)),
        this, SLOT(handle_thread_result (CommandTask::CommandTasks, QString)));

    connect(&command_thread, SIGNAL(finished()),
            this, SLOT(command_thread_finished()));
}

XOSCAR_TabSoftwareConfiguration::~XOSCAR_TabSoftwareConfiguration()
{
}

/**
 *  @author Robert Babilon
 *
 *  Slot called when a new cluster on the general information tab is selected.
 *  This slot is connected via the main window to maintain independence between
 *  the tabs.
 *
 *  @param name The name of the newly selected cluster.
 */
void XOSCAR_TabSoftwareConfiguration::cluster_selection_changed(QString name)
{
    cluster_name = name;

    lineEdit_5->setText(cluster_name);
}

/**
 *  @author Robert Babilon
 *
 *  Slot called when a new partition on the general information tab is selected.
 *  This slot is connected via the main window to maintain independence between
 *  the tabs.
 *
 *  @param name The name of the newly selected partition.
 */
void XOSCAR_TabSoftwareConfiguration::partition_selection_changed(QString name)
{
    partition_name = name;

    lineEdit_4->setText(partition_name);
}

/**
 *  @author Robert Babilon
 *
 *  Slot called when the software configuration tab is selected and needs to
 *  update its information.
 */
void XOSCAR_TabSoftwareConfiguration::software_configuration_tab_activated()
{
    if(partition_name.isEmpty() || cluster_name.isEmpty()) {
        cout << "ERROR: no cluster and/or no partition selected" << endl;
        opkgsListWidget->clear();
        return;
    }

    command_thread.init(CommandTask::DISPLAY_DEFAULT_OPKGS,
                        QStringList(partition_name));
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
 */
int XOSCAR_TabSoftwareConfiguration::handle_thread_result (CommandTask::CommandTasks command_id, 
    const QString result)
{
    QStringList list;
    cout << "SoftwareConfiguration: result from cmd exec thread received: "
         << command_id
         << endl;

    if (command_id == CommandTask::DISPLAY_DEFAULT_OPKGS) {
        opkgsListWidget->clear();

        QStringList pkgs = result.split("\n", QString::SkipEmptyParts);
        for(int i = 0; i < pkgs.count(); i++) {
            // add to list
            QListWidgetItem *item = new QListWidgetItem(pkgs.at(i));
            item->setFlags(item->flags()|Qt::ItemIsUserCheckable);
            item->setCheckState(Qt::Unchecked);
            opkgsListWidget->addItem(item);
        }
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
void XOSCAR_TabSoftwareConfiguration::command_thread_finished()
{
    if(!command_thread.isEmpty()) { 
        command_thread.start();
    }
}
