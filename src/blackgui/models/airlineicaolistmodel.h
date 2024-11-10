// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_AIRLINEICAOLISTMODEL_H
#define BLACKGUI_AIRLINEICAOLISTMODEL_H

#include "blackgui/blackguiexport.h"
#include "blackgui/models/listmodeldbobjects.h"

class QObject;

namespace swift::misc::aviation
{
    class CAirlineIcaoCode;
    class CAirlineIcaoCodeList;
}

namespace BlackGui::Models
{
    //! Airport list model
    class BLACKGUI_EXPORT CAirlineIcaoCodeListModel :
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
