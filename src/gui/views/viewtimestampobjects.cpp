// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/views/viewtimestampobjects.h"
#include "gui/models/aircraftpartslistmodel.h"
#include "gui/models/aircraftsituationlistmodel.h"
#include "gui/models/aircraftsituationchangelistmodel.h"
#include <QAction>

using namespace swift::misc;
using namespace swift::misc::aviation;
using namespace swift::gui;
using namespace swift::gui::models;

namespace swift::gui::views
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

    template class CViewWithTimestampWithOffsetObjects<swift::gui::models::CAircraftPartsListModel>;
    template class CViewWithTimestampWithOffsetObjects<swift::gui::models::CAircraftSituationListModel>;
    template class CViewWithTimestampWithOffsetObjects<swift::gui::models::CAircraftSituationChangeListModel>;

} // namespace
