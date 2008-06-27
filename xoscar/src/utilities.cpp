#include "utilities.h"

using namespace xoscar;

/**
 * @author Geoffroy Vallee.
 *
 * Utility function: convert an integer to a standard string.
 *
 * @param i Integer to convert in string.
 * @return Standard string representing the integer.
 */
string Utilities::intToStdString (int i)
{
    std::stringstream ss;
    std::string str;
    ss << i;
    ss >> str;
    return str;
}
