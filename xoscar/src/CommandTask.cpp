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

CommandTask::CommandTask()
    : command_id(xoscar::INACTIVE)
    , command_args(QString(""))
    , thread_user(NULL)
{
}

CommandTask::CommandTask(CommandId cmd_id, QStringList cmd_args, ThreadUserInterface* threaduser)
{
    command_id = cmd_id;
    command_args = cmd_args;
    thread_user = threaduser;
}

CommandTask::~CommandTask()
{
}
/**
 * @author Robert Babilon
 *
 * Method to get the command id
 */
xoscar::CommandId CommandTask::commandTaskId() const
{
    return command_id;
}

/**
 * @author Robert Babilon
 *
 * Method to set the command task id
 */
void CommandTask::setCommandTaskId(const CommandId cmd_id)
{
    command_id = cmd_id;
}

/**
 * @author Robert Babilon
 *
 * Method to get the command's arguments
 */
QStringList CommandTask::commandArgs() const
{
    return command_args;
}

/**
 * @author Robert Babilon
 *
 * Method to set the command's arguments
 * @param cmd_args The arguments to set.
 */
void CommandTask::setCommandArgs(QStringList cmd_args)
{
    command_args = cmd_args;
}
