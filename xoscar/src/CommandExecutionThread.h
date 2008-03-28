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

#include "pstream.h"
#include "CommandBuilder.h"

using namespace std;
using namespace redi;

#define INACTIVE                                0
#define GET_LIST_REPO                           1
#define GET_LIST_OPKGS                          2
#define GET_SETUP_DISTROS                       3
#define DO_SYSTEM_SANITY_CHECK                  4
#define DO_OSCAR_SANITY_CHECK                   5
#define GET_LIST_DEFAULT_REPOS                  6
#define DISPLAY_PARTITIONS                      7
#define DISPLAY_PARTITION_NODES                 9
#define DISPLAY_PARTITION_DISTRO                10
#define ADD_PARTITION                           11
#define DISPLAY_DETAILS_PARTITION_NODES         12
#define SETUP_DISTRO                            13
#define LIST_UNSETUP_DISTROS                    14
#define DISPLAY_DEFAULT_OSCAR_REPO              15
#define DISPLAY_DEFAULT_DISTRO_REPO             16

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
 */
class CommandExecutionThread : public QThread, public xoscar::CommandBuilder
{
    Q_OBJECT

public:
    CommandExecutionThread(QObject *parent = 0);
    ~CommandExecutionThread();
    void init (int, QStringList);
    void run();

signals:
    virtual void opd_done (QString, QString);
    virtual void oscar_config_done (QString);
    virtual void sanity_command_done (QString);
    /** This signal is a generic signal emitted when the thread ends.
      * @param command_id Unique identifier of the executed command.
      * @param result Result of the executed command.
      */
    virtual void thread_terminated (int command_id, QString result);

protected:

private:
    /** Parameter of the command to execute */
    QStringList command_args;
    /**
      * Identifier to the command to execute. These ids are defined in
      * CommandExecutionThread.h 
      */
    int command_id;

    QString get_output_line_by_line (string);
    QString get_output_word_by_word (string);
};

} // namespace xoscar

#endif // COMMANDEXECUTIONTHREAD_H
