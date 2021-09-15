/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

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
