// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackgui/views/aircraftsituationview.h"

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackGui::Models;
using namespace BlackGui::Menus;

namespace BlackGui::Views
{
    CAircraftSituationView::CAircraftSituationView(QWidget *parent) : CViewWithTimestampWithOffsetObjects(parent)
    {
        this->standardInit(new CAircraftSituationListModel(this));
        this->setMenu(MenuDefault);
    }

    void CAircraftSituationView::setWithMenuRequestElevation(bool enable)
    {
        m_withMenuRequestElevation = enable;
        this->setSingleSelection();
    }

    void CAircraftSituationView::customMenu(CMenuActions &menuActions)
    {
        if (m_withMenuRequestElevation && this->hasSingleSelectedRow())
        {
            menuActions.addAction(CIcons::geoPosition16(), "Request elevation", CMenuAction::pathClientSimulation(), { this, &CAircraftSituationView::emitRequestElevationForSituation });
        }
    }

    void CAircraftSituationView::emitRequestElevationForSituation()
    {
        if (!this->hasSingleSelectedRow()) { return; }
        const CAircraftSituation situation = this->selectedObject();
        emit this->requestElevation(situation);
    }
} // ns
