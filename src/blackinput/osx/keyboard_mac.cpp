/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "keyboard_mac.h"
#include <QDebug>

using namespace BlackMisc::Hardware;

namespace BlackInput
{
    CKeyboardMac::CKeyboardMac(QObject *parent) :
        IKeyboard(parent),
        m_mode(Mode_Nominal)
    {
    }

    CKeyboardMac::~CKeyboardMac()
    {
    }

    bool CKeyboardMac::init()
    {
        return true;
    }

    void CKeyboardMac::setKeysToMonitor(const CKeyboardKeyList &keylist)
    {
        m_listMonitoredKeys = keylist;
    }

    void CKeyboardMac::startCapture(bool ignoreNextKey)
    {
        m_mode = Mode_Capture;
        m_ignoreNextKey = ignoreNextKey;
        m_pressedKey.setKeyObject(CKeyboardKey());
    }

    void CKeyboardMac::triggerKey(const CKeyboardKey &key, bool isPressed)
    {
        if(!isPressed) emit keyUp(key);
        else emit keyDown(key);
    }

    void CKeyboardMac::sendCaptureNotification(const CKeyboardKey &key, bool isFinished)
    {
        if (isFinished)
            emit keySelectionFinished(key);
        else
            emit keySelectionChanged(key);
    }
}
