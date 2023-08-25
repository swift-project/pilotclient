// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackmisc/iconlist.h"
#include "blackmisc/containerbase.h"
#include <QDBusMetaType>

BLACK_DEFINE_SEQUENCE_MIXINS(BlackMisc, CIcon, CIconList)

namespace BlackMisc
{
    CIconList::CIconList(const CSequence<CIcon> &other) : CSequence<CIcon>(other)
    {}

    CIcon CIconList::findByIndex(CIcons::IconIndex index) const
    {
        return this->findBy(&CIcon::getIndex, index).frontOrDefault();
    }

    void CIconList::registerMetadata()
    {
        qRegisterMetaType<BlackMisc::CSequence<CIcon>>();
        qDBusRegisterMetaType<BlackMisc::CSequence<CIcon>>();
        qRegisterMetaTypeStreamOperators<BlackMisc::CSequence<CIcon>>();
        qRegisterMetaType<BlackMisc::CCollection<CIcon>>();
        qDBusRegisterMetaType<BlackMisc::CCollection<CIcon>>();
        qRegisterMetaTypeStreamOperators<BlackMisc::CCollection<CIcon>>();
        qRegisterMetaType<CIconList>();
        qDBusRegisterMetaType<CIconList>();
        qRegisterMetaTypeStreamOperators<CIconList>();
        registerMetaValueType<CIconList>();
    }
} // ns
