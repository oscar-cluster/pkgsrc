/*
 *  Copyright (c) 2007-2008 Oak Ridge National Laboratory, 
 *                          Geoffroy Vallee <valleegr@ornl.gov>
 *                          All rights reserved
 *  This file is part of the xoscar software, part of the OSCAR software.
 *  For license information, see the COPYING file in the top level directory
 *  of the OSCAR source.
 */

/**
 * @file XOSCAR_TabGeneralInformation.cpp
 * @brief Actual implementation of the XOSCAR_TabGeneralInformation class.
 * @author Robert Babilon
 */

#include "XOSCAR_TabGeneralInformation.h"
#include "utilities.h"
#include "Loading.h"
#include <QMessageBox>

using namespace xoscar;

XOSCAR_TabGeneralInformation::XOSCAR_TabGeneralInformation(QWidget* parent)
    : QWidget(parent)
    , v2mpkg(false)
    , loading(0)
{
	setupUi(this);

	// signals for widgets when they are modified
	connect(partitionNameEditWidget, SIGNAL(textEdited(const QString&)),
	        this, SLOT(partitionName_textEdited_handler(const QString&)));

	connect(partitionDistroComboBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(partitionDistro_currentIndexChanged_handler(int)));

	connect(partitionNumberNodesSpinBox, SIGNAL(valueChanged(int)),
	        this, SLOT(partitionNodes_valueChanged_handler(int)));

	// signals for control widgets
	connect(addPartitionButton, SIGNAL(clicked()),
	        this, SLOT(add_partition_handler()));

    connect(removeClusterButton_2, SIGNAL(clicked()),
            this, SLOT(remove_partition_handler()));

    connect(listOscarClustersWidget, SIGNAL(itemSelectionChanged ()),
                    this, SLOT(refresh_list_partitions()));

    connect(listOscarClustersWidget, SIGNAL(currentRowChanged(int)),
            this, SLOT(clusters_list_rowChanged_handler(int)));

    connect(listClusterPartitionsWidget, SIGNAL(currentRowChanged(int)),
            this, SLOT(partition_list_rowChanged_handler(int)));

	connect(listClusterPartitionsWidget, SIGNAL(itemSelectionChanged ()),
            this, SLOT(refresh_partition_info()));

    connect(virtualMachinesCheckBox, SIGNAL(stateChanged(int)),
            this, SLOT(virtualMachinesCheckBox_stateChanged_handler(int)));

    connect(virtualMachinesComboBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(virtualMachinesComboBox_currentIndexChanged_handler(int)));

	connect(saveClusterInfoButton, SIGNAL(clicked()),
	        this, SLOT(save()));

	// signals for command execution thread
    connect(&command_thread, SIGNAL(thread_terminated(CommandTask::CommandTasks, QString)),
        this, SLOT(handle_thread_result (CommandTask::CommandTasks, QString)));

    connect(&command_thread, SIGNAL(finished()),
            this, SLOT(command_thread_finished()));

    enablePartitionInfoWidgets(false);
    setDefaultPartitionValues();
}

XOSCAR_TabGeneralInformation::~XOSCAR_TabGeneralInformation()
{
}

/**
 *  @author Robert Babilon
 *
 *  Resets the modified flag and emits the signal that this widget
 *  has been saved.
 *  This method is overriden from the interface.
 *
 *  @todo return false if the save fails to inform caller they should revert
 *  back to this tab.
 *
 *  @return true if changes were saved successfully; otherwise false.
 */
bool XOSCAR_TabGeneralInformation::save()
{
    cout << "save()" << endl;

	save_cluster_info_handler();
    refresh_list_partitions();

	modified = false;

	return true;
}

/**
 *  @author Robert Babilon
 *
 *  Resets the modified flag.
 *  This method is overriden from the interface.
 *
 *  @todo emit a signal that changes were undone/ignored
 *  @todo reset the widgets to a last known state.
 *        if the partition was added (not saved), remove it from the list
 *        if the partition was modified, could call the command to get the list
 *        of partitions for the current cluster.
 *  @todo return false if the undo fails to inform the caller to revert back to
 *  this tab.
 *
 *  @return true if changes were reversed successfully; otherwise false.
 */
bool XOSCAR_TabGeneralInformation::undo()
{
    cout << "undo()" << endl;

	modified = false;

	return true;
}

/**
 *  @author Robert Babilon
 *
 *  Slot called when the user edits the partition name.
 *
 *  @param text The new partition name.
 */
void XOSCAR_TabGeneralInformation::partitionName_textEdited_handler(const QString& text)
{
	if(loading) return;

    if(listClusterPartitionsWidget->currentItem() == NULL) {
        cout << "ERROR: no partition selected" << endl;
        return;
    }
    listClusterPartitionsWidget->currentItem()->setText(text);

	modified = true;
	emit widgetContentsModified(this);
}

/**
 *  @author Robert Babilon
 *
 *  Slot called when the user changes partition distro.
 *
 *  @param index The index value corresponding to the partition distro
 *  in the combo box.
 */
void XOSCAR_TabGeneralInformation::partitionDistro_currentIndexChanged_handler(int index)
{
	if(!loading) {
	    modified = true;
	    emit widgetContentsModified(this);
    }

    command_thread.init(CommandTask::DISPLAY_DEFAULT_OPKGS, QStringList(""));
}

/**
 *  @author Robert Babilon
 *
 *  Slot called when the user changes the number of partition nodes.
 *
 *  @param index The new number of partition nodes
 *
 */
void XOSCAR_TabGeneralInformation::partitionNodes_valueChanged_handler(int index)
{
	if(loading) return;
	modified = true;
	emit widgetContentsModified(this);
}

/**
 *  @author Robert Babilon
 *
 *  Slot called when the user (or program) checks or unchecks the "virtual
 *  machines based on" check box. 
 *
 *  This function checks the "loading" variable in order to distinguish between
 *  user and program changes.
 *
 *  @param state The current state of the check box.
 */
void XOSCAR_TabGeneralInformation::virtualMachinesCheckBox_stateChanged_handler(int state)
{
    if(!loading) {
        modified = true;
        emit widgetContentsModified(this);
        
        virtualMachinesComboBox->setEnabled(state == Qt::Checked);
    }
}

/**
 *  @author Robert Babilon
 *
 *  Slot called when the user (or program) changes the selection in the virtual machine
 *  combobox.
 *
 *  This function checks the "loading" variable in order to distinguish between
 *  user and program changes.
 *
 *  @param index The index of the selected item in the combobox.
 */
void XOSCAR_TabGeneralInformation::virtualMachinesComboBox_currentIndexChanged_handler(int index)
{
    if(!loading) { 
        modified = true;
        emit widgetContentsModified(this);
    }
}

/**
 * @author Geoffroy Vallee.
 *
 * Slot that handles the click on the "add partition" button.
 *
 * @todo Find a good name by default that avoids conflicts if the user does not
 * change it.
 * @todo Check if a cluster is selected.
 * @todo make a function to handle the prompt save dialog
 */
void XOSCAR_TabGeneralInformation::add_partition_handler()
{
    if(modified) {
        // prompt to save previous changes
        QMessageBox msg(QMessageBox::NoIcon, tr("Save changes?"), tr("The previously added or modified partition has not been saved.\n")
                        + tr("Would you like to save your changes?"),
                        QMessageBox::Save|QMessageBox::No|QMessageBox::Cancel, this);

        switch(msg.exec()) {
            case QMessageBox::Save: 
                this->save();
                break;
            case QMessageBox::No:
                this->undo();
                break;
            case QMessageBox::Cancel:
                return;
                break;
        }
    }

    // must have a cluster selected in order to add a partition to it
    if (listOscarClustersWidget->currentRow() == -1) {
        return;
    }

    // this is still a problem dispite any fancy thread management system. here
    // we'll need to "pause" the GUI by using a dialog and wait until the thread
    // has finished all of it's pending tasks.
    QMessageBox msg(QMessageBox::NoIcon,
                    QString("Test Message Box"), 
                    QString("This is a temporary pausing mechanism.\nClick OK when you feel the thread has finished it's duties."),
                    QMessageBox::Ok);
    msg.exec();

    listClusterPartitionsWidget->addItem ("New_Partition");
    listClusterPartitionsWidget->update ();

    setDefaultPartitionValues();

    listClusterPartitionsWidget->setCurrentRow(listClusterPartitionsWidget->count()-1);

	modified = true;
	emit widgetContentsModified(this);
}

/**
 *  @author Robert Babilon
 *
 *  Slot that handles the click signal on the "remove partition" button.
 *
 *  @todo make a function to handle the prompt save dialog
 */
void XOSCAR_TabGeneralInformation::remove_partition_handler()
{
    if(modified) {
        // prompt to save previous changes
        QMessageBox msg(QMessageBox::NoIcon, tr("Save changes?"), tr("The previously added or modified partition has not been saved.\n")
                        + tr("Would you like to save your changes?"),
                        QMessageBox::Save|QMessageBox::No|QMessageBox::Cancel, this);

        switch(msg.exec()) {
            case QMessageBox::Save: 
                this->save();
                break;
            case QMessageBox::No:
                this->undo();
                break;
            case QMessageBox::Cancel:
                return;
                break;
        }
    }

    if(listClusterPartitionsWidget->currentRow() == -1) {
        return;
    }

    delete listClusterPartitionsWidget->takeItem(listClusterPartitionsWidget->currentRow());
}

/**
 * @author Geoffroy Vallee.
 *
 * This function handles the update of OSCAR cluster information when a new 
 * cluster is selected in the "General Information" widget. It displays the list
 * of partitions within the cluster.
 *
 * @todo currently has a bug when cluster changes the list of partitions is
 * cleared out without checking for any pending changes. we need to add a check
 * for such changes in here and then execute DISPLAY_PARTITIONS.
 */
void XOSCAR_TabGeneralInformation::refresh_list_partitions ()
{
    if(listOscarClustersWidget->currentRow() == -1) {
        return;
    }

	// oscar does not (currently) support multiple clusters so the Perl
	// scripts have the cluster hard coded. This argument is ignored, but in
	// the future would be used to indicate which cluster we are requesting
	// partitions for.
    command_thread.init(CommandTask::DISPLAY_PARTITIONS, QStringList(listOscarClustersWidget->currentItem()->text()));
}

/**
 * @author Geoffroy Vallee.
 *
 * This function handles the update of partition information when a new 
 * partition is selected in the "General Information" widget.
 */
void XOSCAR_TabGeneralInformation::refresh_partition_info()
{
    if(listClusterPartitionsWidget->currentRow() == -1) {
        return;
    }

    QList<QListWidgetItem *> list =
        listClusterPartitionsWidget->selectedItems();

    // if nothing is in the list, we cannot select the next one
    if (list.count() == 0) {
        return;
    }

    QListIterator<QListWidgetItem *> i(list);
    QString current_partition = i.next()->text();
    partitionNameEditWidget->setText(current_partition);

    // here we can check if this entry is modified or newly added
    // if added/modified, we would avoid calling the DISPLAY_PARTITION_NODES
    // since that would overwrite existing modifications

    /* We display the number of nodes composing the partition */
    command_thread.init(CommandTask::DISPLAY_PARTITION_NODES, 
                        QStringList(current_partition));

    /* We get the list of supported distros */
    command_thread.init(CommandTask(CommandTask::GET_SETUP_DISTROS, QStringList("")));

    /* We get the Linux distribution on which the partition is based */
    command_thread.init(CommandTask(CommandTask::DISPLAY_PARTITION_DISTRO, QStringList("")));
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
void XOSCAR_TabGeneralInformation::save_cluster_info_handler()
{
    int nb_nodes = partitionNumberNodesSpinBox->value();
    QString partition_name = partitionNameEditWidget->text();
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
            tmp = partition_name.toStdString() + "_node" + Utilities::intToStdString(i);
            args << tmp.c_str();
        }
        command_thread.init (CommandTask::ADD_PARTITION, args);
    }
    /* We unset the selection of the partition is order to be able to update
       the widget. If we do not do that, a NULL pointer is used and the app
       crashes. */
    listClusterPartitionsWidget->setCurrentRow(-1);
}

/**
 *  @author Robert Babilon
 *
 *  Sets the default values for the widgets in the partition information group
 *  box.
 *
 *  This function will set the "loading" boolean.
 */
void XOSCAR_TabGeneralInformation::setDefaultPartitionValues()
{
    Loading loader(&loading);

    partitionNameEditWidget->setText(tr(""));
    partitionNumberNodesSpinBox->setValue(0);
    partitionDistroComboBox->setCurrentIndex(0);

    virtualMachinesCheckBox->setCheckState(Qt::Unchecked);
    virtualMachinesComboBox->setCurrentIndex(0);
}

/**
 *  @author Robert Babilon
 *
 *  Enables or disables the widgets in the partition information group box.
 *
 *  @param enable true to enable the widgets in the partition information
 *  group box; otherwise false.
 */
void XOSCAR_TabGeneralInformation::enablePartitionInfoWidgets(bool enable)
{
    partitionNameEditWidget->setEnabled(enable);
    partitionNumberNodesSpinBox->setEnabled(enable);
    partitionDistroComboBox->setEnabled(enable);

    virtualMachinesCheckBox->setEnabled(enable ? v2mpkg : false);
    virtualMachinesComboBox->setEnabled(enable ? (v2mpkg && virtualMachinesCheckBox->checkState() == Qt::Checked) : false);
}

/**
 *  @author Robert Babilon
 *
 *  @param row The row index of the selected cluster
 */
void XOSCAR_TabGeneralInformation::clusters_list_rowChanged_handler(int row)
{
    if(row == -1) {
        listClusterPartitionsWidget->setCurrentRow(-1);

        emit cluster_selection_changed(tr(""));
    }
    else {
        emit cluster_selection_changed(listOscarClustersWidget->item(row)->text());
    }
}

/**
 *  @author Robert Babilon
 *
 *  Slot called when the row has changed in the partition list widget.
 *  Sets the default values to the partition info widgets and disables them if
 *  the row is -1. Otherwise the partition info widgets are enabled.
 *
 *  @param row Index of the newly selected row in the list widget. -1 if no row
 *  is currently selected.
 */
void XOSCAR_TabGeneralInformation::partition_list_rowChanged_handler(int row)
{
    if(row == -1) {
        setDefaultPartitionValues();
        enablePartitionInfoWidgets(false);

        emit partition_selection_changed(tr(""));
    }
    else {
        enablePartitionInfoWidgets(true);

        emit partition_selection_changed(listClusterPartitionsWidget->item(row)->text());
    }
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
int XOSCAR_TabGeneralInformation::handle_thread_result (CommandTask::CommandTasks command_id, 
    const QString result)
{
    QStringList list;
    cout << "GeneralInformation: result from cmd exec thread received: "
         << command_id
         << endl;

    if (command_id == CommandTask::DISPLAY_PARTITIONS) {
        Loading loader(&loading);
        // We parse the result: one partition name per line.
        // skip empty strings? otherwise we have extra partitions added
        // could also check result for empty string
        list = result.split("\n", QString::SkipEmptyParts);
        listClusterPartitionsWidget->clear();
        for (int i = 0; i < list.size(); ++i){
            listClusterPartitionsWidget->addItem (list.at(i));
        }
        listClusterPartitionsWidget->update();
    } else if (command_id == CommandTask::DISPLAY_PARTITION_NODES) {
        Loading loader(&loading);
        list = result.split(" ", QString::SkipEmptyParts);
        partitionNumberNodesSpinBox->setValue(list.size());
    } else if (command_id == CommandTask::DISPLAY_PARTITION_DISTRO) {
        cerr << "ERROR: Not yet implemented" << endl;
/*        int index = partitionDistroComboBox->findText(distro_name);
        partitionDistroComboBox->setCurrentIndex(index);*/
    } else if (command_id == CommandTask::SETUP_DISTRO) {
        // We could here try to see if the command was successfully executed or
        // not. Otherwise, nothing to do here.
    } else if (command_id == CommandTask::GET_SETUP_DISTROS) {
        //command_thread.init (CommandTask::GET_LIST_DEFAULT_REPOS, QStringList (""));
        // @todo This GET_LIST_DEFAULT_REPOS is not handled in this class. It
        // was originally in XOSCAR_MainWindow and may or may not need to be
        // ported into this class.
    } else if(command_id == CommandTask::DISPLAY_DEFAULT_OPKGS) {
        Loading loader(&loading);
        virtualMachinesComboBox->clear();

        list = result.split("\n", QString::SkipEmptyParts);
        int count = list.size();
        int index = list.indexOf("v2m");

        v2mpkg = (index != -1);
        if(v2mpkg) {
            list.removeAt(index);
            count--;
        }
        for(int i = 0; i < count; ++i) {
            virtualMachinesComboBox->addItem(list.at(i));
        }
        virtualMachinesCheckBox->setEnabled(v2mpkg);
        virtualMachinesComboBox->setEnabled(v2mpkg && virtualMachinesCheckBox->checkState() == Qt::Checked);
    }

    // wake the thread up so it can exit run() and be ready for another command
    command_thread.wakeThread();
    return 0;
}

/**
 * @author Robert Babilon
 *
 * Slot called when the QThread signal finished() is emitted.
 * Starts the command thread again only if it has tasks left.
 */
void XOSCAR_TabGeneralInformation::command_thread_finished()
{
    if(!command_thread.isEmpty()) { 
        command_thread.start();
    }
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
void XOSCAR_TabGeneralInformation::handle_oscar_config_result(QString list_distros)
{
    Loading loader(&loading);

    cout << list_distros.toStdString () << endl;
    QStringList list = list_distros.split (" ", QString::SkipEmptyParts);

	partitionDistroComboBox->clear();

    for(int i = 0; i < list.size(); i++) {
        partitionDistroComboBox->addItem (list.at(i));
    }
}
