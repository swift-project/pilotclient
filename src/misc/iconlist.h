// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_ICONLIST_H
#define SWIFT_MISC_ICONLIST_H

#include <QJsonObject>
#include <QMetaType>
#include <QString>
#include <QtGlobal>

#include "misc/collection.h"
#include "misc/icon.h"
#include "misc/icons.h"
#include "misc/sequence.h"
#include "misc/swiftmiscexport.h"

SWIFT_DECLARE_SEQUENCE_MIXINS(swift::misc, CIcon, CIconList)

namespace swift::misc
{
    /*!
     * Icon list (swift standard icons)
     */
    class SWIFT_MISC_EXPORT CIconList : public CSequence<CIcon>, public mixin::MetaType<CIconList>
    {
        using CSequence::CSequence;
        SWIFT_MISC_DECLARE_USING_MIXIN_METATYPE(CIconList)

    public:
        //! Constructor
        CIconList() = default;

        //! Construct from a base class object.
        CIconList(const CSequence<CIcon> &other);

        //! Find by index
        CIcon findByIndex(CIcons::IconIndex index) const;

        //! Register metadata of unit and quantity
        static void registerMetadata();
    };
} // namespace swift::misc

Q_DECLARE_METATYPE(swift::misc::CIconList)
Q_DECLARE_METATYPE(swift::misc::CCollection<swift::misc::CIcon>)

#endif // SWIFT_MISC_ICONLIST_H
