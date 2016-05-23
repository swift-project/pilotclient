/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/db/dbflags.h"
#include "blackmisc/verify.h"

#include <QStringList>
#include <QtGlobal>

namespace BlackMisc
{
    namespace Db
    {
        QString CDbFlags::flagToString(CDbFlags::DataRetrievalModeFlag flag)
        {
            switch (flag)
            {
            case Unspecified: return "Unspecified";
            case DbDirect: return "Direct DB access";
            case Shared: return "Shared data";
            case Cached: return "Cached data";
            default:
                BLACK_VERIFY_X(false, Q_FUNC_INFO, "wrong flags");
                return "wrong flags";
            }
        }

        QString CDbFlags::flagToString(BlackMisc::Db::CDbFlags::DataRetrievalMode flag)
        {
            QStringList list;
            if (flag.testFlag(Unspecified)) list << "Unspecified";
            if (flag.testFlag(DbDirect)) list << "Direct DB access";
            if (flag.testFlag(Shared)) list << "Shared data";
            if (flag.testFlag(Cached)) list << "Cached data";
            return list.join(',');
        }

        void CDbFlags::registerMetadata()
        {
            // this is no value class and I register enums here,
            // that's why I do not use the Mixins
            qRegisterMetaType<CDbFlags::DataRetrievalModeFlag>();
            qRegisterMetaType<CDbFlags::DataRetrievalMode>();
        }
    } // namespace
} // namespace
