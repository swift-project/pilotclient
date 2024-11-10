// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_MODELS_COUNTRYLISTMODEL_H
#define SWIFT_GUI_MODELS_COUNTRYLISTMODEL_H

#include "gui/swiftguiexport.h"
#include "gui/models/listmodeldbobjects.h"
#include <QString>

class QObject;

namespace swift::misc
{
    class CCountry;
    class CCountryList;
}

namespace swift::gui::models
{
    //! Country list model
    class SWIFT_GUI_EXPORT CCountryListModel :
        public CListModelDbObjects<swift::misc::CCountryList, QString, true>
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
