// SPDX-FileCopyrightText: Copyright (C) 2020 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \cond PRIVATE_TESTS

/*!
 * \file
 * \ingroup testmisc
 */

#include <QCoreApplication>
#include <QDBusConnection>

#include "../testsharedstate.h"

#include "misc/dbusserver.h"
#include "misc/registermetadata.h"
#include "misc/sharedstate/datalinkdbus.h"

using namespace swift::misc;
using namespace swift::misc::shared_state;
using namespace MiscTest;

//! \private
int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    swift::misc::registerMetadata();
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
