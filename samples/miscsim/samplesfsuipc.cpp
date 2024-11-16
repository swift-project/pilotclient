// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file
//! \ingroup samplemiscsim

#include "samplesfsuipc.h"

#include <QScopedPointer>
#include <QTextStream>

#include "misc/registermetadata.h"
#include "misc/simulation/simulatedaircraft.h"
#include "plugins/simulator/fscommon/fsuipc.h"

using namespace swift::misc;
using namespace swift::misc::simulation;

namespace swift::sample
{
#ifdef Q_OS_WIN
    void CSamplesFsuipc::samplesFsuipc(QTextStream &streamOut)
    {
        using namespace swift::simplugin::fscommon;

        swift::misc::registerMetadata();
        QScopedPointer<CFsuipc> fsuipc(new CFsuipc());
        streamOut << "FSUIPC initialized" << Qt::endl;

        if (fsuipc->open()) { streamOut << "FSUIPC connected" << Qt::endl; }
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
    void CSamplesFsuipc::samplesFsuipc(QTextStream &) {}
#endif

} // namespace swift::sample
