/*
 *  Copyright (c) 2006-2007 Oak Ridge National Laboratory,
 *                          Geoffroy Vallee <valleegr@ornl.gov>
 *                          All rights reserved
 *  This file is part of the KVMs software.  For license information,
 *  see the COPYING file in the top level directory of the source
 */

#include <fstream>
#include <iostream>
#include <string>
#include <libxml++/libxml++.h>

#include "ArgumentParser.h"

/* This is the list of valid arguments for v2m */
#define NB_VALID_ARGS 10 // Number of possible arguments
string valid_arguments [NB_VALID_ARGS] = // list of arguments
    {"--version",
     "--help",
     "--validate-profile",
     "--boot-vm", 
     "--create-vm-image-from-cdrom", 
     "--install-vm-with-oscar",
     "--migrate-vm",
     "--pause-vm",
     "--unpause-vm",
     "--status"}; 

/**
  * @author Geoffroy Vallee
  *
  * Class contructor.
  * @param argc Number of arguments.
  * @param argv Pointer to the array that store the arguments.
  */
ArgumentParser::ArgumentParser (int argc, char **argv)
{
    if (argc == 2) {
        string opt = argv[1];
        if (argumentCheck(opt) == -1) {
            cerr << "ERROR: invalid argument!" << endl;
            exit (-1);
        } else {
            if (opt.compare ("--help") == 0) {
                printUsage();
            }
        }
    } else {
        // We first check the form of the command
        if (argc < 3) {
            cerr << "Error, the command synthax is not correct." << endl;
            printUsage();
            exit (-1);
        }
        cout << "Number of arguments = " << argc << endl;
        cout << "Profile file is " << argv[1] << endl;
        // argv[1] is the XML file for the profile
        // argv[2] is the argument of the command

        // We check the profile: check if the file is there, if the file is 
        // based in the DTD and compliant with the DTD
        ifstream f(argv[1]);
        if (!f) {
            cerr << "ERROR: profile does not exist (." << argv[1] << ")" << endl;
            printUsage();
            exit (-1);
        }
        xmlpp::DomParser parser;
        parser.set_validate();
          parser.parse_file(argv[1]);
        if (!parser) {
            cerr << "Error, profile not valid. Check your profile." << endl;
            exit (-1);
        }

        // We check the argument. Only one action at a time
        if (argumentCheck(argv[2]) == -1) {
            cerr << "Error, invalid argument." << endl;
            printUsage();
            exit (-1);
        }
        cout << "Argument found: " << argv[2] << endl;
    }
}

/**
  * @author Geoffroy Vallee
  *
  * Class destructor.
  */
ArgumentParser::~ArgumentParser () 
{
}

/**
  * @author Geoffroy Vallee
  *
  * Print v2m usage, i.e., available options and the command syntax.
  */
void ArgumentParser::printUsage () 
{
    cout << "v2m usage:" << endl;
    cout << "\tv2m <xml_profile> [OPTION]" << endl;
    cout << "\nv2m [OPTION]:" << endl;
    cout << "\t--boot-vm: boot a VM instantiation based on the profile." <<
            endl;
    cout << "\t--create-vm-image-from-cdrom: create an image for a VM \
             using a bootable CDROM" << endl;
    cout << "\t--install-vm-with-oscar: create an image for a \
             VM using a network installation with OSCAR" << endl;
    cout << "\t--migrate-vm <destination_node>: migrate a VM on a remote node \
             (EXPERIMENTAL)" << endl;
    cout << "\t--pause-vm: pause a VM (EXPERIMENTAL)" << endl;
    cout << "\t--unpause-vm: unpause a VM (EXPERIMENTAL)" << endl;
    cout << "\t--validate-profile: validate the VM's profile" << endl;
    cout << "\t--version: give the V2M version" << endl;
}

/**
  * @author Geoffroy Vallee
  *
  * Check the argument, i.e., compare the argument with the list of valid
  * arguments defined via the string array "valid_arguments"
  * @param arg Argument to analyse (string)
  * @return 1 if success, -1 else
  */
int ArgumentParser::argumentCheck(string arg) {
    int i = 0;
    while (i < NB_VALID_ARGS) {
        if ((valid_arguments[i]).compare(arg) == 0)
            return 1;
        i++;
    }
    return -1;
}

