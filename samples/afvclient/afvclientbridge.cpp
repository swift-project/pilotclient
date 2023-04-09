/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "afvclientbridge.h"

using namespace BlackCore::Afv::Clients;

CAfvClientBridge::CAfvClientBridge(CAfvClient *afvClient, QObject *parent) : QObject(parent),
                                                                             m_afvClient(afvClient)
{
    connect(afvClient, &CAfvClient::receivingCallsignsChanged, this, &CAfvClientBridge::receivingCallsignsChanged);
    connect(afvClient, &CAfvClient::connectionStatusChanged, this, &CAfvClientBridge::connectionStatusChanged);
    connect(afvClient, &CAfvClient::updatedFromOwnAircraftCockpit, this, &CAfvClientBridge::updatedFromOwnAircraftCockpit);
    connect(afvClient, &CAfvClient::ptt, this, &CAfvClientBridge::ptt);
    connect(afvClient, &CAfvClient::inputVolumePeakVU, this, &CAfvClientBridge::inputVolumePeakVU);
    connect(afvClient, &CAfvClient::outputVolumePeakVU, this, &CAfvClientBridge::outputVolumePeakVU);
}
