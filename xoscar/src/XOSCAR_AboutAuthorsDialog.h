/*
 *  Copyright (c) 2007 Oak Ridge National Laboratory, 
 *                     Geoffroy Vallee <valleegr@ornl.gov>
 *                     All rights reserved
 *  This file is part of the xorm software, part of the OSCAR software.
 *  For license information, see the COPYING file in the top level directory
 *  of the OSCAR source.
 */

/**
 * @file XOSCAR_AboutAuthorsDialog.h
 * @brief Defines the class XOSCAR_AboutAuthorsDialog, a widget that displays
 *        information about xoscar authors.
 * @author Geoffroy Vallee
 *
 */

#ifndef XOSCAR_ABOUTAUTHORSDIALOG_H
#define XOSCAR_ABOUTAUTHORSDIALOG_H

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "ui_AboutAuthorsDialog.h"

using namespace std;

namespace xoscar {

class XOSCAR_AboutAuthorsDialog : public QDialog, public Ui_AboutAuthorsDialog
{
Q_OBJECT

public:
    XOSCAR_AboutAuthorsDialog(QDialog *parent = 0);
    ~XOSCAR_AboutAuthorsDialog();

};

// namespace xoscar {
//     class XOSCAR_AboutAuthorsDialog: public Ui_AboutAuthorsDialog {};
} // namespace xoscar


#endif // XOSCAR_ABOUTAUTHORSDIALOG_H
