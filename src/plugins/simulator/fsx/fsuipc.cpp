/* Copyright (C) 2013 VATSIM Community / contributors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "fsuipc.h"
#include <Windows.h>
// bug in FSUIPC_User.h, windows.h not included, so we have to import it first
#include "FSUIPC/FSUIPC_User.h"
#include <QDebug>

namespace BlackSimPlugin
{
    namespace Fsx
    {

        CFsuipc::CFsuipc() : m_connected(false)
        { }

        CFsuipc::~CFsuipc()
        {
            this->disconnect();
        }

        bool CFsuipc::connect()
        {
            DWORD result;
            this->m_lastErrorMessage = "";
            if (this->m_connected) return this->m_connected; // already connected
            if (FSUIPC_Open(SIM_ANY, &result))
            {
                this->m_connected = true;
                qDebug() << "FSUIPC connected";
            }
            else
            {
                this->m_connected = false;
                int index = static_cast<int>(result);
                this->m_lastErrorMessage = CFsuipc::errorMessages().at(index);
                qDebug() << "FSUIPC" << this->m_lastErrorMessage;
            }
            return this->m_connected;
        }

        void CFsuipc::disconnect()
        {
            FSUIPC_Close(); // Closing when it wasn't open is okay, so this is safe here
            this->m_connected = false;
        }
    }
}
