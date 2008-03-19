/*
 *  Copyright (c) 2008 Oak Ridge National Laboratory, 
 *                     Geoffroy Vallee <valleegr@ornl.gov>
 *                     All rights reserved
 *  This file is part of the KVMs software.  For license information,
 *  see the COPYING file in the top level directory of the source
 */

/**
 * @file Hash.cpp
 * @brief Actual implementation of the Hash class.
 * @author Geoffroy Vallee
 */

#include <iostream>

#include "Hash.h"

using namespace xoscar;

Hash::Hash (const char* str, ...)
{
    const char *str2;
    va_list ap, list;
    va_start (ap, str);
    va_copy (list, ap);

    if (is_valid (list) == 0) {
        cout << "ERROR: Invalid hash." << endl;
        return;
    }

    // The first elt is the value of the first key
    str2 = va_arg(ap, const char*);
    hash_elt_t elt;
    elt.key = str;
    elt.value = str2;
    hash.push_back(elt);

    while (str) {
        str = va_arg (ap, const char*);
        str2 = va_arg (ap, const char*);
        if (str == NULL) {
            return;
        }
        hash_elt_t elt2;
        elt2.key = str;
        elt2.value = str2;
        hash.push_back(elt2);
    }
    va_end (ap);
}

Hash::Hash ()
{
}

Hash::~Hash ()
{
}

unsigned int Hash::size ()
{
    return (hash.size());
}

hash_elt_t Hash::at (unsigned int pos) {
    return hash.at (pos);
}

/**
 * @author Geoffroy Vallee
 *
 * A hash is defined by a list of string elements, with the idea that we have
 * each time a key and a value. It seems that the arguments when defining a new
 * hash are always even. This function checks this kind of things, to be sure
 * we can effectively create the hash.
 *
 * @param ap List of argument. This list is based on the stdarg library, refer
 *           to the documentation for more details.
 * @return 1 is the hash definition is valid, 0 else.
 */
int Hash::is_valid (va_list ap)
{
    int count = 0;
    const char* str;

    // We check if we have a even or uneven number of args
    for (;;) {
        str = va_arg(ap, const char*);
        if (str == 0) {
            break;
        } else {
            count ++;
        }
    }
    count++;
    div_t rest = div (count, 2);
    if(rest.rem == 0) {
        // Nb of args is even
        return 1;
    } else {
        // Nb of args is uneven
        return 0;
    }
}

/**
 * @author Geoffroy Vallee
 *
 * Display the content of the hash.
 *
 * @return 1 if an error occurs during the display of the hash, 0 else.
 */
int Hash::print () {
    cout << "(";
    for (unsigned int i=0; i < hash.size(); i++) {
        hash_elt_t elt = hash.at(i);
        cout << elt.key << ": " << elt.value;
        if (i <  hash.size()-1) {
            cout << ", ";
        }
    }
    cout << ")" << endl;
    return 0;
}

/**
 * @author Geoffroy Vallee
 *
 * Add a element to an existing hash.
 *
 * @param key Key of the element to add to the hash.
 * @param value Value of the element to add to the hash.
 */
int Hash::add (string key, string value) {
    hash_elt_t elt;
    elt.key = key;
    elt.value = value;
    hash.push_back(elt);
    return 0;
}

/**
 * @author Geoffroy Vallee
 *
 * Returns the value of a specific element of a hash, based on the element key.
 *
 * @param key Key of the element for which we have to return the value.
 * @return String representing the value of the element we are looking for.
 */
string Hash::value (string key) {
    unsigned int i = 0;
    while (i < hash.size()) {
        hash_elt_t elt = hash.at (i);
        if (elt.key.compare(key) == 0) {
            return elt.value;
        }
        i++;
    }
    return 0;
}
