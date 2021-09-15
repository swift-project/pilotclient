/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

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
