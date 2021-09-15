/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_AIRLINEICAOVIEW_H
#define BLACKGUI_AIRLINEICAOVIEW_H

#include "blackgui/blackguiexport.h"
#include "blackgui/models/airlineicaolistmodel.h"
#include "blackgui/views/viewdbobjects.h"

class QWidget;

namespace BlackMisc::Aviation
{
    class CAirlineIcaoCode;
    class CAirlineIcaoCodeList;
}

namespace BlackGui::Views
{
    //! Aircraft ICAO codes view
    class BLACKGUI_EXPORT CAirlineIcaoCodeView :
        public CViewWithDbObjects<Models::CAirlineIcaoCodeListModel>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CAirlineIcaoCodeView(QWidget *parent = nullptr);
    };
}

#endif // guard
