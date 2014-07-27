/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*!
    \file
*/

#ifndef BLACKGUI_ATCLISTMODEL_H
#define BLACKGUI_ATCLISTMODEL_H

#include "blackmisc/avatcstationlist.h"
#include "blackgui/models/listmodelbase.h"
#include <QAbstractItemModel>
#include <QDBusConnection>

namespace BlackGui
{
    namespace Models
    {
        //! ATC list model
        class CAtcStationListModel : public CListModelBase<BlackMisc::Aviation::CAtcStation, BlackMisc::Aviation::CAtcStationList>
        {

        public:
            //! What kind of stations
            enum AtcStationMode
            {
                NotSet,
                StationsBooked,
                StationsOnline
            };

            //! Constructor
            explicit CAtcStationListModel(AtcStationMode stationMode, QObject *parent = nullptr);

            //! Destructor
            virtual ~CAtcStationListModel() {}

            //! Set station mode
            void setStationMode(AtcStationMode stationMode);

        public slots:
            //! Used to quickly update single station (better response for the user)
            void changedAtcStationConnectionStatus(const BlackMisc::Aviation::CAtcStation &station, bool added);

        private:
            AtcStationMode m_stationMode;
        };
    }
}
#endif // guard
