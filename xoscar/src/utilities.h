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

class Utilities
{
public:
	static string intToStdString(int);
};

}

#endif // UTILTIES_H
