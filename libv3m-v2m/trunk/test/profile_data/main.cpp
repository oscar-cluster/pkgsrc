/*
 *  Copyright (c) 2006 Oak Ridge National Laboratory, 
 *                     Geoffroy Vallee <valleegr@ornl.gov>
 *                     All rights reserved
 *  This file is part of the KVMs software.  For license information,
 *  see the COPYING file in the top level directory of the source
 */

#include <glibmm/ustring.h>
#include <libxml++/libxml++.h>
#include <iostream>

#include "../../libv3m.h"

int main(int argc, char **argv)
{
  Glib::ustring filepath_in = "./profiles.xml";

  xmlpp::DomParser parser;
  parser.set_validate();
  parser.parse_file(filepath_in);

  /* we load the profiles from the XML DOM tree */
  const xmlpp::Node* root = parser.get_document()->get_root_node(); 

  if(root) {

    xmlpp::Node::NodeList list = root->get_children("profile");
    std::cout << list.size() << " profile(s) found" << std::endl;

    for (xmlpp::Node::NodeList::iterator iter = list.begin(); iter != list.end(); ++iter) {
      ProfileXMLNode* profile = new ProfileXMLNode (*iter);
      if (profile == NULL) {
        std::cout << "FormMainWindow::loadProfile_XML: No profile found" << std::endl;
        return -1;
      }
      profile_data_t profile_data = profile->get_profile_data();
      if ((profile_data.name).compare("test1")!=0) {
        std::cerr << "Profile name incorrect (" << (profile_data.name).c_str() << ") -> test failed!" << std::endl;
        return -1;
      }
      else {
        std::cerr << "Profile name is correct -> test succeed!" << std::endl;
      }
      if ((profile_data.type).compare("test2")!=0) {
        std::cerr << "Profile type incorrect (" << (profile_data.type).c_str() << ") -> test failed!" << std::endl;
        return -1;
      }
      else {
        std::cerr << "Profile type is correct -> test succeed!" << std::endl;
      }
      if ((profile_data.image).compare("test3")!=0) {
        std::cerr << "Profile image incorrect (" << (profile_data.image).c_str() << ") -> test failed!" << std::endl;
        return -1;
      }
      else {
        std::cerr << "Profile image is correct -> test succeed!" << std::endl;
      }
      if ((profile_data.nic1_type).compare("TUN/TAP")!=0) {
        std::cerr << "Profile type for NIC1 incorrect (" << (profile_data.nic1_type).c_str() << ") -> test failed!" << std::endl;
        return -1;
      }
      else {
        std::cerr << "Profile type for NIC1 is correct -> test succeed!" << std::endl;
      }
      if ((profile_data.nic1_mac).compare("00:01:02:03:04:05")!=0) {
        std::cerr << "Profile MAC @ for NIC1 incorrect (" << (profile_data.nic1_mac).c_str() << ") -> test failed!" << std::endl;
        return -1;
      }
      else {
        std::cerr << "Profile MAC @ for NIC1 is correct -> test succeed!" << std::endl;
      }
      if ((profile_data.nic2_type).compare("VLAN")!=0) {
        std::cerr << "Profile type for NIC2 incorrect (" << (profile_data.nic2_type).c_str() << ") -> test failed!" << std::endl;
        return -1;
      }
      else {
        std::cerr << "Profile type for NIC2 is correct -> test succeed!" << std::endl;
      }
      if ((profile_data.nic2_mac).compare("00:01:02:03:04:06")!=0) {
        std::cerr << "Profile MAC @ for NIC2 incorrect (" << (profile_data.nic2_mac).c_str() << ") -> test failed!" << std::endl;
        return -1;
      }
      else {
         std::cerr << "Profile MAC @ for NIC2 is correct -> test succeed!" << std::endl;
      }
    }
  }
  else {
    std::cout << "No profile exist" << std::endl;
    return -1;
  }

  std::cout << "Test of profile's data succeed!" << std::endl;
  return 0;
}
