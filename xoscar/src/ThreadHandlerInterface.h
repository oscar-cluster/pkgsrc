/*
 *  Copyright (c) 2007-2008 Oak Ridge National Laboratory,
 *                          Geoffroy Vallee <valleegr@ornl.gov>
 *                          All rights reserved
 *  This file is part of the xoscar software, part of the OSCAR software.
 *  For license information, see the COPYING file in the top level directory
 *  of the OSCAR source.
 */

/**
 * @file ThreadHandlerInterface.h
 * @brief Defines the thread handler interface.
 * @author Robert Babilon
 */

#ifndef THREADHANDLERINTERFACE_H
#define THREADHANDLERINTERFACE_H

#include "CommandTask.h"
#include "utilities.h"
#include "ThreadUserInterface.h"

namespace xoscar {

class CommandTask;
class ThreadUserInterface;

class ThreadHandlerInterface
{
public:
    ThreadHandlerInterface();
    ~ThreadHandlerInterface();

    virtual void enqueue_command_task(CommandTask task, QString message="") = 0;

signals:
    virtual void command_thread_tasks_done() = 0;
};

} // namespace xoscar

#endif // THREADHANDLERINTERFACE_H
