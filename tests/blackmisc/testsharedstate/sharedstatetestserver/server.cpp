// SPDX-FileCopyrightText: Copyright (C) 2020 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \cond PRIVATE_TESTS

/*!
 * \file
 * \ingroup testblackmisc
 */

#include "../testsharedstate.h"
#include "blackmisc/sharedstate/datalinkdbus.h"
#include "blackmisc/registermetadata.h"
#include "blackmisc/dbusserver.h"
#include <QCoreApplication>
#include <QDBusConnection>

using namespace BlackMisc;
using namespace BlackMisc::SharedState;
using namespace BlackMiscTest;

//! \private
int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    BlackMisc::registerMetadata();
    CTestFilter::registerMetadata();

    CDBusServer server(CDBusServer::sessionBusAddress());
    CDataLinkDBus dataLink;
    dataLink.initializeLocal(&server);

    CTestScalarMutator scalarMutator(nullptr);
    CTestScalarJournal scalarJournal(nullptr);
    CTestListMutator listMutator(nullptr);
    CTestListJournal listJournal(nullptr);
    scalarMutator.initialize(&dataLink);
    scalarJournal.initialize(&dataLink);
    listMutator.initialize(&dataLink);
    listJournal.initialize(&dataLink);

    scalarMutator.setValue(42);
    for (int e = 1; e <= 6; ++e) { listMutator.addElement(e); }
    return app.exec();
}

//! \endcond