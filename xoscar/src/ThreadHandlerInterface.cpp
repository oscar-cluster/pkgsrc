/*
 *  Copyright (c) 2007-2008 Oak Ridge National Laboratory,
 *                          Geoffroy Vallee <valleegr@ornl.gov>
 *                          All rights reserved
 *  This file is part of the xoscar software, part of the OSCAR software.
 *  For license information, see the COPYING file in the top level directory
 *  of the OSCAR source.
 */

/**
 * @file XOSCAR_TabWidgetInterface.cpp
 * @brief Implementation of the ThreadHandlerInterface.
 * @author Robert Babilon
 */

#include "ThreadHandlerInterface.h"

using namespace xoscar;

ThreadHandlerInterface::ThreadHandlerInterface()
{
}

ThreadHandlerInterface::~ThreadHandlerInterface()
{
}

/**
 * @author Robert Babilon
 *
 * Associates the given thread user with the given command id. 
 * 
 * @param threaduser The ThreadUserInterface to associate the given command id with.
 * @param command_id The xoscar::CommandId to associate the thread user with.
 */
void ThreadHandlerInterface::append_threaduser_with_commandid(ThreadUserInterface* const threaduser, const xoscar::CommandId command_id)
{
    if(command_id >= LAST_CMD_ID) {
        cerr << "ERROR: LAST_CMD_ID or greater are invalid!" << endl;
        return;
    }
    if(threaduser == NULL) {
        cerr << "ERROR: threaduser is NULL" << endl;
        return;
    }
    if(associated_threadusers[command_id].indexOf(threaduser) != -1) {
        cerr << "ERROR: Failed to associate threaduser " << threaduser << " with command id " << command_id << ": already associated." << endl;
        return;
    }
    associated_threadusers[command_id].append(threaduser);
}

/**
 * @author Robert Babilon
 *
 * Disassociates the given thread user from the given command id.
 * 
 * @param threaduser The ThreadUserInterface to disassociate from the given command id.
 * @param command_id The xoscar::CommandId to disassociate from the thread user.
 */
void ThreadHandlerInterface::remove_threaduser_from_commandid(ThreadUserInterface* const threaduser, const xoscar::CommandId command_id)
{
    if(command_id >= LAST_CMD_ID) {
        cerr << "ERROR: LAST_CMD_ID or greater are invalid!" << endl;
        return;
    }
    int index = associated_threadusers[command_id].indexOf(threaduser);
    if(index == -1) {
        cerr << "ERROR: Failed to disassociate threaduser " << threaduser << " from command id " << command_id << ": no existing association" << endl;
        return;
    }
    associated_threadusers[command_id].removeAt(index);
}
/**
 * @author Robert Babilon
 *
 * Calls the handle_thread_result() for each thread user that is associated
 * with the command_id.
 * 
 * This should be called in the thread handler's slot, handle_thread_result().
 * The idea is to notify any other "listening" thread users that command_id was
 * executed and here are the results.
 * 
 * A few differences between this method of notifying and connecting signals is:
 * 1. This method allows notifying thread users about specific commands. Multiple
 * signals might accomplish the same idea, but it would be more complex.
 * 2. This method ensures the thread waits until all thread users have finished
 * processing the results to their needs. Even using the Qt::DirectConnection will
 * not work properly b/c even though the slot is called immediantly, the slot runs
 * as if on another thread. i.e., the signal emitter (the thread handler), 
 * continues execution as it sees fit whether the thread users have finished.
 * 
 * @param skipThreaduser The ThreadUserInterface to skip. In the case that a
 * thread user has initiated the call for command_id and also listens for
 * command_id, the skipThreaduser will not be called since that same thread
 * user initiated the call and was notified via the direct call to handle_thread_result()
 * in the thread handler's handle_thread_result().
 * @param command_id The xoscar::CommandId to associate the thread user with.
 * @param result QString holding the unmodified commands results from the CommandExecutionThread.
 */
void ThreadHandlerInterface::notify_associated_threadusers(ThreadUserInterface* const skipThreaduser, const xoscar::CommandId command_id, const QString& result)
{
    if(command_id >= LAST_CMD_ID) {
        cerr << "ERROR: LAST_CMD_ID or greater are invalid!" << endl;
        return;
    }
    QList<ThreadUserInterface*> threadusers = associated_threadusers[command_id];
    for(int index = 0; index < threadusers.count(); index++) {
        if(threadusers.at(index) != skipThreaduser) {
            threadusers.at(index)->handle_thread_result(command_id, result);
        }
    }
}
