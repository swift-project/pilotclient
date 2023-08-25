// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_VIEWS_COUNTRYVIEW_H
#define BLACKGUI_VIEWS_COUNTRYVIEW_H

#include "blackgui/views/viewbase.h"
#include "blackgui/models/countrylistmodel.h"
#include "blackgui/blackguiexport.h"
#include "blackmisc/countrylist.h"

namespace BlackGui::Views
{
    //! Countries
    class BLACKGUI_EXPORT CCountryView : public CViewBase<Models::CCountryListModel>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CCountryView(QWidget *parent = nullptr);
    };
} // ns
#endif // guard
