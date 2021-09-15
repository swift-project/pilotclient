/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackgui/models/distributorlistmodel.h"
#include "blackgui/views/distributorview.h"
#include "blackgui/views/viewbase.h"

using namespace BlackGui::Models;

namespace BlackGui::Views
{
    CDistributorView::CDistributorView(QWidget *parent) :
        COrderableViewWithDbObjects(parent)
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
