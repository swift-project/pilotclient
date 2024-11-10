// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackmisc/db/dbflags.h"
#include "blackmisc/verify.h"

#include <QStringList>
#include <QtGlobal>

namespace BlackMisc::Db
{
    bool CDbFlags::readsFromWeb(CDbFlags::DataRetrievalMode mode)
    {
        return mode.testFlag(DbReading) || mode.testFlag(Shared);
    }

    QString CDbFlags::flagToString(CDbFlags::DataRetrievalModeFlag flag)
    {
        switch (flag)
        {
        case Unspecified: return "Unspecified";
        case Ignore: return "Ignore";
        case Canceled: return "Canceled";
        case DbReading: return "Direct DB access";
        case DbWriting: return "DB writing";
        case Shared: return "Shared data";
        case SharedInfoOnly: return "Shared info only";
        case Cached: return "Cached data";
        default:
            SWIFT_VERIFY_X(false, Q_FUNC_INFO, "wrong flags");
            return "wrong flags";
        }
    }

    QString CDbFlags::flagToString(CDbFlags::DataRetrievalMode mode)
    {
        QStringList list;
        if (mode.testFlag(Unspecified)) list << "Unspecified";
        if (mode.testFlag(Canceled)) list << "Canceled";
        if (mode.testFlag(Ignore)) list << "Ignore";

        if (mode.testFlag(DbReading)) list << "Direct DB access";
        if (mode.testFlag(DbWriting)) list << "DB writing";
        if (mode.testFlag(Shared)) list << "Shared data";
        if (mode.testFlag(SharedInfoOnly)) list << "Shared info only";
        if (mode.testFlag(Cached)) list << "Cached data";
        return list.join(", ");
    }

    CDbFlags::DataRetrievalModeFlag CDbFlags::modeToModeFlag(DataRetrievalMode mode)
    {
        if (mode == Unspecified) return Unspecified;
        if (mode == DbReading) return DbReading;
        if (mode == DbWriting) return DbWriting;
        if (mode == Shared) return Shared;
        if (mode == SharedInfoOnly) return SharedInfoOnly;
        if (mode == Cached) return Cached;
        if (mode == Canceled) return Canceled;
        if (mode == Ignore) return Ignore;
        return Unspecified;
    }

    CDbFlags::DataRetrievalMode CDbFlags::adjustWhenDbIsDown(DataRetrievalMode mode)
    {
        switch (mode)
        {
        case DbReading: return Canceled;
        case CacheThenDb: return Cached;
        default: return mode;
        }
    }

    void CDbFlags::registerMetadata()
    {
        // this is no value class and I register enums here,
        // that's why I do not use the Mixins
        qRegisterMetaType<CDbFlags::DataRetrievalModeFlag>();
        qRegisterMetaType<CDbFlags::DataRetrievalMode>();
    }
} // namespace
