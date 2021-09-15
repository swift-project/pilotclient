/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_MODELS_AIRPORTLISTMODEL_H
#define BLACKGUI_MODELS_AIRPORTLISTMODEL_H

#include "blackgui/models/listmodelbase.h"
#include "blackgui/blackguiexport.h"
#include "blackmisc/aviation/airportlist.h"
#include "blackmisc/aviation/airport.h"

class QObject;

namespace BlackGui::Models
{
    //! Airport list model
    class BLACKGUI_EXPORT CAirportListModel :
        public CListModelBase<BlackMisc::Aviation::CAirportList, true>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CAirportListModel(QObject *parent = nullptr);

        //! Destructor
        virtual ~CAirportListModel() override {}
    };
}
#endif // guard
