// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_MODELS_AIRCRAFTSITUATIONCHANGELISTMODEL_H
#define SWIFT_GUI_MODELS_AIRCRAFTSITUATIONCHANGELISTMODEL_H

#include <QVariant>

#include "gui/models/listmodeltimestampobjects.h"
#include "gui/swiftguiexport.h"
#include "misc/aviation/aircraftsituationchangelist.h"

class QModelIndex;
class QObject;

namespace swift::gui::models
{
    //! Aircraft situation changes list model
    class SWIFT_GUI_EXPORT CAircraftSituationChangeListModel :
        public CListModelTimestampWithOffsetObjects<swift::misc::aviation::CAircraftSituationChangeList, true>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CAircraftSituationChangeListModel(QObject *parent = nullptr);

        //! Destructor
        virtual ~CAircraftSituationChangeListModel() {}
    };
} // namespace swift::gui::models

#endif // SWIFT_GUI_MODELS_AIRCRAFTSITUATIONCHANGELISTMODEL_H
