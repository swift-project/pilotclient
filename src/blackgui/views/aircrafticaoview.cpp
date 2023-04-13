/* Copyright (C) 2015
 * swift Project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackgui/views/aircrafticaoview.h"
#include "blackgui/views/viewbase.h"

using namespace BlackMisc;
using namespace BlackGui::Models;

namespace BlackGui::Views
{
    CAircraftIcaoCodeView::CAircraftIcaoCodeView(QWidget *parent) : CViewWithDbObjects(parent)
    {
        this->standardInit(new CAircraftIcaoCodeListModel(this));
        this->setMenu(MenuDefaultDbViews);
    }
} // namespace
