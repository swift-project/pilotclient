/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKGUI_AIRCRAFTLISTMODEL_H
#define BLACKGUI_AIRCRAFTLISTMODEL_H

#include "blackmisc/avaircraftlist.h"
#include "blackgui/listmodelbase.h"
#include <QAbstractItemModel>
#include <QDBusConnection>

namespace BlackGui
{
    /*!
     * Aircraft list model
     */
    class CAircraftListModel : public CListModelBase<BlackMisc::Aviation::CAircraft, BlackMisc::Aviation::CAircraftList>
    {

    public:

        //! Constructor
        explicit CAircraftListModel(QObject *parent = nullptr);

        //! Destructor
        virtual ~CAircraftListModel() {}

    };
}
#endif // guard
