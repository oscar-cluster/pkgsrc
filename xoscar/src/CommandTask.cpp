/*
 *  Copyright (c) 2007-2008 Oak Ridge National Laboratory, 
 *                          Geoffroy Vallee <valleegr@ornl.gov>
 *                          All rights reserved
 *  This file is part of the xoscar software, part of the OSCAR software.
 *  For license information, see the COPYING file in the top level directory
 *  of the OSCAR source.
 */

/**
 * @file CommandTask.cpp
 * @brief Actual implementation of the CommandTask class.
 * @author Robert Babilon
 */
#include "CommandTask.h"

using namespace xoscar;

Q_DECLARE_METATYPE(CommandTask::CommandTasks)

CommandTask::CommandTask()
    : command_id(INACTIVE)
    , command_args(QString(""))
{
    // register this enum so it can be used in the queued connections
    qRegisterMetaType<CommandTask::CommandTasks>();
}

CommandTask::CommandTask(CommandTasks cmd_id, QStringList cmd_args)
{
    qRegisterMetaType<CommandTask::CommandTasks>();

    command_id = cmd_id;
    command_args = cmd_args;
}

CommandTask::~CommandTask()
{
}

CommandTask::CommandTasks CommandTask::commandTaskId() const
{
    return command_id;
}

void CommandTask::setCommandTaskId(CommandTasks cmd_id)
{
    command_id = cmd_id;
}

QStringList CommandTask::commandArgs() const
{
    return command_args;
}

void CommandTask::setCommandArgs(QStringList cmd_args)
{
    command_args = cmd_args;
}
