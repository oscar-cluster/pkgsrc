/*
 *  Copyright (c) 2006-2008 Oak Ridge National Laboratory, 
 *                          Geoffroy Vallee <valleegr at ornl dot gov>
 *                          All rights reserved
 *  This file is part of the KVMs software.  For license information,
 *  see the COPYING file in the top level directory of the source
 */

#include <iostream>

#include "ProfileXMLNode.h"

/**
  * @author Geoffroy Vallee.
  *
  * Class constructor: the class allows user to get an XML representation of a
  * specific profile (ie. specific VM specification).
  * For this constructor, we do not have yet an XML representation but the name
  * and an XML tree in which the profile is stored.
  *
  * @param xml_parser representation of the XML documentation, in our case the
  * XML document representing profiles.
  * @param profile_name name of the profile that we want to isolate.
  */
ProfileXMLNode::ProfileXMLNode (xmlpp::DomParser* xml_parser, Glib::ustring
profile_name)
{
  if (xml_parser == NULL) {
    cerr << "ERROR: the XML parser is not valid (NULL)" << endl;
    return;
  }

  parser = xml_parser;

  /* we find the profile node */
  profile_node = ProfileXMLNode::find_profile_in_dom (profile_name);
  if (profile_node == NULL) {
    cerr << "ERROR: impossible to find the profile" << endl;
    return;
  }

  if (load_profile_data ()) {
        cerr << "ERROR loading the profile's data" << endl;
        exit (-1);
  }
}

/**
  * @author Geoffroy Vallee.
  *
  * Class constructor: the class allows user to get an XML representation of a
  * specific profile (ie. specific VM specification).
  * For this constructor, we have an XML representation of the profile therefore
  * we just create a class instantiation based on that.
  *
  * @param node XML node representing the profile.
  */
ProfileXMLNode::ProfileXMLNode (xmlpp::Node* node)
{
    if (node == NULL) {
        cerr << "ProfileXMLNode::ProfileXMLNode: the node is not valid (NULL)"
             << endl;
        return;
    }

    /* we find the profile node */
    profile_node = node;

    if (load_profile_data ()) {
        cerr << "ERROR loading the profile" << endl;
        exit (-1);
    }
}


/**
  * @author Geoffroy Vallee.
  *
  * Class constructor: the class allows user to get an XML representation of a
  * specific profile (ie. specific VM specification).
  * For this constructor, we have an high-level description of the profile
  * (name, type, image, network information).
  *
  * @param profile_name Name of the profile
  * @param profile_type Type of the profile
  * @param profile_image Image (ie. file used as block device for the VM) of the
  * profile
  * @param typeNic1 Type of the first NIC of the VM (N/A, TUN/TAP or VLAN)
  * @param macNic1 MAC address of the first NIC of the VM (N/A or hexadecimal
  * notation)
  * @param typeNic2 Type of the second NIC of the VM (N/A, TUN/TAP or VLAN)
  * @param macNic2 MAC address of the second NIC of the VM (N/A or hexadecimal
  * notation)
  */
ProfileXMLNode::ProfileXMLNode (Glib::ustring profile_name, Glib::ustring
profile_type, Glib::ustring profile_image, Glib::ustring typeNic1, Glib::ustring
macNic1, Glib::ustring typeNic2, Glib::ustring macNic2)
{
    xmlpp::Document* profile = create_node_for_new_profile (profile_name,
                                                            profile_type,
                                                            profile_image,
                                                            typeNic1,
                                                            macNic1,
                                                            typeNic2,
                                                            macNic2);
    if (profile == NULL) {
        cerr << "ProfileXMLNode::ProfileXMLNode: Impossible to create a new "
             << "profile object" << endl;
    }
    xmlpp::Element* root = profile->get_root_node();
    if (root == NULL) {
        cerr << "ProfileXMLNode::ProfileXMLNode: Impossible to create a new "
             << "profile object" << endl;
    }
    xmlpp::Node::NodeList list = root->get_children("profile");
    xmlpp::Node::NodeList::iterator iter = list.begin();
    profile_node = (*iter);

    if (load_profile_data ()) {
        cerr << "ERROR loading the profile" << endl;
        exit (-1);
    }
}

/**
  * @author Geoffroy Vallee.
  *
  * Class destructor.
  */
ProfileXMLNode::~ProfileXMLNode ()
{
}

/**
  * @author Geoffroy Vallee.
  *
  * Check NIC type.
  *
  * @param type NIC type.
  * @return NIC_TYPE_VALID if the type is N/A, TUN/TAP or VLAN.
  * @return NIC_TYPE_UNVALID if the type is something else.
  */
#define NB_NIC_TYPES 6
std::string valid_nic_types[NB_NIC_TYPES] = {"", 
                                             "N/A",
                                             "TUN/TAP",
                                             "TUN/TAP+NAT",
                                             "VLAN",
                                             "BRIDGED_TAP"};
int ProfileXMLNode::check_nic_type (Glib::ustring type)
{
  for (int i=0; i<NB_NIC_TYPES; i++) {
        if (type.compare (valid_nic_types[i]) == 0) {
                return NIC_TYPE_VALID;
        }
  }
  cerr << "ERROR: Unknow network type (" << type << ")" << endl;
  return NIC_TYPE_UNVALID;
}

/**
  * @author Geoffroy Vallee
  *
  * Check if the image for the VM is really a virtual disk. We do not support
  * "virtual partition (only one partition within the image), the image really
  * have to be a virtual disk, like for Qemu.
  *
  * @param: image path.
  * @return 0 if success, -1 else.
  */
int check_image_type (Glib::ustring image)
{
    Glib::ustring cmd = "file ";
    cmd += image;
    cmd += " | grep sector";
    if (system (cmd.c_str())) {
        cerr << "The image you try to use is not a virtual disk" << endl;
        return -1;
    } else {
        cout << "Checking virtual disk: OK." << endl;
        return 0;
    }
}

/**
  * @author Geoffroy Vallee.
  *
  * Set the name of the profile. DEPRECATED.
  */
void ProfileXMLNode::set_profile_name (char* name)
{
    cerr << "ProfileXMLNode::set_profile_name; Not yet implemented" << endl;
}

/**
  * @author Geoffroy Vallee.
  *
  * Get the image location from ptofile's data.
  *
  * @return Image path.
  */
Glib::ustring ProfileXMLNode::get_profile_image ()
{
  return (data.image);
}

/**
  * @author Geoffroy Vallee.
  *
  * Get the image location from ptofile's type.
  *
  * @return Image type.
  */
Glib::ustring ProfileXMLNode::get_profile_type ()
{
  return (data.type);
}

/**
  * @author Geoffroy Vallee.
  *
  * Set the image of the profile. DEPRECATED.
  */
void ProfileXMLNode::set_profile_image (Glib::ustring image)
{
    cerr << "ProfileXMLNode::set_profile_image; Not yet implemented" << endl;
}

/**
  * @author Geoffroy Vallee.
  *
  * Set the type of the profile. DEPRECATED.
  */
void ProfileXMLNode::set_profile_type (Glib::ustring type)
{
    cerr << "ProfileXMLNode::set_profile_type; Not yet implemented" << endl;
}

/**
  * @author Geoffroy Vallee.
  *
  * Set the MAC address of the first NIC. DEPRECATED.
  *
  */
void ProfileXMLNode::set_profile_nic1_mac (Glib::ustring nic1_mac)
{
    cerr << "ProfileXMLNode::set_profile_nic1_mac; Not yet implemented" << endl;
}

/**
  * @author Geoffroy Vallee.
  *
  * DEPRECATED
  */
void ProfileXMLNode::set_profile_nic1_type (Glib::ustring nic1_type)
{
    cerr << "ProfileXMLNode::set_profile_nic1_type; Not yet implemented" 
         << endl;
}

/**
  * @author Geoffroy Vallee.
  *
  * DEPRECATED
  */
void ProfileXMLNode::set_profile_ni2_mac (Glib::ustring nic2_mac)
{
    cerr << "ProfileXMLNode::set_profile_ni2_mac; Not yet implemented" << endl;
}

/**
  * @author Geoffroy Vallee.
  *
  * DEPRECATED
  */
void ProfileXMLNode::set_profile_nic2_type (Glib::ustring nic2_type)
{
    cerr << "ProfileXMLNode::set_profile_nic2_type; Not yet implemented" 
         << endl;
}



/**
  * @author Geoffroy Vallee.
  *
  * Set the data (profile name, type and so on) of the profile.
  *
  * @param new_data New profile data 
  */
void ProfileXMLNode::set_profile_data (profile_data_t new_data)
{
    data = new_data;
}

/**
  * @author Geoffroy Vallee.
  *
  * The function find a specific profile in the DOM tree. The profile is
  * identified by its name.
  *
  * @param name Profile name.
  * @return Pointer on the profile XML node.
  * @return NULL else.
  */
xmlpp::Node* ProfileXMLNode::find_profile_in_dom (Glib::ustring name)
{
    /* We get all profiles */
    xmlpp::Document *doc = ProfileXMLNode::parser->get_document();
    if (doc == NULL) {
        cerr << "ProfileXMLNode::find_profile_in_dom: Impossible to get the "
             << "XML DOM trees" << endl;
        return NULL;
    }
    xmlpp::Element *root = doc->get_root_node();
    if (root == NULL) {
        cerr << "ProfileXMLNode::find_profile_in_dom: Impossible to find the "
             << "XML root node" << endl;
    }
    xmlpp::Node::NodeList list = root->get_children("profile");
    xmlpp::Node::NodeList::iterator iter = list.begin();
    while (iter != list.end() && name != get_profile_name (*iter)) {
        ++iter;
    }

    return *iter;
}

/**
  * @author Geoffroy Vallee
  *
  * Get the content of an XML node.
  *
  * @param node XML node for a profile.
  * @return text within the XML node.
  * @return N/A else.
  */
Glib::ustring ProfileXMLNode::get_node_content (const xmlpp::Node* node)
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
    }
    // Let's say when it's text. - e.g. let's say what that white space is.
    else if(nodeText) {
        str = nodeText->get_content();
    }
    return str;
}

/**
  * @author Geoffroy Vallee.
  *
  * Load the profile's name from the XML node.
  *
  * @return 0 if success, -1 else
  */
int ProfileXMLNode::load_profile_name_from_node ()
{
    xmlpp::Node::NodeList list = profile_node->get_children("name");
    if (list.size() >= 2) {
        data.name = "Error";
        return -1;
    }
    if (list.size() <= 0) {
        cerr << "ERROR: the profile does not have a name" << endl;
        data.name = "Error";
        return -1;
    } else {
        xmlpp::Node::NodeList::iterator iter = list.begin();
        data.name = get_node_content (*iter);
    }
    return 0;
}

/**
  * @author Geoffroy Vallee.
  *
  * Load the profile's ammount of memory from the XML node.
  *
  * @return 0 if success, -1 else.
  */
int ProfileXMLNode::load_profile_memory_from_node ()
{
    xmlpp::Node::NodeList list = profile_node->get_children("memory");
    if (list.size() >= 2) {
        data.memory = "Error";
        return -1;
    }
    if (list.size() <= 0) {
        /* No memeory information is specified into the profile */
        /* We assign a value by default */
        data.memory = "128";
        return 0;
    } else {
        xmlpp::Node::NodeList::iterator iter = list.begin();
        data.memory = get_node_content (*iter);
    }
    return 0;
}

/**
  * @author Geoffroy Vallee.
  *
  * Load the profile's type from the XML node.
  *
  * @return: 0 if success, -1 else.
  */
int ProfileXMLNode::load_profile_type_from_node ()
{
    Glib::ustring type = "N/A";
    xmlpp::Node::NodeList list = profile_node->get_children("type");
    if (list.size() <= 0 || list.size() >= 2) {
        type = "Error";
        return -1;
    } else {
        xmlpp::Node::NodeList::iterator iter = list.begin(); 
        type = get_node_content (*iter);
    }
    data.type = type;
    return 0;
}

/**
  * @author Geoffroy Vallee.
  *
  * Load the profile's image size from the XML node.
  *
  * @param node XML node representing the image data.
  * @return 0 if success, -1 else.
  */
int ProfileXMLNode::load_image_size_from_node (xmlpp::Node *node)
{
    Glib::ustring size = "N/A";
    xmlpp::Element *el = new xmlpp::Element (node->cobj());
    xmlpp::Attribute *attr = el->get_attribute ("size");
    if (attr) {
        cout << "Image size = " << attr->get_value() << endl;
        size = attr->get_value();
    } else {
        cout << "Image size not found" << endl;
    }
    data.image_size = size;
    return 0;
}

/**
  * @author Geoffroy Vallee.
  *
  * Load the profile image from the XML node.
  *
  * @return 0 if success, -1 else
  */
int ProfileXMLNode::load_profile_image_from_node () 
{
    Glib::ustring image = "N/A";
    xmlpp::Node::NodeList list = profile_node->get_children("image");
    for (xmlpp::Node::NodeList::iterator iter = list.begin(); 
         iter != list.end(); ++iter) {
        load_image_size_from_node (*iter);
        image = get_node_content (*iter);
    }
    data.image = image;
    check_image_type (image);
    return 0;
}

/**
  * @author Geoffroy Vallee.
  *
  * Load cdrom information from the profile.
  *
  * @return 0 if success, -1 else.
  */
int ProfileXMLNode::load_profile_cdrom_from_node ()
{
    Glib::ustring cdrom = "N/A";
    xmlpp::Node::NodeList list = profile_node->get_children("cdrom");
    for (xmlpp::Node::NodeList::iterator iter = list.begin(); 
         iter != list.end(); ++iter) {
        cdrom = get_node_content (*iter);
    }
    data.cdrom = cdrom;
    return 0;
}

/**
  * @author Geoffroy Vallee.
  *
  * Load profile data from XML tree to class data structure (which act as a
  * cache)
  *
  * @return: 0 if success, -1 else.
  */
int ProfileXMLNode::load_profile_data()
{
    if (load_profile_name_from_node ())
        return -1;
    if (load_profile_type_from_node ())
        return -1;
    if (load_profile_image_from_node ())
        return -1;
    if (load_virtual_disks_info_from_node ())
        return -1;
    if (load_profile_memory_from_node ())
        return -1;
    if (load_profile_nics_info_from_node ())
        return -1;
    if (load_profile_cdrom_from_node ())
        return -1;

    return 0;
}

/**
  * @author Geoffroy Vallee.
  *
  * Get profile data from the class data structure 
  *
  * @return Structure representing the profile of the virtual machine.
  */
profile_data_t ProfileXMLNode::get_profile_data ()
{
    return data;
}


/**
  * @author Geoffroy Vallee.
  *
  * Load the image path for a profile from the XML node.
  *
  * @param node XML node representing the profile.
  * @return the profile name (which is "N/A" in case of problem).
  */
Glib::ustring ProfileXMLNode::get_profile_name (const xmlpp::Node* node)
{
    if (node == NULL) {
        cerr << "ProfileXMLNode::get_profile_name: unknown node (NULL)" << endl;
        return "N/A";
    }
    Glib::ustring name = "";
    xmlpp::Node::NodeList list = node->get_children("name");
    for (xmlpp::Node::NodeList::iterator iter = list.begin(); 
         iter != list.end(); ++iter) {
        name = get_node_content (*iter);
    }
    return (name);
}

/** 
  * @author Geoffroy Vallee.
  *
  * Check the validity of a MAC address.
  *
  * @param str String representing the MAC address.
  * @return 0 if success, -1 else.
  */
int ProfileXMLNode::check_mac_address (Glib::ustring str)
{
    if (!str.find ("01:")) {
        cerr << "ERROR your MAC address may be an address for multicast "
             << "(starts by 01)" << endl;
        return - 1;
    }
    if (!str.find ("33:")) {
        cerr << "ERROR your MAC address may be an address for multicast "
             << "(starts by 01)" << endl;
        return -1;
    }
    if (!str.find ("FF:FF:FF:FF:FF:FF")) {
        cerr << "ERROR your MAC address may be an address for broadcast" 
             << endl;
        return -1;
    }

    return 0;
}


/**
  * @author Geoffroy Vallee.
  *
  * Load NIC mac address from the XML node.
  *
  * @param node XML node representing NIC data for a profile.
  * @return the MAC address of the NIC (which is "N/A" in case of problem).
  */
Glib::ustring ProfileXMLNode::load_nic_mac (const xmlpp::Node* node)
{
    if (node == NULL) {
        cerr << "ProfileXMLNode::load_nic_mac: unknown node (NULL)" << endl;
        return "N/A";
    }
    Glib::ustring str = "";
    xmlpp::Node::NodeList list1 = node->get_children("mac");
    for (xmlpp::Node::NodeList::iterator iter = list1.begin(); 
         iter != list1.end(); ++iter) {
        str = get_node_content (*iter);
    }
    return (str);
}

/**
  * @author Geoffroy Vallee.
  *
  * Load NIC's type from the XML node.
  *
  * @param node XML node representing NIC data for a profile.
  * @return NIC type (which is "N/A" in case of problem).
  */
Glib::ustring ProfileXMLNode::load_nic_type (const xmlpp::Node* node)
{
    if (node == NULL) {
        cerr << "ProfileXMLNode::load_nic_type: unknown node (NULL)" << endl;
        return "N/A";
    }
    Glib::ustring str = "";
    xmlpp::Node::NodeList list1 = node->get_children("type");
    for (xmlpp::Node::NodeList::iterator iter = list1.begin(); 
         iter != list1.end(); ++iter) {
        str = get_node_content (*iter);
    }
    return (str);
}

/**
  * @author Geoffroy Vallee.
  *
  * Load NIC's option from the XML node.
  *
  * @param node XML node representing NIC data for a profile.
  * @return NIC option (which is "N/A" in case of problem).
  */
Glib::ustring ProfileXMLNode::load_nic_option (xmlpp::Node* node)
{
    Glib::ustring option = "N/A";

    if (node == NULL) {
        cerr << "ProfileXMLNode::load_nic_option: unknown node (NULL)" 
             << endl;
        return "N/A";
    }

    Glib::ustring str = "";
    xmlpp::Node::NodeList list1 = node->get_children("type");
    for (xmlpp::Node::NodeList::iterator iter = list1.begin(); 
         iter != list1.end(); ++iter) {
        xmlpp::Element *el = new xmlpp::Element ((*iter)->cobj());
        xmlpp::Attribute *attr = el->get_attribute ("option");
        if (attr) {
            option = attr->get_value();
        }
        return (option);
    }
    return "N/A";
}


/**
  * @author Geoffroy Vallee.
  *
  * Load virtual disk ID from XML node. The id is an attribute of the "virtual
  * disk" XML node.
  *
  * @param node XML node representing the image disk path.
  * @return virtual disk id (string), "N/A" if error.
  */
string ProfileXMLNode::load_virtual_disk_id (xmlpp::Node* node)
{
    string id = "N/A";
    xmlpp::Element *el = new xmlpp::Element (node->cobj());
    xmlpp::Attribute *attr = el->get_attribute ("id");
    if (attr) {
        id = attr->get_value();
        cout << "Virtual disk id found (" << id << ")" << endl;
    } else {
        cout << "ERROR: virtual disk id not found" << endl;
    }
    return id;
}

/**
  * @author Geoffroy Vallee.
  *
  * Add virtual disk info into the profile_data_t struct of the object.
  *
  * @param id Virtual disk id.
  * @param location Virtual disk location.
  */
void ProfileXMLNode::add_virtual_disk_in_list(string id, string location)
{
    virtual_fs_t new_elt;
    new_elt.id = id;
    new_elt.location = location;
    (data.list_virtual_fs).push_back(new_elt);
}

/**
  * @author Geoffroy Vallee.
  *
  * Load profile's virtual disk info from XML node.
  *
  * @param node XML node representing the virtual disk info.
  * @return 0 if success, -1 else.
  */
int ProfileXMLNode::load_virtual_disk_info_from_node (const xmlpp::Node* node)
{
    xmlpp::Node::NodeList list = node->get_children("virtual_disk");
    // We have one or more virtual disk from there
    if (list.size() <= 0) {
        cerr << "ERROR: the definition of virtual disks is not correct" << endl;
        cerr << "\t" << list.size() << " virtual disks is declared" << endl;
        return -1;
    }
    for (xmlpp::Node::NodeList::iterator iter = list.begin(); 
         iter != list.end(); ++iter) {
        string id, location;
        // We get the id of each virtual disk
        id = load_virtual_disk_id (*iter);
        if (id.compare ("N/A") == 0) {
            cerr << "ERROR loading the virtual disk ID" << endl;
            return -1;
        }
        // We get the location of each virtual disk
        location = get_node_content (*iter);
        cout << "Virtual disk location is " << location << endl;
        // We include info of the new virtual disk within the list of the 
        // profile's data
        add_virtual_disk_in_list (id, location);
    }
    return 0;
}

/**
  * @author Geoffroy Vallee.
  *
  * Load profile's virtual disks info from XML node.
  *
  * @return: 0 if success, -1 else.
  */
int ProfileXMLNode::load_virtual_disks_info_from_node ()
{
    // We can have zero or one virtual_disks tag
    xmlpp::Node::NodeList list = profile_node->get_children("virtual_disks");
    if (list.size() < 0 || list.size() > 1) {
        cerr << "ERROR: the definition of virtual disks is not correct" << endl;
        cerr << "\tvirtual disks are defined " << list.size() << " time(s)" 
             << endl;
        return -1;
    }
    if (list.size() == 0)
        // no virtual disks, we just exit
        return 0;
    cout << "Virtual disks declaration found" << endl;
    xmlpp::Node::NodeList::iterator iter = list.begin();
    if (load_virtual_disk_info_from_node (*iter)) {
        cerr << "ERROR loading information of virtual disks" << endl;
        return -1;
    }
    return 0;
}

/**
  * @author Geoffroy Vallee.
  *
  * Load profile's nics info from XML node.
  *
  * @return: 0 if success, -1 else.
  */
int ProfileXMLNode::load_profile_nics_info_from_node ()
{
    /* a revoir, interface trop bas niveau pour la classe ProfileXMLNode */
    Glib::ustring str1, str2, str3, str4, str5, str6;

    xmlpp::Node::NodeList list1 = profile_node->get_children("nic1");
    for (xmlpp::Node::NodeList::iterator iter = list1.begin(); 
         iter != list1.end(); ++iter) {
        str1 = load_nic_mac (*iter);
        str2 = load_nic_type (*iter);
       str5 = load_nic_option (*iter);
    }
    xmlpp::Node::NodeList list2 = profile_node->get_children("nic2");
    for (xmlpp::Node::NodeList::iterator iter = list2.begin(); 
         iter != list2.end(); ++iter) {
        str3 = load_nic_mac (*iter);
        str4 = load_nic_type (*iter);
       str6 = load_nic_option (*iter);
    }

    /* We check loaded dara */
    if (check_nic_type (str2) == NIC_TYPE_UNVALID)
        return -1;
    if (check_nic_type (str4) == NIC_TYPE_UNVALID)
        return -1;
    if (check_mac_address (str1))
        return -1;
    if (check_mac_address (str3))
        return -1;

    data.nic1_mac = str1.c_str();
    data.nic1_type = str2.c_str();
    data.nic1_option = str5.c_str();
    data.nic2_mac = str3.c_str();
    data.nic2_type = str4.c_str();
    data.nic2_option = str6.c_str();

    cout << "Nic1 type: " << data.nic1_type << endl;
    cout << "Nic1 MAC: " << data.nic1_mac << endl;
    cout << "Nic1 option: " << data.nic1_option << endl;

    cout << "Nic2 type: " << data.nic2_type << endl;
    cout << "Nic2 MAC: " << data.nic2_mac << endl;
    cout << "Nic2 option: " << data.nic2_option << endl;

    return 0;
}

/** @author Geoffroy Vallee.
  *
  * Get profile XML node.
  *
  * @return XML node of the profile.
  */
xmlpp::Node* ProfileXMLNode::get_profile_node ()
{
    return profile_node;
}

/** @author Geoffroy Vallee.
  *
  * Create a new XML doument for the new profile, this document may be then
  * important in the DOM XML tree for profiles.
  *
  * @param profile_name Profile's name.
  * @param profile_type Profile's type.
  * @param profile_image Path of the profile's image.
  * @param typeNic1 Type of the first NIC.
  * @param macNic1 MAC address of the first NIC.
  * @param typeNic2 Typs of the second NIC.
  * @param macNic2 MAC address of the second NIC.
  * @return Structure representing the DOM tree of the XML profile.
  */
xmlpp::Document* ProfileXMLNode::create_node_for_new_profile (Glib::ustring profile_name, Glib::ustring profile_type, Glib::ustring profile_image, Glib::ustring typeNic1, Glib::ustring macNic1, Glib::ustring typeNic2, Glib::ustring macNic2)
{
    /* first of all we check some parameters */
    if (check_nic_type (typeNic1) == NIC_TYPE_UNVALID) {
        return NULL;
    }
    if (check_nic_type (typeNic2) == NIC_TYPE_UNVALID) {
        return NULL;
    }

    /* add the profile in the DOM tree */
    xmlpp::Document* document = new xmlpp::Document();
    if (document == NULL) {
        cerr << "ProfileXMLNode::create_node_for_new_profile: impossible to "
             << "create a new XML document" << endl;
        return NULL;
    }
    document->set_internal_subset("example_xml_doc", "", "example_xml_doc.dtd");
    /* to befixed */

    //foo is the default namespace prefix.
    xmlpp::Element* root = document->create_root_node("root", "", "");
    //Declares the namespace and uses its prefix for this node
    if (root == NULL) {
        cerr << "ProfileXMLNode::create_node_for_new_profile: impossible to "
             << "get the root node of the document" << endl;
        return NULL;
    }
    root->set_namespace_declaration("", ""); // Also associate this prefix with
                                             // this namespace: 

    xmlpp::Element* profile = root->add_child("profile", "");
    if (profile == NULL) {
        cerr << "ProfileXMLNode::create_node_for_new_profile: impossible to "
             << "add a profile element into the XML tree" << endl;
        return NULL;
    }
    profile->set_namespace_declaration("", "");

    xmlpp::Element* name = profile->add_child("name", "");
    if (name == NULL) {
        cerr << "ProfileXMLNode::create_node_for_new_profile: impossible to "
             << "sadd a name element into the XML tree" << endl;
        return NULL;
    }
    name->set_namespace_declaration("", "");
    name->set_child_text(profile_name);

    xmlpp::Element* type = profile->add_child("type", "");
    if (type == NULL) {
        cerr << "ProfileXMLNode::create_node_for_new_profile: impossible to "
             << "add a type element into the XML tree" << endl;
        return NULL;
    }
    type->set_namespace_declaration("", "");
    type->set_child_text(profile_type);

    xmlpp::Element* image = profile->add_child("image", "");
    if (image == NULL) {
        cerr << "ProfileXMLNode::create_node_for_new_profile: impossible to "
             << "add an image element into the XML tree" << endl;
        return NULL;
    }
    image->set_namespace_declaration("", "");
    image->set_child_text(profile_image);

    xmlpp::Element* nic1 = profile->add_child("nic1", "");
    if (nic1 == NULL) {
        cerr << "ProfileXMLNode::create_node_for_new_profile: impossible to "
             << "add a nic1 element into the XML tree" << endl;
        return NULL;
    }
    nic1->set_namespace_declaration("", "");

    xmlpp::Element* nic1_type = nic1->add_child("type", "");
    if (nic1_type == NULL) {
        cerr << "ProfileXMLNode::create_node_for_new_profile: impossible to "
             << "add a nic1_type element into the XML tree" << endl;
        return NULL;
    }
    nic1_type->set_namespace_declaration("", "");
    if (typeNic1.empty() == TRUE) {
        nic1_type->set_child_text("N/A");
    } else {
        nic1_type->set_child_text(typeNic1);
    }

    xmlpp::Element* nic1_mac = nic1->add_child("mac", "");
    if (nic1_mac == NULL) {
        cerr << "ProfileXMLNode::create_node_for_new_profile: impossible to "
             << "add a nic1_mac element into the XML tree" << endl;
        return NULL;
    }
    nic1_mac->set_namespace_declaration("", "");
    if (macNic1.empty() == TRUE) {
        nic1_mac->set_child_text("N/A");
    } else {
        nic1_mac->set_child_text(macNic1);
    }

    xmlpp::Element* nic2 = profile->add_child("nic2", "");
    if (nic2 == NULL) {
        cerr << "ProfileXMLNode::create_node_for_new_profile: impossible to "
                "add a nic2 element into the XML tree" << endl;
        return NULL;
    }
    nic2->set_namespace_declaration("", "");

    xmlpp::Element* nic2_type = nic2->add_child("type", "");
    if (nic2_type == NULL) {
        cerr << "ProfileXMLNode::create_node_for_new_profile: impossible to "
             << "add a nic2_type element into the XML tree" << endl;
        return NULL;
    }
    nic2_type->set_namespace_declaration("", "");
    if (typeNic2.empty() == TRUE) {
        nic2_type->set_child_text("N/A");
    } else {
        nic2_type->set_child_text(typeNic2);
    }

    xmlpp::Element* nic2_mac = nic2->add_child("mac", "");
    if (nic2_mac == NULL) {
        cerr << "ProfileXMLNode::create_node_for_new_profile: impossible to "
             << "add a nic2_mac element into the XML tree" << endl;
        return NULL;
    }
    nic2_mac->set_namespace_declaration("", "");
    if (macNic2.empty() == TRUE) {
        nic2_mac->set_child_text("N/A");
    } else {
        nic2_mac->set_child_text(macNic2);
    }

    return document;
}
