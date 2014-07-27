/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of Swift Project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef BLACKGUI_ATCSTATIONVIEW_H
#define BLACKGUI_ATCSTATIONVIEW_H

//! \file

#include "viewbase.h"
#include "../models/atcstationlistmodel.h"

namespace BlackGui
{
    namespace Views
    {
        //! ATC stations view
        class CAtcStationView : public CViewBase<Models::CAtcStationListModel>
        {

        public:

            //! Constructor
            explicit CAtcStationView(QWidget *parent = nullptr);

            //! Set station mode
            void setStationMode(Models::CAtcStationListModel::AtcStationMode stationMode);

        public slots:
            //! \copydoc CAtcStationListModel::changedAtcStationConnectionStatus
            void changedAtcStationConnectionStatus(const BlackMisc::Aviation::CAtcStation &station, bool added);

        };
    }
}
#endif // guard
