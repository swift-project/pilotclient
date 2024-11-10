// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_VIEWS_COUNTRYVIEW_H
#define SWIFT_GUI_VIEWS_COUNTRYVIEW_H

#include "gui/views/viewbase.h"
#include "gui/models/countrylistmodel.h"
#include "gui/swiftguiexport.h"
#include "misc/countrylist.h"

namespace swift::gui::views
{
    //! Countries
    class SWIFT_GUI_EXPORT CCountryView : public CViewBase<models::CCountryListModel>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CCountryView(QWidget *parent = nullptr);
    };
} // ns
#endif // guard
