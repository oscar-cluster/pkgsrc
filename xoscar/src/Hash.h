/*
 *  Copyright (c) 2008 Oak Ridge National Laboratory, 
 *                     Geoffroy Vallee <valleegr@ornl.gov>
 *                     All rights reserved
 *  This file is part of the xoscar software.  For license information,
 *  see the COPYING file in the top level directory of the source
 */

/**
  * @file Hash.h
  * @brief This defines a class for the implementation of hashes.
  * @author Geoffroy Vallee.
  *
  * This defines a class for the implementation of hashes, similar to Perl 
  * hashes.
  */

#ifndef HASH_H
#define HASH_H

using namespace std;

typedef struct hash_elt {
    string key;
    string value;
} hash_elt_t;

typedef vector<hash_elt>::iterator hash_t;

class Hash 
{
public:
    Hash (const char *str, ...);
    Hash ();
    ~Hash ();
    int print ();
    int add (string key, string value);
    string value (string key);

private:
    vector<hash_elt> hash;

    int is_valid(va_list);
};

#endif // HASH_H
