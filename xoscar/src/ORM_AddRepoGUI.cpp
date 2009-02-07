/*
 *  Copyright (c) 2007 Oak Ridge National Laboratory, 
 *                     Geoffroy Vallee <valleegr@ornl.gov>
 *                     All rights reserved
 *  This file is part of the xoscar software, part of the OSCAR software.
 *  For license information, see the COPYING file in the top level directory
 *  of the OSCAR source.
 */

/**
 * @file ORM_AddRepoGUI.cpp
 * @brief Actual implementation of the ORMAddRepoDialog class.
 * @author Geoffroy Vallee
 */

#include "ORM_AddRepoGUI.h"

using namespace xoscar;

/**
 * @author Geoffroy Vallee.
 *
 * Class constructor: the widget gathers (i) a text box when the user can enter 
 * the repository URL; and (ii) a combo box where the user specify for which 
 * distribution the repository is made for. For that, it initializes the widget,
 * gets the list of setup distros in order to let user specify what distribution
 * the repository targets when a new repo is added
 *
 * @todo When we get the list of setup distros, if the list is empty we should
 * display a dialog box that says that unfortunately no distribution is 
 * currently setup.
 * @todo When a dialog will be implemented to notify to users that no 
 * distribution has been setup, we should also pop up a new widget for the 
 * configuration of OSCAR (oscar-config tftpboot related stuff).
 */
ORMAddRepoDialog::ORMAddRepoDialog(ThreadHandlerInterface* handler, QDialog *parent) 
    : QDialog (parent) 
    , ThreadUserInterface(handler)
{
    QString list_distros;

    setupUi(this);

// TODO this could be set via a list of commands to "listen" to. the main window already
// calls GET_SETUP_DISTROS in its constructor so this is uncessary. plus, this GET_SETUP_DISTROS
// might need to happen each time this is shown...
// also causes seg fault b/c of initialization sequence of objects
//      threadHandler->enqueue_command_task(CommandTask(xoscar::GET_SETUP_DISTROS, QStringList(""), 
//                                                      dynamic_cast<ThreadUserInterface*>(this)));
}

ORMAddRepoDialog::~ORMAddRepoDialog ()
{
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
int ORMAddRepoDialog::handle_thread_result (xoscar::CommandId command_id, QString result)
{
     if (command_id == xoscar::GET_SETUP_DISTROS) {
        /* Once we have the list, we update the widget */
        QStringList list = result.split(" ");
        for(int i = 0; i < list.size(); i++) {
            distroComboBox->addItem (list.at(i));
        }
        this->update();
    }
    return 0;
}
