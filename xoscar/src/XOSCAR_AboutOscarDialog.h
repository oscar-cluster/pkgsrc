/*
 *  Copyright (c) 2007 Oak Ridge National Laboratory, 
 *                     Geoffroy Vallee <valleegr@ornl.gov>
 *                     All rights reserved
 *  This file is part of the xorm software, part of the OSCAR software.
 *  For license information, see the COPYING file in the top level directory
 *  of the OSCAR source.
 */

/**
 * @file XOSCAR_AboutOscarDialog.h
 * @brief Defines the class XOSCAR_AboutOscarDialog that implements a widget 
 *        that displays information about OSCAR.
 * @author Geoffroy Vallee
 */

#ifndef XOSCAR_ABOUTOSCARSDIALOG_H
#define XOSCAR_ABOUTAUTHORSDIALOG_H

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "ui_AboutOSCARDialog.h"

using namespace std;

namespace xoscar {

class XOSCAR_AboutOscarDialog : public QDialog, public Ui_AboutOscarDialog
{
Q_OBJECT

public:
    XOSCAR_AboutOscarDialog(QDialog *parent = 0);
    ~XOSCAR_AboutOscarDialog();

};

//namespace xoscar {
//    class XOSCAR_AboutOscarDialog: public Ui_AboutOscarDialog {};
} // namespace xorm


#endif // XOSCAR_ABOUTAUTHORSDIALOG_H
