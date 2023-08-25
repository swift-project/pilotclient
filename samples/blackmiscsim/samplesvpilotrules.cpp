// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file
//! \ingroup sampleblackmiscsim

#include "samplesvpilotrules.h"
#include "blackmisc/registermetadata.h"
#include "blackmisc/simulation/fscommon/vpilotmodelruleset.h"
#include "blackmisc/simulation/fscommon/vpilotrulesreader.h"
#include "blackmisc/stringutils.h"

#include <QScopedPointer>
#include <QStringList>
#include <QTextStream>
#include <QtGlobal>

using namespace BlackMisc;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Simulation::FsCommon;

namespace BlackSample
{

    /*
     * Samples
     */
    void CSamplesVPilotRules::samples(QTextStream &streamOut, const QTextStream &streamIn)
    {
        BlackMisc::registerMetadata();
        QScopedPointer<CVPilotRulesReader> vPilotReader(new CVPilotRulesReader());
        bool s = vPilotReader->read(false);
        streamOut << "Read success: " << BlackMisc::boolToYesNo(s) << Qt::endl;
        streamOut << "Read " << vPilotReader->countRulesLoaded() << " rules from " << vPilotReader->countFilesLoaded() << " files" << Qt::endl;
        streamOut << "Distributors: " << vPilotReader->getRules().getSortedDistributors().join(", ");

        Q_UNUSED(streamIn);
    }

} // namespace
