/*
 *  Copyright (c) 2007-2008 Oak Ridge National Laboratory, 
 *                          Geoffroy Vallee <valleegr@ornl.gov>
 *                          All rights reserved
 *  This file is part of the xoscar software, part of the OSCAR software.
 *  For license information, see the COPYING file in the top level directory
 *  of the OSCAR source.
 */

/**
 * @file utilities.h
 * @brief Defines the class Utilities for the oscar GUI. This class is a
 * collection of static methods to process miscellaneous tasks.
 *
 * @author Robert Babilon
 */

#ifndef UTILITIES_H
#define UTILITIES_H

#include <iostream>
#include <sstream>
#include <string>
using namespace std;

namespace xoscar {

enum CommandId {
        INACTIVE = 0,
        GET_LIST_REPO,
        GET_LIST_OPKGS,
        GET_SETUP_DISTROS,
        DO_SYSTEM_SANITY_CHECK,
        DO_OSCAR_SANITY_CHECK,
        GET_LIST_DEFAULT_REPOS,
        DISPLAY_PARTITIONS,
        DISPLAY_PARTITION_NODES,
        DISPLAY_PARTITION_DISTRO,
        ADD_PARTITION,
        DISPLAY_DETAILS_PARTITION_NODES,
        SETUP_DISTRO,
        LIST_UNSETUP_DISTROS,
        DISPLAY_DEFAULT_OSCAR_REPO,
        DISPLAY_DEFAULT_DISTRO_REPO,
        REMOVE_PARTITION,
        DISPLAY_DEFAULT_OPKGS
    };

class Utilities
{
public:
	static string intToStdString(int);
};

}

#endif // UTILTIES_H
