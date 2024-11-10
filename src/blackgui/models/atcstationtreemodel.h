// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_MODELS_ATCTREEMODEL_H
#define BLACKGUI_MODELS_ATCTREEMODEL_H

#include "blackgui/models/columns.h"
#include "misc/aviation/atcstationlist.h"
#include "blackgui/blackguiexport.h"

#include <QStandardItemModel>
#include <QHash>

namespace BlackGui::Models
{
    //! ATC list model
    class BLACKGUI_EXPORT CAtcStationTreeModel : public QStandardItemModel
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CAtcStationTreeModel(QObject *parent = nullptr);

        //! Destructor
        virtual ~CAtcStationTreeModel() override {}

        //! Set columns
        void setColumns(const CColumns &columns) { m_columns.setColumns(columns); }

        //! Update container
        void updateContainer(const swift::misc::aviation::CAtcStationList &stations);

        //! Clear everything
        //! \remark hiding QStandardItemModel::clear()
        void clear();

        //! Get container
        const swift::misc::aviation::CAtcStationList &container() const { return m_stations; }

        //! Used to quickly update single station (better response for the user)
        void changedAtcStationConnectionStatus(const swift::misc::aviation::CAtcStation &station, bool added);

    private:
        CColumns m_columns { "CAtcStationTreeModel" };
        swift::misc::aviation::CAtcStationList m_stations;
        QHash<QString, swift::misc::aviation::CAtcStationList> m_stationsBySuffix;
        QStringList m_suffixes;
    };
} // ns
#endif // guard
