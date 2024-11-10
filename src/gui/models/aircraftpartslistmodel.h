// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_MODELS_AIRCRAFTPARTSLISTMODEL_H
#define SWIFT_GUI_MODELS_AIRCRAFTPARTSLISTMODEL_H

#include "gui/swiftguiexport.h"
#include "gui/models/listmodeltimestampobjects.h"
#include "misc/aviation/aircraftpartslist.h"

#include <QVariant>

class QModelIndex;
class QObject;

namespace swift::gui::models
{
    //! Aircraft parts list model
    class SWIFT_GUI_EXPORT CAircraftPartsListModel : public CListModelTimestampWithOffsetObjects<swift::misc::aviation::CAircraftPartsList, true>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CAircraftPartsListModel(QObject *parent = nullptr);

        //! Destructor
        virtual ~CAircraftPartsListModel() {}
    };
} // namespace

#endif // guard
