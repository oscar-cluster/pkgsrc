/*
 *  Copyright (c) 2007-2008 Oak Ridge National Laboratory, 
 *                          Geoffroy Vallee <valleegr@ornl.gov>
 *                          All rights reserved
 *  This file is part of the xoscar software, part of the OSCAR software.
 *  For license information, see the COPYING file in the top level directory
 *  of the OSCAR source.
 */

/**
 * @file Loading.cpp
 * @brief Actual implementation of the Loading class.
 * @author Robert Babilon
 */
#include "Loading.h"

using namespace xoscar;

/**
 * @author Robert Babilon
 * 
 * Stores the pointer to this member's variable and then increments the
 * dereferenced value by one.
 *
 * @param remoteVar Pointer to the loading variable stored in another class.
 */
Loading::Loading(int *remoteVar)
{
    var = remoteVar;
    *var += 1;
}

/**
 * @author Robert Babilon
 *
 * Decrements the dereferenced value by one and assigns NULL to the member
 * pointer.
 */
Loading::~Loading()
{
    *var -= 1;
    var = NULL;
}

