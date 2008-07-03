/*
 *  Copyright (c) 2007-2008 Oak Ridge National Laboratory, 
 *                          Geoffroy Vallee <valleegr@ornl.gov>
 *                          All rights reserved
 *  This file is part of the xoscar software, part of the OSCAR software.
 *  For license information, see the COPYING file in the top level directory
 *  of the OSCAR source.
 */

/**
 * @file XOSCAR_TabSoftwareConfiguration.h
 * @brief Defines the class XOSCAR_TabSoftwareConfiguration for the software
 * configuration tab.
 * @author Robert Babilon
 *
 */
#ifndef XOSCAR_TABSOFTWARECONFIGURATION_H
#define XOSCAR_TABSOFTWARECONFIGURATION_H

#include "ui_xoscar_softwareconfiguration.h"

#include <QWidget>

using namespace Ui;

namespace xoscar {

class XOSCAR_TabSoftwareConfiguration : public QWidget, public SoftwareConfigurationForm
{
Q_OBJECT

public:
    XOSCAR_TabSoftwareConfiguration(QWidget* parent);
    ~XOSCAR_TabSoftwareConfiguration();
};

}
#endif //XOSCAR_TABSOFTWARECONFIGURATION_H
