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

#include <QList>

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
    virtual void append_threaduser_with_commandid(ThreadUserInterface* const threaduser, const xoscar::CommandId command_id);
    virtual void remove_threaduser_from_commandid(ThreadUserInterface* const threaduser, const xoscar::CommandId command_id);

signals:
    virtual void command_thread_tasks_done() = 0;

public slots:
    virtual int handle_thread_result (xoscar::CommandId, const QString, ThreadUserInterface*) = 0;

protected:
    virtual void notify_associated_threadusers(ThreadUserInterface* const skipThreaduser, 
                                               const xoscar::CommandId command_id, const QString& result);

    /**
     * This array of QList holds the thread users that want to be notified
     * about a command defined in xoscar::CommandId (excluding xoscar::LAST_CMD_ID).
     * The last one is used to initialize the size of the array. b/c the 
     * command values start at 0, their exact value is used to index into 
     * the array and access the QList of thread users.
     */
    QList<ThreadUserInterface*> associated_threadusers[xoscar::LAST_CMD_ID];
};

} // namespace xoscar

#endif // THREADHANDLERINTERFACE_H
