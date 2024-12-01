// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_AIRLINEICAOVIEW_H
#define SWIFT_GUI_AIRLINEICAOVIEW_H

#include "gui/models/airlineicaolistmodel.h"
#include "gui/swiftguiexport.h"
#include "gui/views/viewdbobjects.h"

class QWidget;

namespace swift::misc::aviation
{
    class CAirlineIcaoCode;
    class CAirlineIcaoCodeList;
} // namespace swift::misc::aviation

namespace swift::gui::views
{
    //! Aircraft ICAO codes view
    class SWIFT_GUI_EXPORT CAirlineIcaoCodeView : public CViewWithDbObjects<models::CAirlineIcaoCodeListModel>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CAirlineIcaoCodeView(QWidget *parent = nullptr);
    };
} // namespace swift::gui::views

#endif // SWIFT_GUI_AIRLINEICAOVIEW_H
