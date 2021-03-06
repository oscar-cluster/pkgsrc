/*
 *  Copyright (c) 2007 Oak Ridge National Laboratory, 
 *                     Geoffroy Vallee <valleegr@ornl.gov>
 *                     All rights reserved
 *  This file is part of the xoscar software, part of the OSCAR software.
 *  For license information, see the COPYING file in the top level directory
 *  of the OSCAR source.
 */

/**
 * @file ORM_AddDistroGUI.cpp
 * @brief Actual implementation of the ORMAddDistroDialog class.
 * @author Geoffroy Vallee
 */

#include "ORM_AddDistroGUI.h"

using namespace xoscar;

/**
 * @author Geoffroy Vallee.
 *
 */
ORMAddDistroDialog::ORMAddDistroDialog(ThreadHandlerInterface* handler, QWidget *parent) 
    : QDialog (parent) 
    , ThreadUserInterface(handler)
{
    setupUi(this);
    connect (addDistroOkButton, SIGNAL(clicked()), 
             this, SLOT(newDistroSelected()));
    connect(listNonSetupDistrosWidget, SIGNAL(itemSelectionChanged ()),
            this, SLOT(refresh_repos_url()));
}

ORMAddDistroDialog::~ORMAddDistroDialog ()
{
}
/*
void ORMAddDistroDialog::destroy()
{
    cout << "toto" << endl;
    this->close();
}*/

void ORMAddDistroDialog::newDistroSelected ()
{
    QString distro;
//     char *ohome = getenv ("OSCAR_HOME");

    /* We get the selected distro. Note that the selection widget supports 
       currently a unique selection. */
    QList<QListWidgetItem *> list = listNonSetupDistrosWidget->selectedItems();
    QListIterator<QListWidgetItem *> i(list);
    while (i.hasNext()) {
        distro= i.next()->text();
    }
    QStringList args;
    args << distro << distroRepoEdit->text() << oscarRepoEdit->text();
    threadHandler->enqueue_command_task(CommandTask(xoscar::SETUP_DISTRO, args));

/*
    cout << "Command to execute: " << cmd << endl;
    system (cmd.c_str());*/
    this->close();
    emit (refreshListDistros());
}

/**
 * @todo Replace the explicit call of OSCAR commands with the execution thread
 *       stuff.
 */
void ORMAddDistroDialog::refresh_list_distros() {
    threadHandler->enqueue_command_task(CommandTask(xoscar::LIST_UNSETUP_DISTROS, QStringList("")));
}

/**
 * @author Geoffroy Vallee
 *
 * @todo We need here to have an option for oscar-config that gives the default
 * oscar repo, and the default distro repo. Without these two capabilities, it
 * is not possible to fill up empty widgets.
 * @todo Replace the explicit call of OSCAR commands with the execution thread
 *       stuff.
 */
void ORMAddDistroDialog::refresh_repos_url()
{
    QString distro;

    /* We get the selected distro. Note that the selection widget supports 
       currently a unique selection. */
    QList<QListWidgetItem *> list = listNonSetupDistrosWidget->selectedItems();
    QListIterator<QListWidgetItem *> i(list);
    while (i.hasNext()) {
        distro = i.next()->text();
    }

    threadHandler->enqueue_command_task(CommandTask(xoscar::DISPLAY_DEFAULT_DISTRO_REPO, QStringList(distro)));
    threadHandler->enqueue_command_task(CommandTask(xoscar::DISPLAY_DEFAULT_OSCAR_REPO, QStringList(distro)));
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
 *  @param result Holds the return value of the command.
 */
int ORMAddDistroDialog::handle_thread_result (xoscar::CommandId command_id, QString result)
{
     if (command_id == xoscar::LIST_UNSETUP_DISTROS) {
        /* Once we have the list, we update the widget */
        this->listNonSetupDistrosWidget->clear();
        QStringList list = result.split(" ");
        for(int i = 0; i < list.size(); i++) {
            this->listNonSetupDistrosWidget->addItem (list.at(i));
        }
        this->update();
    } else if (command_id == xoscar::DISPLAY_DEFAULT_OSCAR_REPO) {
        oscarRepoEdit->setText(result);
    } else if (command_id == xoscar::DISPLAY_DEFAULT_DISTRO_REPO) {
        distroRepoEdit->setText(result);
    }
    // We ignore other command IDs
    return 0;
}
