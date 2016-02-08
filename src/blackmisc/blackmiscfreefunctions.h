/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_FREEFUNCTIONS_H
#define BLACKMISC_FREEFUNCTIONS_H

#include "blackmisc/blackmiscexport.h"
#include <QDir> // for Q_INIT_RESOURCE

/*!
 * Workaround, to call initResource from namespace. Used in BlackMisc::initResources().
 * Q_INIT_RESOURCE adds resource, here the translation files.
 */
inline void initBlackMiscResources()
{
    // cannot be declared within namespace, see docu
    // hence BlackMisc::initResources() calls this inline function
    Q_INIT_RESOURCE(blackmisc);
}

//! Free functions in BlackMisc
namespace BlackMisc
{
    //! Init resources
    BLACKMISC_EXPORT void initResources();
}

#endif // guard
