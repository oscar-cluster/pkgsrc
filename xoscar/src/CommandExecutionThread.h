/*
 *  Copyright (c) 2007-2008 Oak Ridge National Laboratory, 
 *                          Geoffroy Vallee <valleegr@ornl.gov>
 *                          All rights reserved
 *  This file is part of the xoscar software, part of the OSCAR software.
 *  For license information, see the COPYING file in the top level directory
 *  of the OSCAR source.
 */

/**
 * @file CommandExecutionThread.h
 * @brief Defines a class used to execute OPD2 commands in a separate thread.
 * @author Geoffroy Vallee
 */

#ifndef COMMANDEXECUTIONTHREAD_H
#define COMMANDEXECUTIONTHREAD_H

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <fstream>
#include <unistd.h>

#include <QThread>
#include <QWaitCondition>
#include <QStringList>
#include <QMutex>

#include "pstream.h"
#include "CommandBuilder.h"
#include "CommandTask.h"
#include "utilities.h"

using namespace std;
using namespace redi;

/**
 * @namespace xoscar
 * @author Geoffroy Vallee
 * @brief The xoscar namespace gathers all classes needed for XOSCAR.
 */
namespace xoscar {

/**
 * @author Geoffroy Vallee
 *
 *  *
 * Using Qt4, the main thread, i.e., the application process, is used to
 * display widgets and as a runtime for the GUI. Therefore is the "main
 * thread" is used to do important tasks, the GUI becomes very difficult
 * to use for users (slow refresh for instance).
 * To avoid this issue, we create a separate thread to execute OSCAR commands.
 * Note that the current implementation is not perfect: every time we create a
 * thread for the execution of the command and explicitely wait for the end of
 * the thread if you want to execute several consecutive command in the thread.
 * If you do not do so, that can be a big mess (thus do it only if you know
 * exactly what you are doing!). An example of "normal" code is:
 * \code
 *    command_thread.init (DO_OSCAR_SANITY_CHECK, QStringList(""));
 *    command_thread.wait();
 * \endcode
 * 
 * The new implementation of this class as of r7135 will allow multiple calls to
 * init() and queue the commands up in a QList. Correct implementation requires
 * some work on the caller side (connecting to the thread_terminated() and
 * finished() signals).
 */
class CommandExecutionThread : public QThread, public xoscar::CommandBuilder
{
    Q_OBJECT

public:
    /**
     * Flags to inidicate the sate of the thread.
     */
    enum StatusFlags {Normal, // thread is in normal state
                      Sleeping, // thread is sleeping or about to sleep
                      CancelRequest // not yet implemented
                     };

    CommandExecutionThread(QObject *parent = 0);
    ~CommandExecutionThread();

    void init (xoscar::CommandId, QStringList);
    void init(CommandTask cmd_task);
    void init(QList<CommandTask> cmd_tasks);

    void wakeThread();
    bool isEmpty();
    QList<CommandTask> commandTasks() const;

signals:
    virtual void opd_done (QString, QString);
    virtual void sanity_command_done (QString);
    /** This signal is a generic signal emitted when the thread ends.
      * @param command_id Unique identifier of the executed command.
      * @param result Result of the executed command.
      * @param threadUser Class implementing the ThreadUserInterface to handle the results.
      */
    virtual void thread_terminated (xoscar::CommandId command_id, QString result, ThreadUserInterface* threadUser);

protected:
    void run();
    void run_command(CommandTask &task);

    void appendCommandTask(CommandTask cmd_task);
    void appendCommandTask(QList<CommandTask> cmd_tasks);

private:
    /**
     * Mutex used to lock the QList of CommandTasks.
     */
    QMutex commandTasksLocker;
    /**
     * Mutex used by the QWaitCondition "resultProcessed".
     */
    QMutex statusFlagMutex;
    /**
     * Causes this worker thread to wait until signaled to continue.
     * This wait condition is to allow the main thread in the GUI to process the
     * result and inform the worker thread when the GUI is ready to process more
     * data from the worker thread.
     */
    QWaitCondition resultProcessed;
    /**
     * The list of CommandTask to execute
     */
    QList<CommandTask> command_tasks;
    /**
     * Indicates the status of this thread.
     * Normal - thread is doing it's job
     * Sleeping - thread is sleeping or will be sleeping very soon
     * CancelRequest - not yet implemented (would indicate that a request to
     * cancel the current task was made; stop the thread ASAP)
     */
    StatusFlags status_flag;

    void resetStatusFlag();
    void setSleepFlag();
    void sleepThread();

    QString get_output_line_by_line (string);
    QString get_output_word_by_word (string);
};

} // namespace xoscar

#endif // COMMANDEXECUTIONTHREAD_H
