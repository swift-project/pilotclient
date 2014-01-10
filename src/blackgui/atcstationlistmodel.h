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
#include "blackgui/listmodelbase.h"
#include <QAbstractItemModel>
#include <QDBusConnection>

namespace BlackGui
{
    /*!
     * \brief ATC list model
     */
    class CAtcListModel : public CListModelBase<BlackMisc::Aviation::CAtcStation, BlackMisc::Aviation::CAtcStationList>
    {

    public:

        /*!
         * \brief Constructor
         * \param parent
         */
        explicit CAtcListModel(QObject *parent = nullptr);

        /*!
         * \brief Destructor
         */
        virtual ~CAtcListModel() {}

    };
}
#endif // guard
