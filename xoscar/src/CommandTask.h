/*
 *  Copyright (c) 2007-2008 Oak Ridge National Laboratory, 
 *                          Geoffroy Vallee <valleegr@ornl.gov>
 *                          All rights reserved
 *  This file is part of the xoscar software, part of the OSCAR software.
 *  For license information, see the COPYING file in the top level directory
 *  of the OSCAR source.
 */

/**
 * @file CommandTask.h
 * @brief Defines a class used to specify which command to execute along with
 * the commands arguments if any.
 * @author Robert Babilon
 */
#ifndef COMMANDTASK_H
#define COMMANDTASK_H

#include <QString>
#include <QStringList>

#include "utilities.h"
#include "ThreadUserInterface.h"

namespace xoscar {

class ThreadUserInterface;

class CommandTask
{
public:
    CommandTask();
    CommandTask(CommandId cmd_id, QStringList cmd_args, ThreadUserInterface* threaduser=NULL);
    ~CommandTask();

    CommandId commandTaskId() const;
    void setCommandTaskId(const CommandId cmd_id);

    QStringList commandArgs() const;
    void setCommandArgs(QStringList cmd_args);

    ThreadUserInterface* threadUser() const {return thread_user;}
    void setThreadUser(ThreadUserInterface* const threaduser) {thread_user = threaduser;}

private:
    /**
      * Identifier to the command to execute. These ids are defined in
      * CommandTask.h 
      */
    CommandId command_id;

    /** Parameter(s) of the command to execute */
    QStringList command_args;

    ThreadUserInterface* thread_user;
};

} // namespace xoscar

#endif // COMMANDTASK_H
