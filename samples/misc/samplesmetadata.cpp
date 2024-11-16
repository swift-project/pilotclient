// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file
//! \ingroup samplemisc

#include "samplesmetadata.h"

#include <stdio.h>

#include <QTextStream>

#include "misc/metadatautils.h"
#include "misc/registermetadata.h"

namespace swift::sample
{
    int CSamplesMetadata::samples()
    {
        QTextStream cin(stdin);
        QTextStream cout(stdout);
        swift::misc::registerMetadata();
        swift::misc::displayAllUserMetatypesTypes(cout);

        cin.readLine();
        cout << "------- Enter --------" << Qt::endl;
        return 0;
    }

} // namespace swift::sample
