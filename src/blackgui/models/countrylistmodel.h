/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COUNTRYLISTMODEL_H
#define BLACKGUI_COUNTRYLISTMODEL_H

#include "blackgui/blackguiexport.h"
#include "blackmisc/countrylist.h"
#include "blackgui/models/listmodelbase.h"
#include <QAbstractItemModel>

namespace BlackGui
{
    namespace Models
    {
        //! Country list model
        class BLACKGUI_EXPORT CCountryListModel :
            public CListModelBase<BlackMisc::CCountry, BlackMisc::CCountryList, true>
        {
        public:
            //! Constructor
            explicit CCountryListModel(QObject *parent = nullptr);

            //! Destructor
            virtual ~CCountryListModel() {}
        };
    } // ns
} // ns
#endif // guard
