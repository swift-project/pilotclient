/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_METADATAUTILS_H
#define BLACKMISC_METADATAUTILS_H

#include "blackmisc/blackmiscexport.h"

#include <stddef.h>
#include <QString>

class QMetaType;
class QTextStream;
struct QMetaObject;

//! Metadata util functions
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

} // ns

#endif // guard
