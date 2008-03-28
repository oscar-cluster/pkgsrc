/*
 *  Copyright (c) 2008 Oak Ridge National Laboratory, 
 *                     Geoffroy Vallee <valleegr@ornl.gov>
 *                     All rights reserved
 *  This file is part of the xoscar software.  For license information,
 *  see the COPYING file in the top level directory of the source
 */

/**
 * @file SimpleConfigFile.cpp
 * @brief Actual implementation of the SimpleConfigFile class.
 * @author Geoffroy Vallee
 */

#include <iostream>
#include <fstream>
#include <QFile>
#include <QTextStream>
#include <QString>
#include <QStringList>

#include "SimpleConfigFile.h"

using namespace xoscar;

SimpleConfigFile::SimpleConfigFile (string config_file_path)
{
    init_default_config ();
    QFile file (config_file_path.c_str());
    configFilePath = config_file_path;
    if (file.exists () == false) {
        cout << "The config file does not exist, we create one: "
             << config_file_path
             << endl;
        // If the config file does not exist, we create it based on default
        save_default_config ();
    }
    load();
    cout << "INFO: Config file parsed" << endl;
}

void SimpleConfigFile::init_default_config ()
{
    default_config = Hash ("management_mode", "local",
                           "command_prefix", "sudo",
                           "username", "user",
                           "oscar_server_ip", "192.168.0.1",
                           NULL);
}

SimpleConfigFile::~SimpleConfigFile ()
{
}

int SimpleConfigFile::save_default_config ()
{
    ofstream myfile;
    int config_size = default_config.size ();

    myfile.open (configFilePath.c_str());
    for (int i = 0; i < config_size; i++) {
        const string key = default_config.at(i).key;
        const string value = default_config.at(i).value;
        myfile << key << " = " << value << endl;
    }
    myfile.close();

    return 0;
}

int SimpleConfigFile::load ()
{
    // First some checking: does the config file exists?
    ifstream configFile;
    configFile.open (configFilePath.c_str(), ios::out);
    if (!configFile) {
        cerr << "Unable to open the config file ("
             << configFilePath
             << ")"
             << endl;
        return -1;
    }
    string line;
    while ( !configFile.eof() ) {
        getline (configFile, line);
        if ( line.size() > 0 && !is_a_comment (line) ) {
            // the line is not a comment, we can analyse it
            cout << "We found a line" << endl;
            analyze_line (line);
        } else {
            cout << "We found a comment: " << line << endl;
        }
    }
    configFile.close();
    return 0;
}

int SimpleConfigFile::is_a_comment (string line)
{
    unsigned int pos = 0;
    string character = " ";
    // We skip the spaces at the beginning of the line
    while (pos < line.size() && character.compare(" ") == 0) {
        character = line.at(pos);
        pos++;
    }
    // Test if we are at the end of the line (empty lines). For us here, an 
    // empty line is like a comment
    if (pos >= line.size()) {
        return 1;
    }

    character = line.at(pos);
    if (character.compare ("#") == 0) {
        return 1;
    } else {
        return 0;
    }

    return 0;
}

/**
 * @return 1 if the line is not a configuration option, 0 if it is a comment, -1
 *         if an error occurs.
 */
int SimpleConfigFile::analyze_line (string line)
{
    QString str = line.c_str();
    QStringList list = str.split(" = ");
    cout << "From config file:" << endl;
    cout << "\tkey: " << list.at(0).toStdString() << endl;
    cout << "\tvalue: " << list.at(1).toStdString() << endl;

    config.add (list.at(0).toStdString(), list.at(1).toStdString());
    return 0;
}

Hash SimpleConfigFile::get_config ()
{
    return config;
}
