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

#include <QMetaType>
#include <QString>
#include <QStringList>

namespace xoscar {

class CommandTask
{
public:
 enum CommandTasks {
        INACTIVE = 0,
        GET_LIST_REPO,
        GET_LIST_OPKGS,
        GET_SETUP_DISTROS,
        DO_SYSTEM_SANITY_CHECK,
        DO_OSCAR_SANITY_CHECK,
        GET_LIST_DEFAULT_REPOS,
        DISPLAY_PARTITIONS,
        DISPLAY_PARTITION_NODES,
        DISPLAY_PARTITION_DISTRO,
        ADD_PARTITION,
        DISPLAY_DETAILS_PARTITION_NODES,
        SETUP_DISTRO,
        LIST_UNSETUP_DISTROS,
        DISPLAY_DEFAULT_OSCAR_REPO,
        DISPLAY_DEFAULT_DISTRO_REPO,
        REMOVE_PARTITION,
        DISPLAY_DEFAULT_OPKGS
    };
    
    CommandTask();
    CommandTask(CommandTasks cmd_id, QStringList cmd_args);
    ~CommandTask();

    CommandTasks commandTaskId() const;
    void setCommandTaskId(CommandTasks cmd_id);

    QStringList commandArgs() const;
    void setCommandArgs(QStringList cmd_args);

private:
    /**
      * Identifier to the command to execute. These ids are defined in
      * CommandTask.h 
      */
    CommandTasks command_id;

    /** Parameter(s) of the command to execute */
    QStringList command_args;
};

} // namespace xoscar

#endif // COMMANDTASK_H
