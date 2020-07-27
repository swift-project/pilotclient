/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_ICONLIST_H
#define BLACKMISC_ICONLIST_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/collection.h"
#include "blackmisc/compare.h"
#include "blackmisc/icon.h"
#include "blackmisc/icons.h"
#include "blackmisc/sequence.h"

#include <QJsonObject>
#include <QMetaType>
#include <QString>
#include <QtGlobal>
#include <tuple>

namespace BlackMisc
{
    /*!
     * Icon list (swift standard icons)
     */
    class BLACKMISC_EXPORT CIconList : public CSequence<CIcon>
    {
        using CSequence::CSequence;

    public:
        //! Constructor
        CIconList() {}

        //! Construct from a base class object.
        CIconList(const CSequence<CIcon> &other);

        //! Find by index
        CIcon findByIndex(CIcons::IconIndex index) const;

        //! Register metadata of unit and quantity
        static void registerMetadata();

        //! All icons
        static const CIconList &allIcons();
    };
}

Q_DECLARE_METATYPE(BlackMisc::CIconList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::CIcon>)

#endif // guard
