/*
 *  Copyright (c) 2008 Oak Ridge National Laboratory, 
 *                     Geoffroy Vallee <valleegr@ornl.gov>
 *                     All rights reserved
 *  This file is part of the xoscar software, part of the OSCAR software.
 *  For license information, see the COPYING file in the top level directory
 *  of the OSCAR source.
 */

/**
 * @file CommandBuilder.cpp
 * @brief Implements the CommandBuilder class.
 * @author Geoffroy Vallee
 */

#include "CommandBuilder.h"
#include "SimpleConfigFile.h"

using namespace xoscar;
using namespace std;

CommandBuilder::CommandBuilder ()
{
    // We read the xoscar configuration file (~/.xoscar.conf). Note that if the
    // file does not exist, a default configuration file is created.
    QString path = getenv("HOME");
    QDir dir (path);
    if ( !dir.exists() ) {
        cerr << "ERROR: Impossible to find the home directory" << endl;
        return;
    }
    path = path + "/.xoscar.conf";

    cout << "Config file: " << path.toStdString() << endl;
    SimpleConfigFile confFile = SimpleConfigFile (path.toStdString());
    Hash config = confFile.get_config();
    if (config.size() <= 0) {
        cerr << "ERROR: Impossible to read the config file ("
             << path.toStdString()
             << ")"
             << endl;
    }
    string mgt_mode = config.value ("management_mode");
    cout << "Mgt mode: " << mgt_mode << "." << endl;
    if (mgt_mode.compare ("local") == 0) {
        preCommand = config.value ("command_prefix") + " ";
    } else if (mgt_mode.compare ("remote") == 0) {
        preCommand = "ssh " 
            + config.value("username") 
            + "@" 
            + config.value("oscar_server_ip")
            + " "
            + config.value ("command_prefix") 
            + " ";
    }
}

CommandBuilder::~CommandBuilder ()
{
}

string CommandBuilder::build_cmd (string cmd) {
/*    cout << "Precommand: " << preCommand << endl;
    cout << "Command: " << cmd << endl;*/
    return (preCommand + cmd);
//     return cmd;
}

