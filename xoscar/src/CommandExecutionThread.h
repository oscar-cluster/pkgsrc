/*
 *  Copyright (c) 2007 Oak Ridge National Laboratory, 
 *                     Geoffroy Vallee <valleegr@ornl.gov>
 *                     All rights reserved
 *  This file is part of the xorm software, part of the OSCAR software.
 *  For license information, see the COPYING file in the top level directory
 *  of the OSCAR source.
 */

/**
 * @file CommandExecutionThread.h
 * @brief Defines a class used to execute OPD2 commands in a separate thread.
 * @author Geoffroy Vallee
 *
 * Using Qt4, the main thread, i.e., the application process, is used to
 * display widgets and as a runtime for the GUI. Therefore is the "main
 * thread" is used to do important tasks, the GUI becomes very difficult
 * to use for users (slow refresh for instance).
 * To avoid this issue, we create a separate thread to execute OSCAR commands.
 * Note that the current implementation is not perfect, there is not real
 * protection against concurency, we currently assume that only one action
 * can be made at a time with the GUI.
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

using namespace std;
using namespace redi;

#define INACTIVE                    0
#define GET_LIST_REPO               1
#define GET_LIST_OPKGS              2
#define GET_SETUP_DISTROS           3
#define DO_SYSTEM_SANITY_CHECK      4
#define DO_OSCAR_SANITY_CHECK       5
#define GET_LIST_DEFAULT_REPOS      6
#define DISPLAY_PARTITIONS          7
#define DISPLAY_PARTITION_NODES     9
#define DISPLAY_PARTITION_DISTRO    10
#define ADD_PARTITION               11


/**
 * @namespace xoscar
 * @author Geoffroy Vallee
 * @brief The xoscar namespace gathers all classes needed for XOSCAR.
 */
namespace xoscar {

class CommandExecutionThread : public QThread 
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

namespace xorm {
    class XORM_CommandExecutionThread: public CommandExecutionThread {};
} // namespace xorm

} // namespace xoscar

#endif // COMMANDEXECUTIONTHREAD_H
