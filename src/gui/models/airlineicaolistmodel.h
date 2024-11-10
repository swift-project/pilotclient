// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_AIRLINEICAOLISTMODEL_H
#define SWIFT_GUI_AIRLINEICAOLISTMODEL_H

#include "gui/swiftguiexport.h"
#include "gui/models/listmodeldbobjects.h"

class QObject;

namespace swift::misc::aviation
{
    class CAirlineIcaoCode;
    class CAirlineIcaoCodeList;
}

namespace swift::gui::models
{
    //! Airport list model
    class SWIFT_GUI_EXPORT CAirlineIcaoCodeListModel :
        public CListModelDbObjects<swift::misc::aviation::CAirlineIcaoCodeList, int, true>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CAirlineIcaoCodeListModel(QObject *parent = nullptr);

        //! Destructor
        virtual ~CAirlineIcaoCodeListModel() {}
    };
}
#endif // guard
