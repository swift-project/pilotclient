/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

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
