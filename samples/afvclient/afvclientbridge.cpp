// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "afvclientbridge.h"

using namespace swift::core::afv::clients;

CAfvClientBridge::CAfvClientBridge(CAfvClient *afvClient, QObject *parent) : QObject(parent), m_afvClient(afvClient)
{
    connect(afvClient, &CAfvClient::receivingCallsignsChanged, this, &CAfvClientBridge::receivingCallsignsChanged);
    connect(afvClient, &CAfvClient::connectionStatusChanged, this, &CAfvClientBridge::connectionStatusChanged);
    connect(afvClient, &CAfvClient::updatedFromOwnAircraftCockpit, this,
            &CAfvClientBridge::updatedFromOwnAircraftCockpit);
    connect(afvClient, &CAfvClient::ptt, this, &CAfvClientBridge::ptt);
    connect(afvClient, &CAfvClient::inputVolumePeakVU, this, &CAfvClientBridge::inputVolumePeakVU);
    connect(afvClient, &CAfvClient::outputVolumePeakVU, this, &CAfvClientBridge::outputVolumePeakVU);
}
