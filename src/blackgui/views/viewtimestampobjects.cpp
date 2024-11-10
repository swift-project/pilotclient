// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackgui/views/viewtimestampobjects.h"
#include "blackgui/models/aircraftpartslistmodel.h"
#include "blackgui/models/aircraftsituationlistmodel.h"
#include "blackgui/models/aircraftsituationchangelistmodel.h"
#include <QAction>

using namespace swift::misc;
using namespace swift::misc::aviation;
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
