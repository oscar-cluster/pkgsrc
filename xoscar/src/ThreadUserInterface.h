/*
 *  Copyright (c) 2007-2008 Oak Ridge National Laboratory,
 *                          Geoffroy Vallee <valleegr@ornl.gov>
 *                          All rights reserved
 *  This file is part of the xoscar software, part of the OSCAR software.
 *  For license information, see the COPYING file in the top level directory
 *  of the OSCAR source.
 */

/**
 * @file ThreadUserInterface.h
 * @brief Defines the thread user interface
 * @author Robert Babilon
 */

#ifndef THREADUSERINTERFACE_H
#define THREADUSERINTERFACE_H

#include "ThreadHandlerInterface.h"
#include "utilities.h"
#include "CommandTask.h"

namespace xoscar {

class ThreadHandlerInterface;

class ThreadUserInterface
{
public:
    ThreadUserInterface(ThreadHandlerInterface* handler);
    ~ThreadUserInterface();

    // thread handler must be able to call this
    virtual int handle_thread_result (xoscar::CommandId command_id, const QString result) = 0;

protected:
    ThreadHandlerInterface *threadHandler;
};

} // namespace xoscar

#endif // THREADUSERINTERFACE_H
