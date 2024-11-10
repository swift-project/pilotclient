// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_MODELS_ATCLISTMODEL_H
#define SWIFT_GUI_MODELS_ATCLISTMODEL_H

#include "gui/models/listmodelcallsignobjects.h"
#include "misc/aviation/atcstation.h"
#include "misc/aviation/atcstationlist.h"
#include "gui/swiftguiexport.h"

#include <QObject>

namespace swift::gui::models
{
    class CAtcStationTreeModel;

    //! ATC list model
    class SWIFT_GUI_EXPORT CAtcStationListModel : public CListModelCallsignObjects<swift::misc::aviation::CAtcStationList, true>
    {
        Q_OBJECT

    public:
        //! What kind of stations
        enum AtcStationMode
        {
            NotSet,
            StationsOnline
        };

        //! Constructor
        explicit CAtcStationListModel(AtcStationMode stationMode, QObject *parent = nullptr);

        //! Destructor
        virtual ~CAtcStationListModel() override {}

        //! Set station mode
        void setStationMode(AtcStationMode stationMode);

        //! Station mode
        AtcStationMode getStationMode() const { return this->m_stationMode; }

        //! A group by type (TWR, APP, ...) model
        CAtcStationTreeModel *toAtcTreeModel() const;

    public slots:
        //! Used to quickly update single station (better response for the user)
        void changedAtcStationConnectionStatus(const swift::misc::aviation::CAtcStation &station, bool added);

    private:
        AtcStationMode m_stationMode = NotSet;
    };
} // ns

#endif // guard
