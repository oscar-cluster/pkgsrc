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

#include <stdarg.h>
#include <cstdlib>
#include <vector>

using namespace std;

/**
 * @namespace xoscar
 * @author Geoffroy Vallee.
 * @brief The xoscar namespace gathers all classes needed for XOSCAR.
 */
namespace xoscar {

/**
 * Type storing the key and the value of each element within a hash (via a
 * structure) .
 */
typedef struct hash_elt {
    /** Key of the hash element */
    string key;
    /** Value of the hash element */
    string value;
} hash_elt_t;

typedef vector<hash_elt>::iterator hash_t;

/**
 * @class Hash
 *
 * There two ways to create a new hash:
 * - during initialization, for instance the following code creates a hash w/
 *   two elements, the first one has for key 'key1' and for value 'value1', and
 *   the second one has for key 'key2' and for value 'value2'.
 *   \code
 *   Hash my_hash = Hash ("key1", "value1", "key2", "value2", NULL);
 *   \endcode
 *   WARNING: you must put NULL to end the hash definition.
 * - add hash elements after the hash creation. For instance, the following
 *   code creates a hash and then we add one element for which the key is 
 *   'key1' and the value 'value1'.
 *   \code
 *   Hash my_hash = Hash ();
 *   my_hash.add ("key1", "value1");
 *   \endcode
 */
class Hash 
{
public:
    Hash (const char *str, ...);
    Hash ();
    ~Hash ();
    int print ();
    int add (string key, string value);
    string value (string key);
    unsigned int size ();
    hash_elt_t at (unsigned int);

private:
    vector<hash_elt> hash;

    int is_valid(va_list);
};

} // namespace xoscar

#endif // HASH_H
