// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_VIEWS_DISTRIBUTORVIEW_H
#define BLACKGUI_VIEWS_DISTRIBUTORVIEW_H

#include "blackgui/blackguiexport.h"
#include "blackgui/models/distributorlistmodel.h"
#include "blackgui/views/viewdbobjects.h"

class QString;
class QWidget;

namespace BlackMisc::Simulation
{
    class CDistributor;
    class CDistributorList;
}

namespace BlackGui::Views
{
    //! Distributors
    class BLACKGUI_EXPORT CDistributorView :
        public COrderableViewWithDbObjects<BlackGui::Models::CDistributorListModel>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CDistributorView(QWidget *parent = nullptr);

        //! Set mode
        void setDistributorMode(BlackGui::Models::CDistributorListModel::DistributorMode distributorMode);

        //! Mode
        BlackGui::Models::CDistributorListModel::DistributorMode getDistributorMode() const;
    };
} // ns
#endif // guard
