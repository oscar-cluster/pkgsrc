/*
 *  Copyright (c) 2007 Oak Ridge National Laboratory, 
 *                     Geoffroy Vallee <valleegr@ornl.gov>
 *                     All rights reserved
 *  This file is part of the xoscar software, part of the OSCAR software.
 *  For license information, see the COPYING file in the top level directory
 *  of the OSCAR source.
 */

/**
 * @file XOSCAR_AboutAuthorsDialog.cpp
 * @brief Actual implementation of the XOSCAR_AboutAuthorsDialog class.
 * @author Geoffroy Vallee
 */

#include "XOSCAR_AboutAuthorsDialog.h"

using namespace xoscar;

XOSCAR_AboutAuthorsDialog::XOSCAR_AboutAuthorsDialog(QDialog *parent)
    : QDialog (parent)
{
    setupUi(this);
}

XOSCAR_AboutAuthorsDialog::~XOSCAR_AboutAuthorsDialog ()
{
}

