/*
 *  Copyright (c) 2007-2008 Oak Ridge National Laboratory, 
 *                          Geoffroy Vallee <valleegr@ornl.gov>
 *                          All rights reserved
 *  This file is part of the xoscar software, part of the OSCAR software.
 *  For license information, see the COPYING file in the top level directory
 *  of the OSCAR source.
 */

/**
 * @file XOSCAR_TabGeneralInformation.h
 * @brief Defines the class XOSCAR_TabGeneralInformation for the general
 * information tab.
 * @author Robert Babilon
 */

#ifndef XOSCAR_TABGENERALINFORMATION_H
#define XOSCAR_TABGENERALINFORMATION_H

#include "XOSCAR_TabWidgetInterface.h"
#include "ui_xoscar_generalinformation.h"
#include "CommandExecutionThread.h"

using namespace Ui;

namespace xoscar {

/** 
 * States for the partition list widget items. The states indicate the
 * modification status: saved (the item was loaded from oscar), modified (the
 * user changed a property on a saved item), created (added since last save)
 * The order of these states is significant. Changing the order or adding new
 * states may affect XOSCAR_TabGeneralInformation::setPartitionItemState().
 * */
enum PartitionState { Saved, Modified, Created };

class XOSCAR_TabGeneralInformation : public QWidget, public GeneralInformationForm, public XOSCAR_TabWidgetInterface
{
Q_OBJECT

public:
    XOSCAR_TabGeneralInformation(QWidget* parent=0);
    ~XOSCAR_TabGeneralInformation();

public slots:
    void partitionName_textEdited_handler(const QString&);
    void partitionDistro_currentIndexChanged_handler(int);
	void partitionNodes_valueChanged_handler(int);
	void add_partition_handler();
    void remove_partition_handler();
    SaveResult save_cluster_info_handler();
	void refresh_list_partitions();
    void refresh_partition_info();
    void setDefaultPartitionValues();
    void enablePartitionInfoWidgets(bool enable);
     int handle_thread_result (CommandTask::CommandTasks command_id, const QString result);
    void handle_oscar_config_result(QString list_distros);
    SaveResult save();
    SaveResult undo();
    void partition_list_rowChanged_handler(int);
    void clusters_list_rowChanged_handler(int);
    void virtualMachinesCheckBox_stateChanged_handler(int state);
    void virtualMachinesComboBox_currentIndexChanged_handler(int index);
    void command_thread_finished();

signals:
    void widgetContentsModified(QWidget* widget);
    void cluster_selection_changed(QString);
    void partition_selection_changed(QString);

protected:
    SaveResult prompt_save_changes();
    bool isPartitionModified(int partitionRow);
    void setPartitionItemState(int partitionRow, PartitionState state, bool overwrite = false);
    PartitionState partitionItemState(int partitionRow);

private:
   CommandExecutionThread command_thread;

   int loading;
   bool v2mpkg;
   int currentPartitionRow;
};

}

Q_DECLARE_METATYPE(xoscar::PartitionState)

#endif // XOSCAR_TABGENERALINFORMATION_H
