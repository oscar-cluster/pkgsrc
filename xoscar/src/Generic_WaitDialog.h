/*
 *  Copyright (c) 2007 Oak Ridge National Laboratory, 
 *                     Geoffroy Vallee <valleegr@ornl.gov>
 *                     All rights reserved
 *  This file is part of the xoscar software, part of the OSCAR software.
 *  For license information, see the COPYING file in the top level directory
 *  of the OSCAR source.
 */

/**
 * @file Generic_WaitDialog.h
 * @brief Defines the class GenericWaitDialog that implements a widget that asks 
 *        users to wait during the execution of OSCAR commands.
 * @author Robert Babilon
 */

#ifndef GENERIC_WAITDIALOG_H
#define GENERIC_WAITDIALOG_H

#include <iostream>

#include <QCloseEvent>

#include "ui_GenericWaitDialog.h"

using namespace std;

namespace xoscar {

class GenericWaitDialog : public QDialog, public Ui_GenericWaitDialog
{
Q_OBJECT

public:
    GenericWaitDialog(QWidget *parent = 0, QString command_detail = "");
    ~GenericWaitDialog();

    void startTimer(int waitTime = 700);
    void checkFlags();
    void setLabelText(QString message);

public slots:
    void timerNotify();
    void threadNotify();

protected:
	void closeEvent(QCloseEvent* event);

private:
    /** Indicates when the QTimer has finished */
    bool timerFlag;
    /** Inidcates when the parent's thread has finished */
    bool threadFlag;
};

} // namespace xoscar


#endif // GENERIC_WAITDIALOG_H
