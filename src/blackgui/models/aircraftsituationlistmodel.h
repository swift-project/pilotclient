// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_MODELS_AIRCRAFTSITUATIONLISTMODEL_H
#define BLACKGUI_MODELS_AIRCRAFTSITUATIONLISTMODEL_H

#include "blackgui/models/listmodeltimestampobjects.h"
#include "blackgui/blackguiexport.h"
#include "blackmisc/aviation/aircraftsituationlist.h"

#include <QVariant>

namespace BlackGui::Models
{
    //! Client list model
    class BLACKGUI_EXPORT CAircraftSituationListModel : public CListModelTimestampWithOffsetObjects<BlackMisc::Aviation::CAircraftSituationList, true>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CAircraftSituationListModel(QObject *parent = nullptr);

        //! Destructor
        virtual ~CAircraftSituationListModel() {}
    };
} // namespace
#endif // guard
