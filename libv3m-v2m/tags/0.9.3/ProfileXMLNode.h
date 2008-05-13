/*
 *  Copyright (c) 2006 Oak Ridge National Laboratory, 
 *                     Geoffroy Vallee <valleegr@ornl.gov>
 *                     All rights reserved
 *  This file is part of the KVMs software.  For license information,
 *  see the COPYING file in the top level directory of the source
 */

#ifndef PROFILEXMLNODE_H
#define PROFILEXMLNODE_H

#include <libxml++/libxml++.h>

#include "nic.h"

using namespace std;
class Glib::ustring;

typedef struct virtual_fs{
    /** Identifier of the virtual file system */
    string id;

    /** Location of the virtual file system */
    string location;
} virtual_fs_t;

typedef struct profile_data{
    /** Profile's name */
    Glib::ustring name;

    /** Profile's image */
    Glib::ustring image;

    /** Profile's image size */
    Glib::ustring image_size;

    /** Profile's amount of memory */
    Glib::ustring memory;

    /** Profile's type */
    Glib::ustring type;

    /** Profile's MAC address of the first NIC */
    Glib::ustring nic1_mac;

    /** Profile's MAC address of the second NIC */
    Glib::ustring nic2_mac;

    /** Profile's type of the first NIC */
    Glib::ustring nic1_type;

    /** Profile's type of the second NIC */
    Glib::ustring nic2_type;

    /** Profile's name */
    Glib::ustring cdrom;

    /** Profile's name */
    list<virtual_fs_t> list_virtual_fs;
} profile_data_t;

class ProfileXMLNode 
{
public:
  ProfileXMLNode (xmlpp::DomParser* parser, Glib::ustring profile_name);
  ProfileXMLNode (xmlpp::Node* node);
  ProfileXMLNode (Glib::ustring profile_name, Glib::ustring profile_type, Glib::ustring profile_image, Glib::ustring typeNic1, Glib::ustring macNic1, Glib::ustring typeNic2, Glib::ustring macNic2);
  ~ProfileXMLNode ();

  profile_data_t get_profile_data ();
  Glib::ustring get_profile_image ();
  Glib::ustring get_profile_type ();
  xmlpp::Node* get_profile_node ();

  void set_profile_data (profile_data_t);
  void set_profile_name (char* name);
  xmlpp::Document* create_node_for_new_profile (Glib::ustring profile_name, Glib::ustring profile_type, Glib::ustring profile_image, Glib::ustring typeNic1, Glib::ustring macNic1, Glib::ustring typeNic2, Glib::ustring macNic2);

private:
  xmlpp::DomParser* parser;
  xmlpp::Node* profile_node;
  profile_data_t data;

  Glib::ustring get_node_content (const xmlpp::Node* node);
  Glib::ustring get_profile_name (const xmlpp::Node* node);
  xmlpp::Node* find_profile_in_dom (Glib::ustring name);
  int load_profile_image_from_node ();
  int load_image_size_from_node (xmlpp::Node *node);
  int load_profile_type_from_node ();
  int load_profile_name_from_node ();
  int load_profile_memory_from_node ();
  Glib::ustring load_nic_mac (const xmlpp::Node* node);
  Glib::ustring load_nic_type (const xmlpp::Node* node);
  int load_profile_nics_info_from_node ();
  int load_virtual_disk_info_from_node (const xmlpp::Node* node);
  string load_virtual_disk_id (xmlpp::Node* node);
  string load_virtual_disk_location (const xmlpp::Node* node);
  void add_virtual_disk_in_list(string, string);
  int load_virtual_disks_info_from_node ();
  int load_profile_cdrom_from_node ();
  int load_profile_data ();
  void set_profile_image (Glib::ustring image);
  void set_profile_type (Glib::ustring type);
  void set_profile_nic1_mac (Glib::ustring nic1_mac);
  void set_profile_nic1_type (Glib::ustring nic1_type);
  void set_profile_ni2_mac (Glib::ustring nic2_mac);
  void set_profile_nic2_type (Glib::ustring nic2_type);
  int check_nic_type (Glib::ustring type);
  int check_mac_address (Glib::ustring mac);
};

#endif
