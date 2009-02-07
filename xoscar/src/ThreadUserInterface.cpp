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
 * @brief Implementation of the ThreadUserInterface.
 * @author Robert Babilon
 */

#include "ThreadUserInterface.h"

using namespace xoscar;

ThreadUserInterface::ThreadUserInterface(ThreadHandlerInterface* handler)
{
    threadHandler = handler;
}

ThreadUserInterface::~ThreadUserInterface()
{
}
