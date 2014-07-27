/* Copyright (C) 2013
 * Swift Project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of Swift Project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_NAMEVARIANTLISTMODEL_H
#define BLACKGUI_NAMEVARIANTLISTMODEL_H

#include "blackmisc/namevariantpairlist.h"
#include "blackgui/models/listmodelbase.h"
#include <QAbstractItemModel>
#include <QMap>

namespace BlackGui
{
    namespace Models
    {

        /*!
         * Simple model displaying name / variant values
         */
        class CNameVariantPairModel : public CListModelBase<BlackMisc::CNameVariantPair, BlackMisc::CNameVariantPairList>
        {

        public:

            //! Constructor
            explicit CNameVariantPairModel(QObject *parent = nullptr);

            //! Constructor
            explicit CNameVariantPairModel(const BlackMisc::CNameVariantPairList &nameValues, QObject *parent = nullptr);

            //! Destructor
            virtual ~CNameVariantPairModel() {}

        };
    }
}
#endif // guard
