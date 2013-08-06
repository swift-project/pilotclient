/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "atc_manager.h"

/**
 * Constructor
 */
CAtcManager::CAtcManager(QObject *parent) :
    QObject(parent)
{
}

/**
 * Return QStringList of controller callsigns
 */
QStringList CAtcManager::atcList() const
{
    return m_atcList;
}

void CAtcManager::addATC(const QString &controller)
{
    if (m_atcList.indexOf(controller) == -1)
        m_atcList << controller;
}
