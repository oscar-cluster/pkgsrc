/*
 *  Copyright (c) 2006-2007 Oak Ridge National Laboratory, 
 *                          Geoffroy Vallee <valleegr@ornl.gov>
 *                          All rights reserved
 *  This file is part of the KVMs software.  For license information,
 *  see the COPYING file in the top level directory of the source
 */

#include <iostream>
#include <stdarg.h>
#include <cstdlib>
#include <vector>

#include "Hash.h"

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

int Hash::add (string key, string value) {
    hash_elt_t elt;
    elt.key = key;
    elt.value = value;
    hash.push_back(elt);
    return 0;
}

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
