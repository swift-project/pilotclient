/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_AIRLINEICAOVIEW_H
#define BLACKGUI_AIRLINEICAOVIEW_H

#include "blackgui/blackguiexport.h"
#include "viewbase.h"
#include "../models/airlineicaolistmodel.h"

namespace BlackGui
{
    namespace Views
    {
        //! Aircraft ICAO codes view
        class BLACKGUI_EXPORT CAirlineIcaoCodeView : public CViewBase<Models::CAirlineIcaoCodeListModel, BlackMisc::Aviation::CAirlineIcaoCodeList, BlackMisc::Aviation::CAirlineIcaoCode>
        {

        public:
            //! Constructor
            explicit CAirlineIcaoCodeView(QWidget *parent = nullptr);
        };
    }
}
#endif // guard
