/*
 *  Copyright (c) 2007-2008 Oak Ridge National Laboratory, 
 *                          Geoffroy Vallee <valleegr@ornl.gov>
 *                          All rights reserved
 *  This file is part of the xoscar software, part of the OSCAR software.
 *  For license information, see the COPYING file in the top level directory
 *  of the OSCAR source.
 */

/**
 * @file Loading.h
 * @brief Defines the class Loading.
 * @author Robert Babilon
 *
 */
#include <iostream>
using namespace std;

class Loading
{
public:
    Loading(bool *);
    ~Loading();

private:
    bool *var;
};
