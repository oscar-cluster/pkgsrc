/*
 *  Copyright (c) 2007 Oak Ridge National Laboratory, 
 *                     Geoffroy Vallee <valleegr@ornl.gov>
 *                     All rights reserved
 *  This file is part of the xoscar software, part of the OSCAR software.
 *  For license information, see the COPYING file in the top level directory
 *  of the OSCAR source.
 */

/**
 * @file ORM_AddRepoGUI.h
 * @brief Defines the class ORMAddRepoDialog that implements a widget that 
 *        allows users to enter a new OSCAR repository.
 * @author Geoffroy Vallee
 *
 * The file defines the widget for the dialog which allows user to enter a new
 * OSCAR repository URL. For that it inherents from the class generated from the
 * .ui file created with QtDesigner.
 */

#ifndef ORM_ADDREPOGUI_H
#define ORM_ADDREPOGUI_H

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <fstream>
#include <unistd.h>

#include "ui_AddRepoWidget.h"
#include "CommandExecutionThread.h"

using namespace std;
using namespace redi;

/**
 * @namespace xoscar
 * @author Geoffroy Vallee
 * @brief The xoscar namespace gathers all classes needed for XOSCAR.
 */
namespace xoscar {

class ORMAddRepoDialog : public QDialog, public Ui_AddRepoDialog
{
Q_OBJECT

public:
    ORMAddRepoDialog(QDialog *parent = 0);
    ~ORMAddRepoDialog();

public slots:
     int handle_thread_result (int, QString);

private:
    CommandExecutionThread command_thread;
};

    class XORM_AddRepoDialog: public ORMAddRepoDialog {};
} // namespace xoscar

#endif // ORM_ADDREPOGUI_H
