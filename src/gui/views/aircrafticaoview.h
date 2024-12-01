// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_VIEWS_AIRCRAFTICAOVIEW_H
#define SWIFT_GUI_VIEWS_AIRCRAFTICAOVIEW_H

#include "gui/models/aircrafticaolistmodel.h"
#include "gui/swiftguiexport.h"
#include "gui/views/viewdbobjects.h"

namespace swift::gui::views
{
    //! Aircraft ICAO codes view
    class SWIFT_GUI_EXPORT CAircraftIcaoCodeView : public CViewWithDbObjects<models::CAircraftIcaoCodeListModel>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CAircraftIcaoCodeView(QWidget *parent = nullptr);
    };
} // namespace swift::gui::views
#endif // SWIFT_GUI_VIEWS_AIRCRAFTICAOVIEW_H
