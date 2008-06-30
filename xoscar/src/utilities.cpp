/*
 *  Copyright (c) 2007-2008 Oak Ridge National Laboratory, 
 *                          Geoffroy Vallee <valleegr@ornl.gov>
 *                          All rights reserved
 *  This file is part of the xoscar software, part of the OSCAR software.
 *  For license information, see the COPYING file in the top level directory
 *  of the OSCAR source.
 */

/**
 * @file utilities.cpp
 * @brief Actual implementation of the Utilities class.
 * @author Robert Babilon
 */

#include "utilities.h"

using namespace xoscar;

/**
 * @author Geoffroy Vallee.
 *
 * Utility function: convert an integer to a standard string.
 *
 * @param i Integer to convert in string.
 * @return Standard string representing the integer.
 */
string Utilities::intToStdString (int i)
{
    std::stringstream ss;
    std::string str;
    ss << i;
    ss >> str;
    return str;
}
