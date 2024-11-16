// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_MODELS_AIRCRAFTSITUATIONLISTMODEL_H
#define SWIFT_GUI_MODELS_AIRCRAFTSITUATIONLISTMODEL_H

#include <QVariant>

#include "gui/models/listmodeltimestampobjects.h"
#include "gui/swiftguiexport.h"
#include "misc/aviation/aircraftsituationlist.h"

namespace swift::gui::models
{
    //! Client list model
    class SWIFT_GUI_EXPORT CAircraftSituationListModel :
        public CListModelTimestampWithOffsetObjects<swift::misc::aviation::CAircraftSituationList, true>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CAircraftSituationListModel(QObject *parent = nullptr);

        //! Destructor
        virtual ~CAircraftSituationListModel() {}
    };
} // namespace swift::gui::models
#endif // guard
