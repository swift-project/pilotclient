/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKCORE_FREEFUNCTIONS_H
#define BLACKCORE_FREEFUNCTIONS_H

#include "blackcoreexport.h"
#include <QObject>

namespace BlackCore
{
    //! Register all relevant metadata in BlackCore
    BLACKCORE_EXPORT void registerMetadata();

    //! Is the current thread the QObject's thread?
    //! \remarks can be used as ASSERT check for threaded objects
    BLACKCORE_EXPORT bool isCurrentThreadObjectThread(QObject *toBeTested);

    //! Is the application thread the QObject's thread?
    //! \remarks can be used as ASSERT check for threaded objects
    BLACKCORE_EXPORT bool isApplicationThreadObjectThread(QObject *toBeTested);

    //! Is the current thread the Application thread?
    //! \remarks can be used as ASSERT check for threaded objects
    BLACKCORE_EXPORT bool isCurrentThreadApplicationThread();

} // BlackCore

#endif // guard
