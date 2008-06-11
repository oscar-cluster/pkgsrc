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
  *
  * @todo we should only use the thread_terminated signal, not the old 
  *       individual signals (such as opd_done).
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
        const string cmd = build_cmd ((string) ohome 
            + "/scripts/opd2  --non-interactive --list-repos");
        list_repos = get_output_word_by_word (cmd);
        emit (opd_done(list_repos, list_opkgs));
    } else if (command_id == GET_LIST_OPKGS) {
        /* We update the list of available OPKGs, based on the new repo */
        const string cmd = build_cmd ((string) ohome 
            + "/scripts/opd2  --non-interactive --repo " 
            + command_args.at(0).toStdString ());
        list_opkgs = get_output_word_by_word (cmd);
        emit (opd_done(list_repos, list_opkgs));
    } else if (command_id == GET_SETUP_DISTROS) {
        /* We update the list of available OPKGs, based on the new repo */
        const string cmd = build_cmd ((string) ohome 
            + "/scripts/oscar-config --list-setup-distros");
        result = get_output_word_by_word (cmd);
        emit (oscar_config_done(result));
        emit (thread_terminated(GET_SETUP_DISTROS, result));
    } else if (command_id == DO_SYSTEM_SANITY_CHECK) {
        const string cmd = build_cmd ((string) ohome
            + "/scripts/system-sanity");
        result = get_output_line_by_line (cmd);
        emit (sanity_command_done(result));
    } else if (command_id == DO_OSCAR_SANITY_CHECK) {
        const string cmd = build_cmd ((string) ohome 
           + "/scripts/oscar_sanity_check");
        result = get_output_line_by_line (cmd);
        emit (sanity_command_done(result));
    } else if (command_id == GET_LIST_DEFAULT_REPOS) {
        const string cmd = build_cmd ((string) ohome 
            + "/scripts/opd2 --non-interactive --list-default-repos");
        result = get_output_line_by_line (cmd);
        cout << "Default repos: " << result.toStdString() << endl;
        emit (thread_terminated(GET_LIST_DEFAULT_REPOS, result));
    } else if (command_id == DISPLAY_PARTITIONS) {
        const string cmd = build_cmd ((string) ohome 
            + "/scripts/oscar --display-partitions "
            + command_args.at(0).toStdString());
        result = get_output_line_by_line (cmd);
        emit (thread_terminated(DISPLAY_PARTITIONS, result));
    } else if (command_id == DISPLAY_PARTITION_NODES) {
        const string cmd = build_cmd ((string) ohome 
            + "/scripts/oscar --display-partition-nodes "
            + command_args.at(0).toStdString());
        result = get_output_line_by_line (cmd);
        emit (thread_terminated(DISPLAY_PARTITION_NODES, result));
    } else if (command_id == DISPLAY_PARTITION_DISTRO) {
        const string cmd = build_cmd ((string) ohome 
            + "/scripts/oscar --display-partition-distro "
            + command_args.at(0).toStdString());
        result = get_output_line_by_line (cmd);
        emit (thread_terminated(DISPLAY_PARTITION_DISTRO, result));
    } else if (command_id == ADD_PARTITION) {
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
    } else if (command_id == DISPLAY_DETAILS_PARTITION_NODES) {
        const string cmd = build_cmd ((string) ohome 
            + "/scripts/oscar --display-partition-nodes "
            + command_args.at(0).toStdString()
            + " -v");
        result = get_output_line_by_line (cmd);
        emit (thread_terminated(DISPLAY_DETAILS_PARTITION_NODES, result));
    } else if (command_id == SETUP_DISTRO) {
        const string cmd = build_cmd ((string) ohome 
                + "/scripts/oscar-config --setup-distro "
                + command_args.at(0).toStdString()
                + " --use-distro-repo "
                + command_args.at(1).toStdString()
                + " --use-oscar-repo "
                + command_args.at(2).toStdString());
        result = get_output_line_by_line (cmd);
        emit (thread_terminated(SETUP_DISTRO, result));
    } else if (command_id == LIST_UNSETUP_DISTROS) {
        const string cmd = build_cmd ((string) ohome
                + "/scripts/oscar-config --list-unsetup-distros");
        result = get_output_word_by_word (cmd);
        emit (thread_terminated(LIST_UNSETUP_DISTROS, result));
    } else if (command_id == DISPLAY_DEFAULT_OSCAR_REPO) {
        const string cmd = build_cmd ((string) ohome 
                + "/scripts/oscar-config --display-default-oscar-repo "
                + command_args.at(0).toStdString());
        result = get_output_word_by_word (cmd);
        emit (thread_terminated(DISPLAY_DEFAULT_OSCAR_REPO, result));
    } else if (command_id == DISPLAY_DEFAULT_DISTRO_REPO) {
        const string cmd = build_cmd ((string) ohome
                + "/scripts/oscar-config --display-default-distro-repo "
                + command_args.at(0).toStdString());
        result = get_output_word_by_word (cmd);
        emit (thread_terminated(DISPLAY_DEFAULT_DISTRO_REPO, result));
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


