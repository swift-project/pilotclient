// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file
//! \ingroup sampleblackmiscsim

#include "plugins/simulator/fscommon/fsuipc.h"
#include "samplesfsuipc.h"
#include "blackmisc/simulation/simulatedaircraft.h"
#include "blackmisc/registermetadata.h"

#include <QTextStream>
#include <QScopedPointer>

using namespace BlackMisc;
using namespace BlackMisc::Simulation;

namespace BlackSample
{
#ifdef Q_OS_WIN
    void CSamplesFsuipc::samplesFsuipc(QTextStream &streamOut)
    {
        using namespace BlackSimPlugin::FsCommon;

        BlackMisc::registerMetadata();
        QScopedPointer<CFsuipc> fsuipc(new CFsuipc());
        streamOut << "FSUIPC initialized" << Qt::endl;

        if (fsuipc->open())
        {
            streamOut << "FSUIPC connected" << Qt::endl;
        }
        else
        {
            streamOut << "FSUIPC NOT(!) connected" << Qt::endl;
            streamOut << "Need FS WideClient?" << Qt::endl;
            return;
        }

        CSimulatedAircraft aircraft;
        if (fsuipc->read(aircraft, true, true, true))
        {
            streamOut << "Aircraft read: " << aircraft.toQString(true) << Qt::endl;
        }

        fsuipc->close();
    }

#else
    void CSamplesFsuipc::samplesFsuipc(QTextStream &)
    {
    }
#endif

} // namespace
