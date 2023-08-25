// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_MODELS_COUNTRYLISTMODEL_H
#define BLACKGUI_MODELS_COUNTRYLISTMODEL_H

#include "blackgui/blackguiexport.h"
#include "blackgui/models/listmodeldbobjects.h"
#include <QString>

class QObject;

namespace BlackMisc
{
    class CCountry;
    class CCountryList;
}

namespace BlackGui::Models
{
    //! Country list model
    class BLACKGUI_EXPORT CCountryListModel :
        public CListModelDbObjects<BlackMisc::CCountryList, QString, true>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CCountryListModel(QObject *parent = nullptr);

        //! Destructor
        virtual ~CCountryListModel() {}
    };
} // ns
#endif // guard
