// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file
//! \ingroup sampleblackmisc

#include "samplesmetadata.h"
#include "blackmisc/metadatautils.h"
#include "blackmisc/registermetadata.h"

#include <stdio.h>
#include <QTextStream>

namespace BlackSample
{
    int CSamplesMetadata::samples()
    {
        QTextStream cin(stdin);
        QTextStream cout(stdout);
        BlackMisc::registerMetadata();
        BlackMisc::displayAllUserMetatypesTypes(cout);

        cin.readLine();
        cout << "------- Enter --------" << Qt::endl;
        return 0;
    }

} // namespace
