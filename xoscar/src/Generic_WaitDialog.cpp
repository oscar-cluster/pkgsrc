/*
 *  Copyright (c) 2007-2008 Oak Ridge National Laboratory, 
 *                          Geoffroy Vallee <valleegr@ornl.gov>
 *                          All rights reserved
 *  This file is part of the xoscar software, part of the OSCAR software.
 *  For license information, see the COPYING file in the top level directory
 *  of the OSCAR source.
 */

/**
 * @file Generic_WaitDialog.cpp
 * @brief Actual implementation of the GenericWaitDialog class.
 * @author Robert Babilon
 */

#include <QTimer>

#include "Generic_WaitDialog.h"

using namespace xoscar;

GenericWaitDialog::GenericWaitDialog(QWidget *parent, QString command_detail)
    : QDialog (parent)
{
    setupUi(this);
    commandDetailLabel->setText(command_detail);
}

GenericWaitDialog::~GenericWaitDialog ()
{
}

/**
 * @author Robert Babilon
 *
 * Method to start the QTimer::singleShot and reset the timer flags.
 *
 * @param waitTime The time in milliseconds to set the timer.
 */
void GenericWaitDialog::startTimer(int waitTime/*=700*/)
{
    timerFlag = false;
    threadFlag = false;
    QTimer::singleShot(waitTime, this, SLOT(timerNotify()));
}

/**
 * @author Robert Babilon
 *
 * Slot that is called when the QTimer::singleShot is done.
 */
void GenericWaitDialog::timerNotify()
{
    timerFlag = true;
    checkFlags();
}

/**
 * @author Robert Babilon
 * Slot called by the parent widget to inform the dialog the thread is finished.
 */
void GenericWaitDialog::threadNotify()
{
    threadFlag = true;
    checkFlags();
}

/**
 * @author Robert Babilon
 *
 * Method to check the flags and hide the dialog if both flags are true.
 */
void GenericWaitDialog::checkFlags()
{
    if(timerFlag && threadFlag) this->hide();
}

/**
 * @author Robert Babilon
 *
 * Method to set the text to the label
 */
void GenericWaitDialog::setLabelText(QString message)
{
    commandDetailLabel->setText(message);
}

/**
 * @author Robert Babilon
 * Override the Qt closeEvent to prevent the user from closing the dialog.
 * @param event The QCloseEvent holding details about the close event.
 */
void GenericWaitDialog::closeEvent(QCloseEvent* event)
{
    if(timerFlag && threadFlag) event->accept();
    else event->ignore();
}
