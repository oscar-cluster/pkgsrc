/*
 *  Copyright (c) 2008 Oak Ridge National Laboratory, 
 *                     Geoffroy Vallee <valleegr@ornl.gov>
 *                     All rights reserved
 *  This file is part of the xoscar software.  For license information,
 *  see the COPYING file in the top level directory of the source
 */

/**
  * @file SimpleConfigFile.h
  * @brief Defines a class for the management of simple configuration files.
  * @author Geoffroy Vallee.
  */

#ifndef SIMPLE_CONFIG_FILE_H
#define SIMPLE_CONFIG_FILE_H

#include "Hash.h"

using namespace std;

/**
 * @namespace xoscar
 * @author Geoffroy Vallee.
 * @brief The xoscar namespace gathers all classes needed for XOSCAR.
 */
namespace xoscar {

    /**
    * @class SimpleConfigFile
    */
    class SimpleConfigFile
    {
    public:
        SimpleConfigFile (const string);
        ~SimpleConfigFile ();
        xoscar::Hash get_config ();

    private:
        int load ();
        int is_a_comment (string);
        int analyze_line (string);
        int save_default_config ();
        void init_default_config ();

        string configFilePath;
        xoscar::Hash config;
        xoscar::Hash default_config;
    };

} // namespace xoscar

#endif // SIMPLE_CONFIG_FILE_H
