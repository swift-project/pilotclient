/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackgui/views/aircraftsituationview.h"

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackGui::Models;
using namespace BlackGui::Menus;

namespace BlackGui::Views
{
    CAircraftSituationView::CAircraftSituationView(QWidget *parent) :
        CViewWithTimestampWithOffsetObjects(parent)
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
