// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_ICONLIST_H
#define BLACKMISC_ICONLIST_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/collection.h"
#include "blackmisc/mixin/mixincompare.h"
#include "blackmisc/icon.h"
#include "blackmisc/icons.h"
#include "blackmisc/sequence.h"

#include <QJsonObject>
#include <QMetaType>
#include <QString>
#include <QtGlobal>
#include <tuple>

BLACK_DECLARE_SEQUENCE_MIXINS(BlackMisc, CIcon, CIconList)

namespace BlackMisc
{
    /*!
     * Icon list (swift standard icons)
     */
    class BLACKMISC_EXPORT CIconList :
        public CSequence<CIcon>,
        public Mixin::MetaType<CIconList>
    {
        using CSequence::CSequence;
        BLACKMISC_DECLARE_USING_MIXIN_METATYPE(CIconList)

    public:
        //! Constructor
        CIconList() {}

        //! Construct from a base class object.
        CIconList(const CSequence<CIcon> &other);

        //! Find by index
        CIcon findByIndex(CIcons::IconIndex index) const;

        //! Register metadata of unit and quantity
        static void registerMetadata();
    };
}

Q_DECLARE_METATYPE(BlackMisc::CIconList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::CIcon>)

#endif // guard
