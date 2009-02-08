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

XOSCAR_TabGeneralInformation::XOSCAR_TabGeneralInformation(ThreadHandlerInterface* handler, QWidget* parent)
    : QWidget(parent)
    , ThreadUserInterface(handler)
{
    v2mpkg = false;
    loading = 0;
    setModifiedFlag(false);

    // register the PartitionState enum with Qt in order to use it with the
    // QListWidgetItem::data() property
    qRegisterMetaType<PartitionState>("PartitionState");

    setupUi(this);

    // signals for widgets when they are modified
    connect(partitionNameEditWidget, SIGNAL(textEdited(const QString&)),
            this, SLOT(partitionName_textEdited_handler(const QString&)));

    connect(partitionDistroComboBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(partitionDistro_currentIndexChanged_handler(int)));

    connect(partitionNumberNodesSpinBox, SIGNAL(valueChanged(int)),
            this, SLOT(partitionNodes_valueChanged_handler(int)));

    connect(virtualMachinesCheckBox, SIGNAL(stateChanged(int)),
            this, SLOT(virtualMachinesCheckBox_stateChanged_handler(int)));

    connect(virtualMachinesComboBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(virtualMachinesComboBox_currentIndexChanged_handler(int)));

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

    connect(saveClusterInfoButton, SIGNAL(clicked()),
            this, SLOT(save()));

    enablePartitionInfoWidgets(false);
    setDefaultPartitionValues();
}

XOSCAR_TabGeneralInformation::~XOSCAR_TabGeneralInformation()
{
}

/**
 *  @author Robert Babilon
 *
 *  Saves changes to current partition item and refreshes the list of partitions
 *  based on current cluster selection.
 *
 *  This method is overriden from the interface.
 *
 *  @return XOSCAR_TabWidgetInterface::SaveResult
 */
XOSCAR_TabWidgetInterface::SaveResult XOSCAR_TabGeneralInformation::save()
{
    cout << "DEBUG: save()" << endl;

    SaveResult result = save_cluster_info_handler();
    if(result == Saving) {
        refresh_list_partitions();
    }

    return result;
}

/**
 *  @author Robert Babilon
 *
 *  This method is overriden from the interface.
 *
 *  @todo emit a signal that changes were undone/ignored
 *  @todo reset the widgets to a last known state.
 *
 *  @return XOSCAR_TabWidgetInterface::SaveResult
 */
XOSCAR_TabWidgetInterface::SaveResult XOSCAR_TabGeneralInformation::undo()
{
    //cout << "DEBUG: undo()" << endl;

    SaveResult result = NoChange;

    if(currentPartitionRow == -1) {
        return result;
    }
    
    int tempRow = currentPartitionRow;
    setModifiedFlag(false);

    // get the state flag, if modified, then undo the changes by calling
    // refresh on this partition; if created, remove the item
    if(partitionItemState(tempRow) == Created) {
        delete listClusterPartitionsWidget->takeItem(tempRow);
        result = Undoing;
    }
    else { 
        // set this row to Saved state and overwrite any previous values
        setPartitionItemState(tempRow, Saved, true);
        listClusterPartitionsWidget->setCurrentRow(tempRow);

        // refresh the list of partitions to undo all changes to this partition
        refresh_list_partitions();

        result = Undoing;
    }

    return result;
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
        cerr << "ERROR: no partition selected" << endl;
        return;
    }
    listClusterPartitionsWidget->currentItem()->setText(text);

    setModifiedFlag();
    setPartitionItemState(currentPartitionRow, Modified);
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
        setModifiedFlag();
        setPartitionItemState(currentPartitionRow, Modified);
        emit widgetContentsModified(this);
    }

    threadHandler->enqueue_command_task(CommandTask(xoscar::DISPLAY_DEFAULT_OPKGS, QStringList(""), dynamic_cast<ThreadUserInterface*>(this)));
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
    setModifiedFlag();
    setPartitionItemState(currentPartitionRow, Modified);
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
        setModifiedFlag();
        setPartitionItemState(currentPartitionRow, Modified);
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
        setModifiedFlag();
        setPartitionItemState(currentPartitionRow, Modified);
        emit widgetContentsModified(this);
    }
}

/**
 * @author Geoffroy Vallee.
 *
 * Slot that handles the click on the "add partition" button.
 */
void XOSCAR_TabGeneralInformation::add_partition_handler()
{
    //cout << "DEBUG: add_partition_handler: ENTER" << endl;

    // connect slot
    connect(this, SIGNAL(command_thread_tasks_done()),
            this, SLOT(add_partition()));

    SaveResult result = prompt_save_changes();

    if(result == SaveFailed || result == UserCanceled) {
        // disconnect slot
        disconnect(this, SIGNAL(command_thread_tasks_done()),
                   this, SLOT(add_partition()));
    }
    else if(result == NoChange) {
        add_partition();
    }
}

/**
 * @author Robert Babilon
 *
 * Slot that adds a new item to the list of partitions.
 *
 * @todo Find a good name by default that avoids conflicts if the user does not
 * change it.
 */
void XOSCAR_TabGeneralInformation::add_partition()
{
    disconnect(this, SIGNAL(command_thread_tasks_done()),
               this, SLOT(add_partition()));

    // must have a cluster selected in order to add a partition to it
    if (listOscarClustersWidget->currentRow() == -1) {
        return;
    }

    QListWidgetItem *pItem = new QListWidgetItem("New_Partition");
    QVariant var;
    var.setValue(Created);
    pItem->setData(Qt::UserRole, var);
    listClusterPartitionsWidget->addItem (pItem);
    listClusterPartitionsWidget->update ();

    setDefaultPartitionValues();

    listClusterPartitionsWidget->setCurrentRow(listClusterPartitionsWidget->count()-1);

    setModifiedFlag();
    emit widgetContentsModified(this);
}

/**
 *  @author Robert Babilon
 *
 *  Slot that handles the click signal on the "remove partition" button.
 *
 *  @todo execute a command REMOVE_PARTITION to remove the partition from the
 *  cluster when the item is saved or modified. Note when the partition is
 *  created, we can simply remove the item from the list. If the partition name 
 *  were modified, we would need to retrieve the original name and then execute
 *  the remove command.
 */
void XOSCAR_TabGeneralInformation::remove_partition_handler()
{
    if(listClusterPartitionsWidget->currentRow() == -1) {
        return;
    }

    setModifiedFlag(false);
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

    // this is semi-temporary fix. oscar only allows a single cluster
    // currently. avoid refreshing partitions fixes a seg fault.
    // the seg fault has to do with a lot of stuff.
    if(isPartitionModified(currentPartitionRow)) return;

    // oscar does not (currently) support multiple clusters so the Perl
    // scripts have the cluster hard coded. This argument is ignored, but in
    // the future would be used to indicate which cluster we are requesting
    // partitions for.
    threadHandler->enqueue_command_task(CommandTask(xoscar::DISPLAY_PARTITIONS, 
                                        QStringList(listOscarClustersWidget->currentItem()->text()), 
                                        dynamic_cast<ThreadUserInterface*>(this)));
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

    // if added/modified, we avoid calling the DISPLAY_PARTITION_NODES
    // since that would overwrite existing modifications
    // GET_SETUP_DISTROS will retrieve the list of distros and add them to the
    // comobo box; this should also be skipped b/c the selection would change
    if(isPartitionModified(listClusterPartitionsWidget->currentRow())) {
        //cout << "DEBUG: refresh_partition_info: current row is modified, ignoring: " << listClusterPartitionsWidget->currentRow() << endl;
        return;
    }
    //cout << "DEBUG: refresh_partition_info: refreshing info for row: " << listClusterPartitionsWidget->currentRow() << endl;

    /* We display the number of nodes composing the partition */
    threadHandler->enqueue_command_task(CommandTask(xoscar::DISPLAY_PARTITION_NODES, 
                        QStringList(current_partition),dynamic_cast<ThreadUserInterface*>(this)));

    /* We get the list of supported distros */
    threadHandler->enqueue_command_task(CommandTask(xoscar::GET_SETUP_DISTROS, QStringList(""), dynamic_cast<ThreadUserInterface*>(this)));

    /* We get the Linux distribution on which the partition is based */
    threadHandler->enqueue_command_task(CommandTask(xoscar::DISPLAY_PARTITION_DISTRO, QStringList(""), dynamic_cast<ThreadUserInterface*>(this)));
}

/**
 * @author Geoffroy Vallee.
 *
 * Slot that handles the click on the "Save Cluster Configuration" button.
 *
 * @return XOSCAR_TabWidgetInterface::SaveResult
 *
 * @todo Check if the partition name already exists or not.
 * @todo Check the return value of the command to add partition information
 *       in the database.
 * @todo Use specific commands depending on the item's status. i.e.
 *       use ADD_PARTITION when adding partitions; use UPDATE_PARTITION 
 *       when an existing partition was modified and only needs updating.
 */
XOSCAR_TabWidgetInterface::SaveResult XOSCAR_TabGeneralInformation::save_cluster_info_handler()
{
    cout << "DEBUG: save_cluster_info_handler()" << endl;

    SaveResult result = NoChange;

    if(!isPartitionModified(currentPartitionRow)) {
        return result;
    }

    int nb_nodes = partitionNumberNodesSpinBox->value();
    QString partition_name = partitionNameEditWidget->text();
    QString partition_distro = partitionDistroComboBox->currentText();

    if (partition_name.compare("") == 0 || nb_nodes == 0 
        || partition_distro.compare("") == 0) {
        cerr << "ERROR: invalid partition information" << endl;
        // need to inform caller that they should not continue either since this
        // has failed. i.e. in save() do not call refresh_list_partitions() b/c
        // it would erase the changes.
        result = SaveFailed;

        QMessageBox msg(QMessageBox::Warning, tr("Invalid Partition Information"), 
                        tr("The partition information is invalid and cannot be saved.\n") + 
                        tr("Please correct the error(s) and try again."), 
                        QMessageBox::Ok, this);
        msg.exec();
    } else {
        QStringList args;
        args << partition_name << partition_distro;

        /* We had now the compute nodes, giving them a default name */
        string tmp;
        for (int i=0; i < nb_nodes; i++) {
            tmp = partition_name.toStdString() + "_node" + Utilities::intToStdString(i);
            args << tmp.c_str();
        }

        setModifiedFlag(false);
        result = Saving;

        threadHandler->enqueue_command_task(CommandTask(xoscar::ADD_PARTITION, args, dynamic_cast<ThreadUserInterface*>(this)));

        /* We unset the selection of the partition is order to be able to update
        the widget. If we do not do that, a NULL pointer is used and the app
        crashes. */
        listClusterPartitionsWidget->setCurrentRow(-1);
    }

    return result;
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

    virtualMachinesCheckBox->setEnabled(enable && v2mpkg);
    virtualMachinesComboBox->setEnabled(enable && (v2mpkg && virtualMachinesCheckBox->checkState() == Qt::Checked));
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
 *  @todo When saving changes here, we will need to wait for the thread to
 *  finish its execution and then re-select the newly selected partion. More
 *  notes added in code where this mechanism is needed.
 */
void XOSCAR_TabGeneralInformation::partition_list_rowChanged_handler(int row)
{
    if(row == -1) {
        setDefaultPartitionValues();
        enablePartitionInfoWidgets(false);

        emit partition_selection_changed(tr(""));
    }
    else {
        //cout << "DEBUG: partition_list_rowChanged_handler: row: " << row << endl;
        enablePartitionInfoWidgets(true);

        emit partition_selection_changed(listClusterPartitionsWidget->item(row)->text());
    }

    //cout << "DEBUG: partition_list_rowChanged_handler: currentPartitionRow: " << currentPartitionRow << endl;

    // if count is 0, nothing left to save; clear out flag variables
    if(listClusterPartitionsWidget->count() == 0) {
        //cout << "DEBUG: partition_list_rowChanged_handler: partitions list count == 0" << endl;
        setModifiedFlag(false);
    }
    // if row is -1 must prompt here to save changes only if currentPartitionRow
    // is != -1 meaning something was selected before. saving or reversing
    // changes here is easy b/c we do not have to select a new item after the
    // thread has finished. only if the save() or undo() fail or they cancel do
    // we select the previous partition item
    else if(row == -1) {
        //cout << "DEBUG: partition_list_rowChanged_handler: row == -1" << endl;

        SaveResult result = prompt_save_changes();
        if(result == Saving || result == Undoing) {
            // saved or undo changes, ignore
            // undo here does not need to set the old selection b/c the new
            // selection is -1.
            setModifiedFlag(false);
        }
        else {
            // leave the currentPartitionRow as is; next call to this method
            // will result in row == currentPartitionRow which does nothing.
            listClusterPartitionsWidget->setCurrentRow(currentPartitionRow);
        }
    }
    // row is >= 0, something was selected.
    else {
        //cout << "DEBUG: partition_list_rowChanged_handler: row >= 0" << endl;

        // will need to set the text of the partition to the text widget
        partitionNameEditWidget->setText(listClusterPartitionsWidget->item(row)->text());

        // no previous item was selected, so we only to need save the selected row
        if(currentPartitionRow == -1) {
            currentPartitionRow = row;
            //cout << "DEBUG: partition_list_rowChanged_handler: (set currentPartitionRow = row): " << currentPartitionRow << endl;

            /*if(isPartitionModified(currentPartitionRow)) {
                // when would the selected row be modified while previous row was -1?
                //cout << "DEBUG: FLUKE! previous row was -1, and newly selected row is modified!" << endl;
            }*/
        }
        // if the currentPartitionRow is same as this newly selected row, then
        // we do not want to do anything. the user probably canceled some
        // changes so we programmatically selected the previous row.
        else if(currentPartitionRow == row) { 
            //cout << "DEBUG: partition_list_rowChanged_handler: currentPartitionRow == row" << endl;
        }
        // different item selected: must check first if the item is modified
        else {
            //cout << "DEBUG: partition_list_rowChanged_handler: currentPartitionRow != row" << endl;

            // previous partition not modified; we need to save the new row
            if(!isPartitionModified(currentPartitionRow)) {
                //cout << "DEBUG: partition_list_rowChanged_handler: previous partition not modified" << endl; 
                currentPartitionRow = row;
            }
            else {
                //cout << "DEBUG: partition_list_rowChanged_handler: previous partition was modified!" << endl;
                SaveResult result = prompt_save_changes();
                if(result == Saving || result == Undoing) {
                    // will need to add a pausing mechanism here to wait for the
                    // thread to finish and re-select the newly selected item
                    // since the items would be cleared out causing a potential
                    // re-ordering of items. if the partition names must all be
                    // unique, this will work. for now, the user will have to
                    // manual re-select.
                    currentPartitionRow = row;
                }
                else {
                    //cout << "DEBUG: partition_list_rowChanged_handler: save failed! setting row to: " << currentPartitionRow << endl;
                    // if cancel(), set selected row to currentPartitionRow.
                    // since the row and currentPartitionRow would be the same
                    // (after this most recent call), the item will be selected
                    // yet no information will be lost.
                    listClusterPartitionsWidget->setCurrentRow(currentPartitionRow);
                }
            }
        }
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
 *  @todo will need to handle a few new commands such as: 
 *        REMOVE_PARTITION,
 *        UPDATE_PARTITION
 */
int XOSCAR_TabGeneralInformation::handle_thread_result (xoscar::CommandId command_id, 
    const QString result)
{
    QStringList list;
    cout << "GeneralInformation: result from cmd exec thread received: "
         << command_id
         << endl;

    if (command_id == xoscar::DISPLAY_PARTITIONS) {
        Loading loader(&loading);
        // We parse the result: one partition name per line.
        // skip empty strings? otherwise we have extra partitions added
        // could also check result for empty string
        list = result.split("\n", QString::SkipEmptyParts);
        // check for changes? they should be saved before this call
        listClusterPartitionsWidget->clear();
        for (int i = 0; i < list.size(); ++i){
            // add item using new and set flag to Saved
            QListWidgetItem *pItem = new QListWidgetItem(list.at(i));
            QVariant var;
            var.setValue(Saved);
            pItem->setData(Qt::UserRole, var);
            listClusterPartitionsWidget->addItem (pItem);
        }
        /* *** Test Code *** 
            QListWidgetItem *pItem = new QListWidgetItem("New_Partition0");
            QVariant var;
            var.setValue(Saved);
            pItem->setData(Qt::UserRole, var);
            listClusterPartitionsWidget->addItem (pItem);
        // *** End Test Code ***/ 
        listClusterPartitionsWidget->update();
    } else if (command_id == xoscar::DISPLAY_PARTITION_NODES) {
        Loading loader(&loading);
        list = result.split(" ", QString::SkipEmptyParts);
        partitionNumberNodesSpinBox->setValue(list.size());
    } else if (command_id == xoscar::DISPLAY_PARTITION_DISTRO) {
        Loading loader(&loading);
        int index = partitionDistroComboBox->findText(result);
        if(index == -1) {
            cout << "WARNING: partition's distro was not found" << endl;
        }
        partitionDistroComboBox->setCurrentIndex(index);
    } else if (command_id == xoscar::SETUP_DISTRO) {
        // We could here try to see if the command was successfully executed or
        // not. Otherwise, nothing to do here.
    } else if (command_id == xoscar::GET_SETUP_DISTROS) {
        handle_oscar_config_result(result);
    } else if(command_id == xoscar::DISPLAY_DEFAULT_OPKGS) {
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
    return 0;
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

/**
 * @author Robert Babilon
 *
 * This function calls save() or undo() method in this class.
 * If changes were made and the user wishes to save, this returns true
 * If changes were made and the user wishes to undo the changes, this returns
 * true.
 * If the user wishes to cancel, this returns false.
 *
 * @return XOSCAR_TabWidgetInterface::SaveResult enum
 */
XOSCAR_TabWidgetInterface::SaveResult XOSCAR_TabGeneralInformation::prompt_save_changes()
{
    SaveResult result = NoChange;

    if(currentPartitionRow == -1) {
        return result;
    }

    if(isPartitionModified(currentPartitionRow)) {
        QMessageBox msg(QMessageBox::NoIcon, tr("Save changes?"), tr("The previous partition has been modified.\n")
                                                                + tr("Would you like to save your changes?"),
                        QMessageBox::Save|QMessageBox::No|QMessageBox::Cancel, this);

        switch(msg.exec()) {
            case QMessageBox::Save: 
                result = save();
                break;
            case QMessageBox::No:
                result = undo();
                break;
            case QMessageBox::Cancel:
                result = UserCanceled;
                break;
        }
    }
    
    return result;
}

/**
 * @author Robert Babilon
 *
 * Returns true if the partition's state (stored in the data property of
 * QListWidgetItem) is Modified or Created; otherwise this returns false.
 *
 * @param partitionRow The row index of the partition item to check.
 */
bool XOSCAR_TabGeneralInformation::isPartitionModified(int partitionRow)
{
    bool result = false;

    if(partitionRow == -1) {
        return result;
    }

    QListWidgetItem *pItem = listClusterPartitionsWidget->item(partitionRow);
    QVariant var = pItem->data(Qt::UserRole);
    PartitionState state = var.value<PartitionState>();

    if(state == Modified || state == Created) {
        result = true;
    }

    return result;
}

/**
 * @author Robert Babilon
 *
 * Sets the partition's state (stored in the data property of QListWidgetITem)
 * to state only if the new state is greater than the old state. If overwrite is
 * true, then the new state will be saved to the item regardless of values.
 *
 * @param partitionRow The row index of partition to set the state.
 * @param state The new state the partition should be set to.
 * @param overwrite Send true to set the new state regarless of the old state;
 * send false (or ignore) to set the new state based on state precedence.
 */
void XOSCAR_TabGeneralInformation::setPartitionItemState(int partitionRow, PartitionState state, bool overwrite/*=false*/)
{
    if(partitionRow == -1) return;

    QListWidgetItem *pItem = listClusterPartitionsWidget->item(partitionRow);

    if(pItem == NULL) {
        return;
    }

    if(!overwrite) {
        PartitionState oldState = pItem->data(Qt::UserRole).value<PartitionState>();
        state = (state > oldState) ? state : oldState;
    }

    QVariant data = pItem->data(Qt::UserRole);
    data.setValue(state);
    pItem->setData(Qt::UserRole, data);
}

/**
 * @author Robert Babilon
 *
 * Returns the xoscar::PartitionState of the specified partition item.
 * @param partitionRow The row index of the partition to retrieve state
 * information for.
 * @return xoscar::PartitionState The state of the specified partition item.
 */
PartitionState XOSCAR_TabGeneralInformation::partitionItemState(int partitionRow)
{
    if(partitionRow == -1) return Saved;

    QListWidgetItem *pItem = listClusterPartitionsWidget->item(partitionRow);

    if(pItem == NULL) {
        cerr << "ERROR: invalid partition item" << endl;
        return Saved;
    }
    return pItem->data(Qt::UserRole).value<PartitionState>();
}

/**
 * @author Robert Babilon
 *
 * If state is false, XOSCAR_TabGeneralInformation::currentPartitionRow is set to -1.
 * @param state The new state for the XOSCAR_TabGeneralInformation::modified flag.
 */
void XOSCAR_TabGeneralInformation::setModifiedFlag(bool state/*=true*/)
{
    if(state == false) {
        currentPartitionRow = -1;
    }
    this->modified = state;
}
