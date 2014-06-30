/* Copyright (C) 2013 VATSIM Community / contributors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKSIMPLUGIN_FSUIPC_H
#define BLACKSIMPLUGIN_FSUIPC_H

#include "blackmisc/avaircraft.h"
#include "blackmisc/nwaircraftmodel.h"
#include <QStringList>

namespace BlackSimPlugin
{
    namespace Fsx
    {
        //! Class representing a FSUIPC "interface"
        class CFsuipc
        {
        public:

            //! Constructor
            CFsuipc();

            //! Destructor
            ~CFsuipc();

            //! Open conenction with FSUIPC
            bool connect();

            //! Disconnect
            void disconnect();

            //! Is connected?
            bool isConnected() const { return m_connected; }

            //! Valid read values
            bool validReadValues() const { return m_validReadValues; }

            //! Process reading and writing variables
            void process();

            //! Error messages
            static const QStringList &errorMessages()
            {
                static const QStringList errors(
                {
                    "Okay",
                    "Attempt to Open when already Open",
                    "Cannot link to FSUIPC or WideClient",
                    "Failed to Register common message with Windows",
                    "Failed to create Atom for mapping filename",
                    "Failed to create a file mapping object",
                    "Failed to open a view to the file map",
                    "Incorrect version of FSUIPC, or not FSUIPC",
                    "Sim is not version requested",
                    "Call cannot execute, link not Open",
                    "Call cannot execute: no requests accumulated",
                    "IPC timed out all retries",
                    "IPC sendmessage failed all retries",
                    "IPC request contains bad data",
                    "Maybe running on WideClient, but FS not running on Server, or wrong FSUIPC",
                    "Read or Write request cannot be added, memory for Process is full",
                }
                );
                return errors;
            }

            //! Simulators
            static const QStringList &simulators()
            {
                static const QStringList sims(
                {
                    "any", "FS98", "FS2000", "CFS2", "CFS1", "Fly!", "FS2002", "FS2004", "FSX", "ESP", "P3D"
                }
                );
                return sims;
            }

        private:
            bool m_connected;
            bool m_validReadValues;
            QString m_lastErrorMessage;
            QString m_fsuipcVersion;
            BlackMisc::Aviation::CAircraft m_aircraft; //!< FSUIPC read aircraft
            BlackMisc::Network::CAircraftModel m_model; //!< FSUIPC read model

            //! Read data from FSUIPC
            void read();

            //! Integer representing fractional
            static double intToFractional(double fractional);

        };
    }
}

#endif // guard
