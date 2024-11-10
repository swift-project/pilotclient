// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file
//! \ingroup samplemiscsim

#include "samplesvpilotrules.h"
#include "misc/registermetadata.h"
#include "misc/simulation/fscommon/vpilotmodelruleset.h"
#include "misc/simulation/fscommon/vpilotrulesreader.h"
#include "misc/stringutils.h"

#include <QScopedPointer>
#include <QStringList>
#include <QTextStream>
#include <QtGlobal>

using namespace swift::misc;
using namespace swift::misc::simulation;
using namespace swift::misc::simulation::fscommon;

namespace swift::sample
{

    /*
     * Samples
     */
    void CSamplesVPilotRules::samples(QTextStream &streamOut, const QTextStream &streamIn)
    {
        swift::misc::registerMetadata();
        QScopedPointer<CVPilotRulesReader> vPilotReader(new CVPilotRulesReader());
        bool s = vPilotReader->read(false);
        streamOut << "Read success: " << swift::misc::boolToYesNo(s) << Qt::endl;
        streamOut << "Read " << vPilotReader->countRulesLoaded() << " rules from " << vPilotReader->countFilesLoaded() << " files" << Qt::endl;
        streamOut << "Distributors: " << vPilotReader->getRules().getSortedDistributors().join(", ");

        Q_UNUSED(streamIn);
    }

} // namespace
