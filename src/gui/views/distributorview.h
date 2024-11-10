// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_VIEWS_DISTRIBUTORVIEW_H
#define SWIFT_GUI_VIEWS_DISTRIBUTORVIEW_H

#include "gui/swiftguiexport.h"
#include "gui/models/distributorlistmodel.h"
#include "gui/views/viewdbobjects.h"

class QString;
class QWidget;

namespace swift::misc::simulation
{
    class CDistributor;
    class CDistributorList;
}

namespace swift::gui::views
{
    //! Distributors
    class SWIFT_GUI_EXPORT CDistributorView :
        public COrderableViewWithDbObjects<swift::gui::models::CDistributorListModel>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CDistributorView(QWidget *parent = nullptr);

        //! Set mode
        void setDistributorMode(swift::gui::models::CDistributorListModel::DistributorMode distributorMode);

        //! Mode
        swift::gui::models::CDistributorListModel::DistributorMode getDistributorMode() const;
    };
} // ns
#endif // guard
