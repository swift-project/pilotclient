/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_ICONLIST_H
#define BLACKMISC_ICONLIST_H

#include "blackmiscexport.h"
#include "sequence.h"
#include "collection.h"
#include "icon.h"
#include "icons.h"

namespace BlackMisc
{

    /*!
     * Icon
     */
    class BLACKMISC_EXPORT CIconList : public CSequence<CIcon>
    {
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

        //! Icon for given index
        //! \deprecated Use CIcon::iconByIndex instead.
        static const CIcon &iconByIndex(CIcons::IconIndex index);

        //! Icon for given index
        //! \deprecated Use CIcon::iconByIndex instead.
        static const CIcon &iconByIndex(int index);
    };
}

Q_DECLARE_METATYPE(BlackMisc::CIconList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::CIcon>)
Q_DECLARE_METATYPE(BlackMisc::CSequence<BlackMisc::CIcon>)

#endif // guard
