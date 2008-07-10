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
	void save_cluster_info_handler();
	void refresh_list_partitions();
    void refresh_partition_info();
    void setDefaultPartitionValues();
    void enablePartitionInfoWidgets(bool enable);
     int handle_thread_result (int command_id, const QString result);
    void handle_oscar_config_result(QString list_distros);
	bool save();
	bool undo();
    void partition_list_rowChanged_handler(int);
    void clusters_list_rowChanged_handler(int);

signals:
    void widgetContentsModified(QWidget* widget);
    void cluster_selection_changed(QString);
    void partition_selection_changed(QString);

private:
   CommandExecutionThread command_thread;

   bool loading;
};

}

#endif // XOSCAR_TABGENERALINFORMATION_H
