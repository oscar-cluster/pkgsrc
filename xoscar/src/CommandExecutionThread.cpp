/*
 *  Copyright (c) 2007-2008 Oak Ridge National Laboratory, 
 *                          Geoffroy Vallee <valleegr@ornl.gov>
 *                          All rights reserved
 *  This file is part of the xoscar software, part of the OSCAR software.
 *  For license information, see the COPYING file in the top level directory
 *  of the OSCAR source.
 */

/**
 * @file CommandExecutionThread.cpp
 * @brief Actual implementation of the CommandExecutionThread class.
 * @author Geoffroy Vallee
 */

#include "CommandExecutionThread.h"

using namespace xoscar;

CommandExecutionThread::CommandExecutionThread(QObject *parent) 
    : QThread (parent)
{
}

/**
 *  @author Robert Babilon
 *
 *  To ensure the thread is destroyed properly, the destructor calls wakeOne on
 *  the QWaitCondition (in case it were stuck sleeping for some reason) and then
 *  a call to wait() to allow the thread to exit from run().
 */
CommandExecutionThread::~CommandExecutionThread()
{
    if(this->isRunning()) {
        cout << "WARNING: Thread is still running! Trying to safely terminate the thread..." << endl;
    }
    resultProcessed.wakeOne();
    wait(500);

    if(this->isRunning()) {
        cout << "WARNING: Unable to safely terminate thread!" << endl;
    } else {
        cout << "Thread was terminated safely." << endl;
    }
}

/**
  * @author Geoffroy Vallee.
  *
  * Thread initialization function. Used to set data used later on by the
  * thread when running.
  *
  * @param args Repository URL that has to be used by the thread when running.
  * @param cmd_id Query mode. For instance, get the list of repositories or 
  *          the list of OSCAR packages for the specified repository. The 
  *          different modes are defined in CommandTask.h
  */
void CommandExecutionThread::init (xoscar::CommandId cmd_id, QStringList args)
{
    appendCommandTask(CommandTask(cmd_id, args));

    start(QThread::TimeCriticalPriority);
}

/**
 *  @author Robert Babilon
 *
 *  Overloaded thread initialization function. Adds the given CommandTask to the
 *  list and starts the thread.
 *
 *  @param cmd_task A CommandTask to append and run.
 */
void CommandExecutionThread::init(CommandTask cmd_task)
{
    appendCommandTask(cmd_task);
    start(QThread::TimeCriticalPriority);
}

/**
 *  @author Robert Babilon
 *
 *  Overloaded thread initialization function. Adds the given CommandTask(s) to the
 *  list and starts the thread.
 *
 *  @param cmd_tasks A QList of CommandTask to append and run.
 */
void CommandExecutionThread::init(QList<CommandTask> cmd_tasks)
{
    appendCommandTask(cmd_tasks);

    start(QThread::TimeCriticalPriority);
}

/**
 *  @author Robert Babilon
 *
 *  Append a CommandTask to this thread's own list of CommandTask.
 *  This method locks the list before accessing it.
 *
 *  @param cmd_task A CommandTask to append.
 */
void CommandExecutionThread::appendCommandTask(CommandTask cmd_task)
{
    commandTasksLocker.lock();
    command_tasks += cmd_task;
    commandTasksLocker.unlock();
}

/**
 *  @author Robert Babilon
 *
 *  Append a list of CommandTask to this thread's own list of CommandTask.
 *  This method locks the list before accessing it.
 *
 *  @param cmd_tasks A QList of CommandTask to append.
 */
void CommandExecutionThread::appendCommandTask(QList<CommandTask> cmd_tasks)
{
    commandTasksLocker.lock();
    command_tasks += cmd_tasks;
    commandTasksLocker.unlock();
}

/**
 *  @author Robert Babilon
 *
 *  Function to check if the QList of CommandTask is empty.
 *  @return true if the list is empty; otherwise false.
 */
bool CommandExecutionThread::isEmpty()
{
    bool empty = false;

    commandTasksLocker.lock();
    empty = command_tasks.isEmpty();
    commandTasksLocker.unlock();

    return empty;
}

/**
 *  @author Robert Babilon
 *
 *  Method that starts the threads execution. This is a QThread method that must
 *  be reimplemented in order to use QThread.
 *
 *  The basic functionality of this method is to grab the first command from the
 *  QList of CommandTask, set the status_flag to Sleeping, run the command,
 *  sleep until the calling thread (GUI) finishes with the command's results,
 *  and finally reset the status_flag.
 *
 *  Note that setting the status_flag can be done in multiple locations. Please
 *  see the comment for setSleepFlag() and sleepThread() for more information.
 */
void CommandExecutionThread::run()
{
    CommandTask current_task;
    bool empty = false;

    commandTasksLocker.lock();
    empty = command_tasks.isEmpty();
    if(!empty) {
        current_task = command_tasks.takeFirst();
    }
    commandTasksLocker.unlock();

    if(empty) {
        cout << "no more tasks to execute" << endl;
    }
    else {
        setSleepFlag();
        run_command(current_task);
        sleepThread();
    }

    // reset status to Normal just before we exit run()
    resetStatusFlag();
}

/**
 *  @author Robert Babilon
 *
 *  Call at some point before emitting the thread_terminated signal.
 *
 *  This procedure sets the status_flag to Sleeping. This thread is not
 *  necessarily sleeping right after returning from this function so checking
 *  the flag from outside this thread may be misleading information. The flag's
 *  purpose is to indicate that the thread should sleep/wait after running a
 *  command (i.e. after emitting the thread_terminated signal).
 *  setSleepFlag() can be called just before run_command(), just before emitting
 *  the thread_terminated signal, or even just inside run_command(). The only
 *  important thing to remember is to call it before emitting that signal.
 *  Please see the comment for sleepThread() for more information.
 */
void CommandExecutionThread::setSleepFlag()
{
    // only set to Sleeping if current status is Normal. if status were
    // CancelRequest, then the next check for cancelRequested() will stop this
    // thread.
    statusFlagMutex.lock();
    if(status_flag == Normal) {
        status_flag = Sleeping;
    }
    statusFlagMutex.unlock();
}

/**
 *  @author Robert Babilon
 *
 *  Call after finishing a command; a point where this thread should rest a
 *  while until the caller (GUI thread) is ready to process another command's
 *  result.
 *  This procedure checks if the status_flag is still Sleeping and if so sets
 *  the wait condition. This check is necessary in certain cases where the
 *  command executed executes so fast that the thread_terminated signal is
 *  emited and processed by the GUI thread before entering this function.
 *  Perhaps if this procedure were called directly after the emit() call, the
 *  probably would not exist; however, this check is miniscule and helps keep
 *  the bugs out.
 *  Please see the comment for setSleepFlag() for more information.
 */
void CommandExecutionThread::sleepThread()
{
    statusFlagMutex.lock();
    if(status_flag == Sleeping) {
        // if status_flag is CancelRequest or Normal, we do not want to call
        // wait() because the GUI expects this thread to stop or continue
        resultProcessed.wait(&statusFlagMutex);
    }
    statusFlagMutex.unlock();
}

/**
 *  @author Robert Babilon
 *
 *  Call this procedure when ready to handle the next command. i.e. just
 *  before returning from handle_thread_result().
 */
void CommandExecutionThread::wakeThread()
{
    statusFlagMutex.lock();
    if(status_flag == Sleeping) {
        status_flag = Normal;
    }
    statusFlagMutex.unlock();
    
    resultProcessed.wakeOne();
}

/**
 *  @author Robert Babilon
 *
 *  Call only when re/starting the thread. This method should remain at least
 *  protected. No outsiders need access to this method.
 *  Currently called just before returning from run().
 */
void CommandExecutionThread::resetStatusFlag()
{
    statusFlagMutex.lock();
    status_flag = Normal;
    statusFlagMutex.unlock();
}

/**
  * @author Geoffroy Vallee.
  *
  * Method to actually run a specific command.
  * Each command emits the thread_terminated signal in order to ensure the
  * caller (GUI thread) knows that the command has executed and thus allowing
  * the caller inform this thread that it may continue execution.
  *
  * @todo we should only use the thread_terminated signal, not the old 
  *       individual signals (such as opd_done).
  */
void CommandExecutionThread::run_command(CommandTask &task)
{
    xoscar::CommandId command_id = task.commandTaskId();
    QStringList command_args = task.commandArgs();
    char *ohome = getenv ("OSCAR_HOME");
    QString result = "";

    if (command_id == xoscar::INACTIVE) {
        emit (thread_terminated(command_id, result, task.threadUser()));
        return;
    } else if (command_id == xoscar::GET_LIST_REPO) {
        /* We refresh the list of available repositories */
        const string cmd = build_cmd ((string) ohome 
            + "/scripts/opd2  --non-interactive --list-repos");
        result = get_output_word_by_word (cmd);
        emit (thread_terminated(command_id, result, task.threadUser()));
    } else if (command_id == xoscar::GET_LIST_OPKGS) {
        /* We update the list of available OPKGs, based on the new repo */
        const string cmd = build_cmd ((string) ohome 
            + "/scripts/opd2  --non-interactive --repo " 
            + command_args.at(0).toStdString ());
        result = get_output_word_by_word (cmd);
        emit (thread_terminated(command_id, result, task.threadUser()));
    } else if (command_id == xoscar::GET_SETUP_DISTROS) {
        /* We update the list of available OPKGs, based on the new repo */
        const string cmd = build_cmd ((string) ohome 
            + "/scripts/oscar-config --list-setup-distros");
        result = get_output_word_by_word (cmd);
        emit (thread_terminated(command_id, result, task.threadUser()));
    } else if (command_id == xoscar::DO_SYSTEM_SANITY_CHECK) {
        const string cmd = build_cmd ((string) ohome
            + "/scripts/system-sanity");
        result = get_output_line_by_line (cmd);
        emit (thread_terminated(command_id, result, task.threadUser()));
    } else if (command_id == xoscar::DO_OSCAR_SANITY_CHECK) {
        const string cmd = build_cmd ((string) ohome 
           + "/scripts/oscar_sanity_check");
        result = get_output_line_by_line (cmd);
        emit (thread_terminated(command_id, result, task.threadUser()));
    } else if (command_id == xoscar::GET_LIST_DEFAULT_REPOS) {
        const string cmd = build_cmd ((string) ohome 
            + "/scripts/opd2 --non-interactive --list-default-repos");
        result = get_output_line_by_line (cmd);
        cout << "Default repos: " << result.toStdString() << endl;
        emit (thread_terminated(command_id, result, task.threadUser()));
    } else if (command_id == xoscar::DISPLAY_PARTITIONS) {
        const string cmd = build_cmd ((string) ohome 
            + "/scripts/oscar --display-partitions "
            + command_args.at(0).toStdString());
        result = get_output_line_by_line (cmd);
        emit (thread_terminated(command_id, result, task.threadUser()));
    } else if (command_id == xoscar::DISPLAY_PARTITION_NODES) {
        const string cmd = build_cmd ((string) ohome 
            + "/scripts/oscar --display-partition-nodes "
            + command_args.at(0).toStdString());
        result = get_output_line_by_line (cmd);
        emit (thread_terminated(command_id, result, task.threadUser()));
    } else if (command_id == xoscar::DISPLAY_PARTITION_DISTRO) {
        const string cmd = build_cmd ((string) ohome 
            + "/scripts/oscar --display-partition-distro "
            + command_args.at(0).toStdString());
        result = get_output_line_by_line (cmd);
        emit (thread_terminated(command_id, result, task.threadUser()));
    } else if (command_id == xoscar::ADD_PARTITION) {
        string cmd = (string) ohome 
                        + "/scripts/oscar"
                        + " --add-partition " + command_args.at(0).toStdString()
                        + " --cluster oscar"
                        + " --distro " + command_args.at(1).toStdString();
        for (int i=2; i < command_args.size(); i++) {
            cmd += " --client ";
            cmd += command_args.at(i).toStdString();
        }
        result = get_output_line_by_line (build_cmd (cmd));
        emit (thread_terminated(command_id, result, task.threadUser()));
    } else if (command_id == xoscar::REMOVE_PARTITION) {
        result = "ERROR: xoscar::REMOVE_PARTITION not yet implemented.";
        cout << result.toStdString() << endl;
        emit (thread_terminated(command_id, result, task.threadUser()));
    } else if (command_id == xoscar::DISPLAY_DETAILS_PARTITION_NODES) {
        const string cmd = build_cmd ((string) ohome 
            + "/scripts/oscar --display-partition-nodes "
            + command_args.at(0).toStdString()
            + " -v");
        result = get_output_line_by_line (cmd);
        emit (thread_terminated(command_id, result, task.threadUser()));
    } else if (command_id == xoscar::SETUP_DISTRO) {
        const string cmd = build_cmd ((string) ohome 
                + "/scripts/oscar-config --setup-distro "
                + command_args.at(0).toStdString()
                + " --use-distro-repo "
                + command_args.at(1).toStdString()
                + " --use-oscar-repo "
                + command_args.at(2).toStdString());
        result = get_output_line_by_line (cmd);
        emit (thread_terminated(command_id, result, task.threadUser()));
    } else if (command_id == xoscar::LIST_UNSETUP_DISTROS) {
        const string cmd = build_cmd ((string) ohome
                + "/scripts/oscar-config --list-unsetup-distros");
        result = get_output_word_by_word (cmd);
        emit (thread_terminated(command_id, result, task.threadUser()));
    } else if (command_id == xoscar::DISPLAY_DEFAULT_OSCAR_REPO) {
        const string cmd = build_cmd ((string) ohome 
                + "/scripts/oscar-config --display-default-oscar-repo "
                + command_args.at(0).toStdString());
        result = get_output_word_by_word (cmd);
        emit (thread_terminated(command_id, result, task.threadUser()));
    } else if (command_id == xoscar::DISPLAY_DEFAULT_DISTRO_REPO) {
        const string cmd = build_cmd ((string) ohome
                + "/scripts/oscar-config --display-default-distro-repo "
                + command_args.at(0).toStdString());
        result = get_output_word_by_word (cmd);
        emit (thread_terminated(command_id, result, task.threadUser()));
    } else if (command_id == xoscar::DISPLAY_DEFAULT_OPKGS) {
        const string cmd = build_cmd ((string) ohome
                + "/scripts/opd2 --non-interactive --default-opkgs "
                + command_args.at(0).toStdString());
        result = get_output_line_by_line (cmd);
        emit (thread_terminated(command_id, result, task.threadUser()));
    }
    // We ignore other command IDs
}

QString CommandExecutionThread::get_output_line_by_line (string cmd)
{
    cout << "Executing: " << cmd << endl;
    pstream proc (cmd, pstreambuf::pstdout);
    string s, buf;
    while (std::getline(proc, s)) {
        buf += s;
        cout << buf << endl;
        buf += "\n";
    }
    QString res = buf.c_str();
    return (res);
}

/**
 * We get each word from the output and put them in a string, each word being
 * separated by a space.
 */
QString CommandExecutionThread::get_output_word_by_word (string cmd)
{
    cout << "Executing: " << cmd << endl;
    ipstream proc(cmd);
    string buf, tmp_list;
    while (proc >> buf) {
        tmp_list += buf;
        tmp_list += " ";
    }
    QString result = tmp_list.c_str();
    return (result);
}


