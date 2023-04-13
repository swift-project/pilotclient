/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackgui/views/viewtimestampobjects.h"
#include "blackgui/models/aircraftpartslistmodel.h"
#include "blackgui/models/aircraftsituationlistmodel.h"
#include "blackgui/models/aircraftsituationchangelistmodel.h"
#include <QAction>

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackGui;
using namespace BlackGui::Models;

namespace BlackGui::Views
{
    template <class T>
    void CViewWithTimestampWithOffsetObjects<T>::push_frontKeepLatestAdjustedFirst(const ObjectType &object, int max)
    {
        this->derivedModel()->push_frontKeepLatestAdjustedFirst(object, max);
    }

    template <class T>
    CViewWithTimestampWithOffsetObjects<T>::CViewWithTimestampWithOffsetObjects(QWidget *parent) : CViewBase<ModelClass>(parent)
    {
        // void
    }

    template class CViewWithTimestampWithOffsetObjects<BlackGui::Models::CAircraftPartsListModel>;
    template class CViewWithTimestampWithOffsetObjects<BlackGui::Models::CAircraftSituationListModel>;
    template class CViewWithTimestampWithOffsetObjects<BlackGui::Models::CAircraftSituationChangeListModel>;

} // namespace
