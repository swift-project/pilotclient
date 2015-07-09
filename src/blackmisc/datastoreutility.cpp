/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/datastoreutility.h"
#include "blackmisc/blackmiscfreefunctions.h"

namespace BlackMisc
{
    bool CDatastoreUtility::dbBoolStringToBool(const QString &dbBool)
    {
        return BlackMisc::stringToBool(dbBool);
    }

    int CDatastoreUtility::extractIntegerKey(const QString &stringWithKey)
    {
        int i1 = stringWithKey.lastIndexOf('(');
        if (i1 < 0) { return -1; }
        int i2 = stringWithKey.lastIndexOf(')');
        if (i2 <= i1 + 1) { return -1;}
        QString n(stringWithKey.mid(i1 + 1, i2 - i1 - 1));
        bool ok = false;
        int key = n.toInt(&ok);
        return ok ? key : -1;
    }
} // namespace
