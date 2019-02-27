/* Copyright (C) 2018
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SIMULATION_FLIGHTGEAR_FGSWIFTBUSCONFIGWRITER_H
#define BLACKMISC_SIMULATION_FLIGHTGEAR_FGSWIFTBUSCONFIGWRITER_H

#include "blackmisc/blackmiscexport.h"

#include <QObject>

namespace BlackMisc
{
    namespace Simulation
    {
        namespace Flightgear
        {
            //! FGSwiftBus configuration file writer
            class BLACKMISC_EXPORT CFGSwiftBusConfigWriter : public QObject
            {
                Q_OBJECT

            public:
                //! Default constructor.
                CFGSwiftBusConfigWriter(QObject *parent = nullptr);

                //! Set new DBus address
                void setDBusAddress(const QString &dBusAddress);

                //! Update fgswiftbus.conf in all known X-Plane versions (XP9 - XP11 are supported)
                void updateInAllXPlaneVersions();

                //! Update xswiftbus.conf in X-Plane 9
                void updateInXPlane9();

                //! Update xswiftbus.conf in X-Plane 10
                void updateInXPlane10();

                //! Update xswiftbus.conf in X-Plane 11
                void updateInXPlane11();

                //! Write new xswiftbus.conf to filePath. Existing files are removed.
                void writeTo(const QString &filePath);

            private:
                QString m_dbusMode = "p2p";
                QString m_dbusAddress = "127.0.0.1";
                QString m_dbusPort = "45001";
            };
        }
    }
}

#endif
