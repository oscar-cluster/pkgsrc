#include "XOSCAR_TabGeneralInformation.h"
#include "utilities.h"

using namespace xoscar;

XOSCAR_TabGeneralInformation::XOSCAR_TabGeneralInformation(QWidget* parent)
    : QWidget(parent)
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

	connect(listClusterPartitionsWidget, SIGNAL(itemSelectionChanged ()),
            this, SLOT(refresh_partition_info()));

	connect(saveClusterInfoButton, SIGNAL(clicked()),
	        this, SLOT(save()));

	// signals for command execution thread
    connect(&command_thread, SIGNAL(thread_terminated(int, QString)),
        this, SLOT(handle_thread_result (int, QString)));
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
 */
bool XOSCAR_TabGeneralInformation::save()
{
	save_cluster_info_handler();
    refresh_list_partitions();

	modified = false;
	emit widgetContentsSaved(this);

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
 */
bool XOSCAR_TabGeneralInformation::undo()
{
	modified = false;
	emit widgetContentsSaved(this);

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
	if(loading) return;
	modified = true;
	emit widgetContentsModified(this);
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
 * @author Geoffroy Vallee.
 *
 * Slot that handles the click on the "add partition" button.
 *
 * @todo Find a good name by default that avoids conflicts if the user does not
 * change it.
 * @todo Check if a cluster is selected.
 */
void XOSCAR_TabGeneralInformation::add_partition_handler()
{
    if (listOscarClustersWidget->currentRow() == -1)
        return;

    listClusterPartitionsWidget->addItem ("New_Partition");
    listClusterPartitionsWidget->update ();

	modified = true;
	emit widgetContentsModified(this);
}

/**
 * @author Geoffroy Vallee.
 *
 * This function handles the update of OSCAR cluster information when a new 
 * cluster is selected in the "General Information" widget. It displays the list
 * of partitions within the cluster.
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
    command_thread.init(DISPLAY_PARTITIONS, QStringList(listOscarClustersWidget->currentItem()->text()));
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
        command_thread.init (ADD_PARTITION, args);
    }
    /* We unset the selection of the partition is order to be able to update
       the widget. If we do not do that, a NULL pointer is used and the app
       crashes. */
    listClusterPartitionsWidget->setCurrentRow(-1);
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
int XOSCAR_TabGeneralInformation::handle_thread_result (int command_id, 
    const QString result)
{
    QStringList list;
    cout << "MainWindow: result from cmd exec thread received: "
         << command_id
         << endl;

    if (command_id == DISPLAY_PARTITIONS) {
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
        list = result.split(" ", QString::SkipEmptyParts);
        partitionNumberNodesSpinBox->setValue(list.size());
    } else if (command_id == DISPLAY_PARTITION_DISTRO) {
        cerr << "ERROR: Not yet implemented" << endl;
/*        int index = partitionDistroComboBox->findText(distro_name);
        partitionDistroComboBox->setCurrentIndex(index);*/
    } else if (command_id == SETUP_DISTRO) {
        // We could here try to see if the command was successfully executed or
        // not. Otherwise, nothing to do here.
    } else if (command_id == GET_SETUP_DISTROS) {
        command_thread.init (GET_LIST_DEFAULT_REPOS, QStringList (""));
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
	loading = true;

    cout << list_distros.toStdString () << endl;
    QStringList list = list_distros.split (" ", QString::SkipEmptyParts);

	partitionDistroComboBox->clear();

    for(int i = 0; i < list.size(); i++) {
        partitionDistroComboBox->addItem (list.at(i));
    }
	
	loading = false;
}
