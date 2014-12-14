/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_AIRCRAFTLISTMODEL_H
#define BLACKGUI_AIRCRAFTLISTMODEL_H

#include "blackmisc/avaircraftlist.h"
#include "blackgui/models/listmodelbase.h"
#include <QAbstractItemModel>
#include <QDBusConnection>

namespace BlackGui
{
    namespace Models
    {

        //! Aircraft list model
        class CAircraftListModel : public CListModelBase<BlackMisc::Aviation::CAircraft, BlackMisc::Aviation::CAircraftList>
        {

        public:

            //! Constructor
            explicit CAircraftListModel(QObject *parent = nullptr);

            //! Destructor
            virtual ~CAircraftListModel() {}

        };
    }
}
#endif // guard
