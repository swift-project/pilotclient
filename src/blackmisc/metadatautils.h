// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_METADATAUTILS_H
#define BLACKMISC_METADATAUTILS_H

#include "blackmisc/blackmiscexport.h"

#include <QString>
#include <stddef.h>

class QMetaType;
class QTextStream;
class QObject;
struct QMetaObject;

namespace BlackMisc
{
    //! Display all user metatypes
    //! \remarks Used in order to debug code, do not remove
    BLACKMISC_EXPORT void displayAllUserMetatypesTypes(QTextStream &out);

    //! Get all user metatypes
    BLACKMISC_EXPORT QString getAllUserMetatypesTypes(const QString &separator = "\n");

    //! Real heap size of an object
    BLACKMISC_EXPORT size_t heapSizeOf(const QMetaType &type);

    //! Real heap size of an object
    BLACKMISC_EXPORT size_t heapSizeOf(const QMetaObject &objectType);

    //! Class name as from QMetaObject::className with namespace
    BLACKMISC_EXPORT QString className(const QObject *object);

    //! Class name as from QMetaObject::className without namespace
    BLACKMISC_EXPORT QString classNameShort(const QObject *object);

} // ns

#endif // guard
