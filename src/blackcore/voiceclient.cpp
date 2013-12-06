/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "voiceclient.h"

namespace BlackCore
{
    IVoiceClient::IVoiceClient(QObject *parent)
        : QObject(parent)
    {
        qRegisterMetaType<IVoiceClient::ComUnit>("IVoiceClient::ComUnit");
    }
}
