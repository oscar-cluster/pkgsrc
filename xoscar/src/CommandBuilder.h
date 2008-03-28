/*
 *  Copyright (c) 2008 Oak Ridge National Laboratory, 
 *                     Geoffroy Vallee <valleegr@ornl.gov>
 *                     All rights reserved
 *  This file is part of the xoscar software, part of the OSCAR software.
 *  For license information, see the COPYING file in the top level directory
 *  of the OSCAR source.
 */

/**
 * @file CommandBuilder.h
 * @brief Defines the CommandBuilder class that prepares OSCAR commands.
 * @author Geoffroy Vallee
 */

#ifndef COMMANDBUILDER_H
#define COMMANDBUILDER_H

#include <QString>
#include <QDir>

#include <iostream>

using namespace std;

/**
 * @namespace xoscar
 * @author Geoffroy Vallee
 * @brief The xoscar namespace gathers all classes needed for XOSCAR.
 */
namespace xoscar {

class CommandBuilder
{
public:
    CommandBuilder ();
    ~CommandBuilder ();
    string build_cmd (string);

private:
    string preCommand;
};

} //namespace xoscar

#endif // COMMANDBUILDER_H
