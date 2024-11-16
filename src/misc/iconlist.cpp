// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/iconlist.h"

#include <QDBusMetaType>

#include "misc/containerbase.h"

SWIFT_DEFINE_SEQUENCE_MIXINS(swift::misc, CIcon, CIconList)

namespace swift::misc
{
    CIconList::CIconList(const CSequence<CIcon> &other) : CSequence<CIcon>(other) {}

    CIcon CIconList::findByIndex(CIcons::IconIndex index) const
    {
        return this->findBy(&CIcon::getIndex, index).frontOrDefault();
    }

    void CIconList::registerMetadata()
    {
        qRegisterMetaType<swift::misc::CSequence<CIcon>>();
        qDBusRegisterMetaType<swift::misc::CSequence<CIcon>>();
        qRegisterMetaType<swift::misc::CCollection<CIcon>>();
        qDBusRegisterMetaType<swift::misc::CCollection<CIcon>>();
        qRegisterMetaType<CIconList>();
        qDBusRegisterMetaType<CIconList>();
        registerMetaValueType<CIconList>();
    }
} // namespace swift::misc
