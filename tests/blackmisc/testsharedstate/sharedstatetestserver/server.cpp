/* Copyright (C) 2020
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

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