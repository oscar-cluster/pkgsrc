/*
 *  Copyright (c) 2007-2008 Oak Ridge National Laboratory, 
 *                          Geoffroy Vallee <valleegr@ornl.gov>
 *                          All rights reserved
 *  This file is part of the xoscar software, part of the OSCAR software.
 *  For license information, see the COPYING file in the top level directory
 *  of the OSCAR source.
 */

/**
 * @file XOSCAR_TabSoftwareConfiguration.cpp
 * @brief Actual implementation of the XOSCAR_TabSoftwareConfiguration class.
 * @author Robert Babilon
 */

#include "XOSCAR_TabSoftwareConfiguration.h"

using namespace xoscar;

XOSCAR_TabSoftwareConfiguration::XOSCAR_TabSoftwareConfiguration(QWidget* parent)
    : QWidget(parent)
{
    setupUi(this);
}

XOSCAR_TabSoftwareConfiguration::~XOSCAR_TabSoftwareConfiguration()
{
}
