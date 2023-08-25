// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackgui/models/distributorlistmodel.h"
#include "blackgui/views/distributorview.h"
#include "blackgui/views/viewbase.h"

using namespace BlackGui::Models;

namespace BlackGui::Views
{
    CDistributorView::CDistributorView(QWidget *parent) : COrderableViewWithDbObjects(parent)
    {
        this->standardInit(new CDistributorListModel(this));
        this->setMenu(MenuDefaultDbViews);
    }

    void CDistributorView::setDistributorMode(CDistributorListModel::DistributorMode distributorMode)
    {
        derivedModel()->setDistributorMode(distributorMode);
    }

    CDistributorListModel::DistributorMode CDistributorView::getDistributorMode() const
    {
        return derivedModel()->getDistributorMode();
    }
} // namespace
