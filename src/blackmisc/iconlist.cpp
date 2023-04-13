/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

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
