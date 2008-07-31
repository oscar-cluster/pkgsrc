/*
 *  Copyright (c) 2006 Oak Ridge National Laboratory,
 *                     Geoffroy Vallee <valleegr@ornl.gov>
 *                     All rights reserved
 *  This file is part of the KVMs software.  For license information,
 *  see the COPYING file in the top level directory of the source
 */

#include <iostream>
#include <stdlib.h>

#include "ArgumentParser.h"
#include "vm.h"
#include "vm_status.h"
#include "debug.h"
char* debug = getenv("V2M_DEBUG");

using namespace std;

VM *vm;

/** 
  * @author Geoffroy Vallee 
  * Analyzes the arguments from the command line.
  * @param argc Number of arguments.
  * @param argv Pointer to the array that stores the arguments.
  */
int analyze_argument (int argc, char **argv)
{
    string argument;
    // First case: command with only one argument
    if (argc == 2) {
        argument = argv[1];
        if (argument.compare("--version") == 0) {
            cout << VERSION << endl;
            return 0;
        }
        if (argument.compare("--help") == 0) {
            return 0;
        }
    } else {
        string xml_profile = argv[1];
        string argument = argv[2];
        vm = new VM(xml_profile);
        DEBUG ("VM instantiated\n");
        DEBUG ("Analysing argument %s\n", argument.c_str()); 
        if (argument.length() <= 0) {
            cerr << "Error, problem analyzing the argument" << endl;
            return -1;
        }
        if (argument.compare("--validate-profile") == 0) {
            // the profile is validated when the VM is instantiated
            cout << "Profile validated" << endl;
            return 0;
        }
        if (argument.compare("--boot-vm") == 0) {
            vm->boot_vm();
            return 0;
        }
        if (argument.compare("--create-vm-image-from-cdrom") == 0) {
            vm->create_image_from_cdrom();
            return 0;
        }
        if (argument.compare("--migrate-vm") == 0) {
            cerr << "WARNING the fonctionality is not yet fully supported" 
                 << endl;
            string dest_node = argv[3];
            vm->migrate (dest_node);
            return 0;
        }
        if (argument.compare("--pause-vm") == 0) {
            cerr << "WARNING the fonctionality is not yet fully supported"
                 << endl;
            vm->pause ();
            return 0;
        }
        if (argument.compare("--unpause-vm") == 0) {
            cerr << "WARNING the fonctionality is not yet fully supported"
                 << endl;
            vm->unpause ();
            return 0;
        }
        if (argument.compare("--install-vm-with-oscar") == 0) {
            vm->create_image_with_oscar();
            return 0;
        }
        if (argument.compare("--status") == 0) {
            int vmstatus = vm->status();
            if (vmstatus < 0){
                cout << "VM not found" << endl;    
            } else {
                if(vmstatus == RUNNING)
                    cout << "VM is running" << endl;    
                else if(vmstatus == PAUSE)
                    cout << "VM is pause" << endl;
                else if(vmstatus == CRASH)
                    cout << "VM is crash" << endl;
                else if(vmstatus == SHUTDOWN)
                    cout << "VM is shutdown" << endl;
                else if(vmstatus == UNKNOWN)
                    cout << "VM is in unknown status" << endl;
            }
        return 0;
        }
    }
}

/** 
  * @author Geoffroy Vallee 
  * Main function of the V2M software.
  * @param argc Number of arguments.
  * @param argv Pointer to the array that stores the arguments.
  */
int main(int argc, char **argv)
{
    ArgumentParser arg_parser (argc, argv);
    DEBUG ("Command validated\n");
    if (analyze_argument (argc, argv) != 0) {
        cerr << "Error: Impossible to execute the command" << endl;
        return -1;
    }
    DEBUG ("I am done... bye bye!\n");
    return 0;
}
