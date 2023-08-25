// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_MODELS_AIRCRAFTICAOLISTMODEL_H
#define BLACKGUI_MODELS_AIRCRAFTICAOLISTMODEL_H

#include "blackmisc/aviation/aircrafticaocodelist.h"
#include "blackmisc/aviation/aircrafticaocode.h"
#include "blackgui/models/listmodeldbobjects.h"
#include "blackgui/blackguiexport.h"

namespace BlackGui::Models
{
    //! Airport list model
    class BLACKGUI_EXPORT CAircraftIcaoCodeListModel :
        public CListModelDbObjects<BlackMisc::Aviation::CAircraftIcaoCodeList, int, true>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CAircraftIcaoCodeListModel(QObject *parent = nullptr);

        //! Destructor
        virtual ~CAircraftIcaoCodeListModel() {}
    };
} // ns

#endif // guard
