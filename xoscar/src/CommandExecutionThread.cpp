/*
 *  Copyright (c) 2007 Oak Ridge National Laboratory, 
 *                     Geoffroy Vallee <valleegr@ornl.gov>
 *                     All rights reserved
 *  This file is part of the xorm software, part of the OSCAR software.
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

CommandExecutionThread::~CommandExecutionThread()
{
}

/**
  * @author Geoffroy Vallee.
  *
  * Thread initialization function. Used to set data used later on by the
  * thread when running.
  *
  * @param url Repository URL that has to be used by the thread when running.
  * @param m Query mode. For instance, get the list of repositories or 
  *          the list of OSCAR packages for the specified repository. The 
  *          different modes are defined in CommandExecutionThread.h
  */
void CommandExecutionThread::init (int cmd_id, QStringList args)
{
    command_args = args;
    command_id = cmd_id;
    start(QThread::TimeCriticalPriority);
}

/**
  * @author Geoffroy Vallee.
  *
  * Method to actually start the thread execution. Note that the value of the
  * command_id private variable tells us what we need to do.
  * @todo we have some code duplication in that function, that should be 
  * improved.
  */
void CommandExecutionThread::run()
{
    char *ohome = getenv ("OSCAR_HOME");
    QString list_opkgs = "", list_repos = "";
    QString result;

    if (command_id == INACTIVE) {
        return;
    } else if (command_id == GET_LIST_REPO) {
        /* We refresh the list of available repositories */
        const string cmd = (string) ohome 
            + "/scripts/opd2  --non-interactive --list-repos";
        list_repos = get_output_word_by_word (cmd);
        emit (opd_done(list_repos, list_opkgs));
        return;
    } else if (command_id == GET_LIST_OPKGS) {
        /* We update the list of available OPKGs, based on the new repo */
        const string cmd = (string) ohome 
            + "/scripts/opd2  --non-interactive --repo " 
            + command_args.at(0).toStdString ();
        list_opkgs = get_output_word_by_word (cmd);
        emit (opd_done(list_repos, list_opkgs));
        return;
    } else if (command_id == GET_SETUP_DISTROS) {
        /* We update the list of available OPKGs, based on the new repo */
        const string cmd = (string) ohome 
            + "/scripts/oscar-config --list-setup-distros";
        result = get_output_word_by_word (cmd);
        emit (oscar_config_done(result));
        return;
    } else if (command_id == DO_SYSTEM_SANITY_CHECK) {
        const string cmd = (string) ohome + "/scripts/system-sanity";
        result = get_output_line_by_line (cmd);
        emit (sanity_command_done(result));
        return;
    } else if (command_id == DO_OSCAR_SANITY_CHECK) {
        const string cmd = (string) ohome + "/scripts/oscar_sanity_check";
        result = get_output_line_by_line (cmd);
        emit (sanity_command_done(result));
        return;
    } else if (command_id == GET_LIST_DEFAULT_REPOS) {
        const string cmd = (string) ohome 
            + "/scripts/opd2 --non-interactive --list-default-repos";
        result = get_output_line_by_line (cmd);
        emit (thread_terminated(GET_LIST_DEFAULT_REPOS, result));
    } else if (command_id == DISPLAY_PARTITIONS) {
        const string cmd = (string) ohome 
            + "/scripts/oscar --display-partitions";
        result = get_output_line_by_line (cmd);
        emit (thread_terminated(DISPLAY_PARTITIONS, result));
    } else if (command_id == DISPLAY_PARTITION_NODES) {
        const string cmd = (string) ohome 
            + "/scripts/oscar --display-partition-nodes "
            + command_args.at(0).toStdString();
        result = get_output_line_by_line (cmd);
        emit (thread_terminated(DISPLAY_PARTITION_NODES, result));
    } else if (command_id == DISPLAY_PARTITION_DISTRO) {
        const string cmd = (string) ohome 
            + "/scripts/oscar --display-partition-distro "
            + command_args.at(0).toStdString();
        result = get_output_line_by_line (cmd);
        emit (thread_terminated(DISPLAY_PARTITION_DISTRO, result));
    } else if (command_id == ADD_PARTITION) {
        char *ohome = getenv ("OSCAR_HOME");
        string cmd = (string) ohome 
                        + "/scripts/oscar"
                        + " --add-partition " + command_args.at(0).toStdString()
                        + " --cluster oscar"
                        + " --distro " + command_args.at(1).toStdString();
        for (int i=0; i < command_args.size()-2; i++) {
            cmd += " --client ";
            cmd += command_args.at(i).toStdString();
        }
    } else {
        cerr << "ERROR: Unsupported command id: " << command_id << endl;
        exit (-1);
    }
}

QString CommandExecutionThread::get_output_line_by_line (string cmd)
{
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
    ipstream proc(cmd);
    string buf, tmp_list;
    while (proc >> buf) {
        tmp_list += buf;
        tmp_list += " ";
    }
    QString result = tmp_list.c_str();
    return (result);
}


