/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "samplesvpilotrules.h"
#include "blackmisc/simulation/fscommon/vpilotrulesreader.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include "blackmisc/sampleutils.h"

#include <QDebug>
#include <QScopedPointer>

using namespace BlackMisc;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Simulation::FsCommon;

namespace BlackSimTest
{

    /*
     * Samples
     */
    void CSamplesVPilotRules::samples(QTextStream &streamOut, QTextStream &streamIn)
    {
        BlackMisc::registerMetadata();
        QScopedPointer<CVPilotRulesReader> vPilotReader(new CVPilotRulesReader());
        bool s = vPilotReader->read();
        streamOut << "Read success: " << BlackMisc::boolToYesNo(s) << endl;
        streamOut << "Read " << vPilotReader->countRulesLoaded() << " rules from " << vPilotReader->countFilesLoaded() << " files" << endl;
        streamOut << "Distributors: " << vPilotReader->getRules().getSortedDistributors().join(", ");

        Q_UNUSED(streamIn);
    }

} // namespace
