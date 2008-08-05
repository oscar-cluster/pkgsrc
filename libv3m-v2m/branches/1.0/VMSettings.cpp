/*
 * Copyright (c) 2006 Oak Ridge National Laboratory,
 *                    Geoffroy Vallee <valleegr@ornl.gov>
 *                    All rights reserved
 * This file is part of the libv3m software.  For license information,
 * see the COPYING file in the top level directory of the source
 */

#include <iostream>
#include <fstream>
#include <string>
#include <libxml++/libxml++.h>
#include <libxml++/parsers/textreader.h>
#include "VMSettings.h"

using namespace std;

/**
  * @author Geoffroy Vallee.
  */
VMSettings::VMSettings ()
{
    Glib::ustring filepath = "/etc/libv3m/vm.conf";
    cout << "Analyzing V3M configuration file..." << endl;
    ifstream myfile (filepath.c_str());
    if (myfile.is_open()) {
        xmlpp::DomParser parser;
        parser.set_substitute_entities(false);
        parser.parse_file(filepath);
        if(parser) {
            const xmlpp::Node* pNode = parser.get_document()->get_root_node(); 
            xmlpp::Node::NodeList list = pNode->get_children("qemu");
            xmlpp::Node::NodeList::iterator iter = list.begin();
            xmlpp::Node::NodeList list2;
            xmlpp::Node::NodeList::iterator iter2;
            if (list.size() > 0) {
                /* We get first the qemu command */
                list2 = (*iter)->get_children("command");
                if (list2.size() > 0) {
                    xmlpp::Node::NodeList::iterator iter2 = list2.begin();
                    qemuCommand = get_node_content (*iter2);
                    cout << "Qemu command = " << qemuCommand << endl;
                } else {
                    cerr << "ERROR: Impossible to get the Qemu command" << endl;
                    exit (-1);
                }

                /* Then we get the Qemu precammnd */
                list2 = (*iter)->get_children("precommand");
                if (list2.size() > 0) {
                    iter2 = list2.begin();
                    qemuPrecommand = get_node_content (*iter2);
                    cout << "Qemu precommand = " << qemuPrecommand << endl;
                }
            }
            list = pNode->get_children("xen");
            iter = list.begin();
            if (list.size () > 0) {
                /* We get first the xen command */
                xmlpp::Node::NodeList list2 = (*iter)->get_children("command");
                if (list2.size() > 0) {
                    xmlpp::Node::NodeList::iterator iter2 = list2.begin();
                    xenCommand = get_node_content (*iter2);
                    std::cout << "Xen command = " << xenCommand << std::endl;
                } else {
                    cerr << "ERROR: Impossible to get the Xen command" << endl;
                    exit (-1);
                }
                /* Then we get the xen precammnd */
                list2 = (*iter)->get_children("precommand");
                if (list2.size() > 0) {
                    iter2 = list2.begin();
                    xenPrecommand = get_node_content (*iter2);
                    cout << "Xen precommand = " << xenPrecommand << endl;
                }
                /* Then we check if we want to use a network emulation image */
                list2 = (*iter)->get_children("netboot-image");
                if (list2.size() > 0) {
                    iter2 = list2.begin();
                    netboot = get_node_content (*iter2);
                    cout << "Image for netboot emulation = " << netboot << endl;
                }
            }
            list = pNode->get_children("vmware");
            iter = list.begin();
            if (list.size () > 0) {
                /* We get first the xen command */
                xmlpp::Node::NodeList list2 = (*iter)->get_children("command");
                if (list2.size() > 0) {
                    xmlpp::Node::NodeList::iterator iter2 = list2.begin();
                    vmwareCommand = get_node_content (*iter2);
                    cout << "VMWare command = " << vmwareCommand << endl;
                } else {
                    cerr << "ERROR: Impossible to get the VMWare command" 
                         << endl;
                    exit (-1);
                }
                /* Then we get the xen precammnd */
                list2 = (*iter)->get_children("precommand");
                if (list2.size() > 0) {
                    iter2 = list2.begin();
                    vmwarePrecommand = get_node_content (*iter2);
                    cout << "VMWare precommand = " << vmwarePrecommand << endl;
                }
            }
            /*KVM Part*/
            list = pNode->get_children("kvm");
            iter = list.begin();
            if (list.size () > 0) {
                /* We get first the xen command */
                xmlpp::Node::NodeList list2 = (*iter)->get_children("command");
                if (list2.size() > 0) {
                    xmlpp::Node::NodeList::iterator iter2 = list2.begin();
                    kvmCommand = get_node_content (*iter2);
                    std::cout << "KVM command = " << kvmCommand << std::endl;
                } else {
                    cerr << "ERROR: Impossible to get the KVM command" << endl;
                    exit (-1);
                }
                /* Then we get the xen precammnd */
                list2 = (*iter)->get_children("precommand");
                if (list2.size() > 0) {
                    iter2 = list2.begin();
                    kvmPrecommand = get_node_content (*iter2);
                    cout << "KVM precommand = " << kvmPrecommand << endl;
                }
                /* Then we check if we want to use a network emulation image */
                list2 = (*iter)->get_children("netboot-image");
                if (list2.size() > 0) {
                    iter2 = list2.begin();
                    netboot = get_node_content (*iter2);
                    cout << "Image for netboot emulation = " << netboot << endl;
                }
            }

        }
    } else {
        cerr << "ERROR: Impossible to open the configuration file." << endl;
    }
    cout << "V3M configured" << endl;
}

/**
  * @author Geoffroy Vallee.
  */
VMSettings::~VMSettings () {
}

/**
  * @author Geoffroy Vallee.
  */
Glib::ustring VMSettings::get_node_content (const xmlpp::Node* node)
{
    const xmlpp::ContentNode* nodeContent = 
        dynamic_cast<const xmlpp::ContentNode*>(node);
    const xmlpp::TextNode* nodeText = 
        dynamic_cast<const xmlpp::TextNode*>(node);
    Glib::ustring str = "N/A";

    if(!nodeContent) {
        //Recurse through child nodes:
        xmlpp::Node::NodeList list = node->get_children();
        for(xmlpp::Node::NodeList::iterator iter = list.begin(); 
            iter != list.end(); ++iter) {
            str = get_node_content (*iter); //recursive
        }
    } else if(nodeText) {
        str = nodeText->get_content();
    }
    return str;
}

/**
  * @author Panyong Zhang.
  */
string VMSettings::getKvmCommand() {
    return kvmCommand;
}

/**
  * @author Panyong Zhang.
  */
string VMSettings::getKvmPrecommand () {
    return kvmPrecommand;
}

/**
  * @author Geoffroy Vallee.
  */
string VMSettings::getQemuCommand () {
    return qemuCommand;
}

/**
  * @author Geoffroy Vallee.
  */
string VMSettings::getQemuPrecommand () {
    return qemuPrecommand;
}

/**
  * @author Geoffroy Vallee.
  */
string VMSettings::getXenCommand () {
    return xenCommand;
}

/**
  * @author Geoffroy Vallee.
  */
string VMSettings::getXenPrecommand () {
    return xenPrecommand;
}

/**
  * @author Geoffroy Vallee.
  */
string VMSettings::getVmwareCommand () {
    return vmwareCommand;
}

/**
  * @author Geoffroy Vallee.
  */
string VMSettings::getVmwarePrecommand () {
    return vmwarePrecommand;
}

/**
  * @author Geoffroy Vallee.
  */
string VMSettings::getVMMHPCCommand () {
    return vmmhpcCommand;
}

/**
  * @author Geoffroy Vallee.
  */
string VMSettings::getVMMHPCPrecommand () {
    return vmmhpcPrecommand;
}

/**
  * @author Geoffroy Vallee.
  */
string VMSettings::getNetbootImage () {
    return netboot;
}
