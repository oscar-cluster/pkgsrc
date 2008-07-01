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

Loading::Loading(const bool *remoteVar)
{
    var = remoteVar;
    *var = true;
}

Loading::~Loading()
{
    *var = false;
    var = NULL;
}

