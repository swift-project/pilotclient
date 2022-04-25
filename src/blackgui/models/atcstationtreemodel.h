/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_MODELS_ATCTREEMODEL_H
#define BLACKGUI_MODELS_ATCTREEMODEL_H

#include "blackgui/models/columns.h"
#include "blackmisc/aviation/atcstationlist.h"
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
        void updateContainer(const BlackMisc::Aviation::CAtcStationList &stations);

        //! Clear everything
        //! \remark hiding QStandardItemModel::clear()
        void clear();

        //! Get container
        const BlackMisc::Aviation::CAtcStationList &container() const { return m_stations; }

        //! Used to quickly update single station (better response for the user)
        void changedAtcStationConnectionStatus(const BlackMisc::Aviation::CAtcStation &station, bool added);

    private:
        CColumns m_columns { "CAtcStationTreeModel" };
        BlackMisc::Aviation::CAtcStationList m_stations;
        QHash<QString, BlackMisc::Aviation::CAtcStationList> m_stationsBySuffix;
        QStringList m_suffixes;
    };
} // ns
#endif // guard
