/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackgui/views/aircraftsituationview.h"

using namespace BlackGui::Models;

namespace BlackGui
{
    namespace Views
    {
        CAircraftSituationView::CAircraftSituationView(QWidget *parent) :
            CViewWithTimestampWithOffsetObjects(parent)
        {
            this->standardInit(new CAircraftSituationListModel(this));
            this->setMenu(MenuDefault);
        }
    } // ns
} // ns
