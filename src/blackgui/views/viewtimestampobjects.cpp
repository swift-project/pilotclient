/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
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

namespace BlackGui
{
    namespace Views
    {
        template<class ModelClass, class ContainerType, class ObjectType>
        void CViewWithTimestampWithOffsetObjects<ModelClass, ContainerType, ObjectType>::push_frontKeepLatestAdjustedFirst(const ObjectType &object, int max)
        {
            this->derivedModel()->push_frontKeepLatestAdjustedFirst(object, max);
        }

        template <class ModelClass, class ContainerType, class ObjectType>
        CViewWithTimestampWithOffsetObjects<ModelClass, ContainerType, ObjectType>::CViewWithTimestampWithOffsetObjects(QWidget *parent) :
            CViewBase<ModelClass, ContainerType, ObjectType>(parent)
        {
            // void
        }

        template class CViewWithTimestampWithOffsetObjects<BlackGui::Models::CAircraftPartsListModel, BlackMisc::Aviation::CAircraftPartsList, BlackMisc::Aviation::CAircraftParts>;
        template class CViewWithTimestampWithOffsetObjects<BlackGui::Models::CAircraftSituationListModel, BlackMisc::Aviation::CAircraftSituationList, BlackMisc::Aviation::CAircraftSituation>;
        template class CViewWithTimestampWithOffsetObjects<BlackGui::Models::CAircraftSituationChangeListModel, BlackMisc::Aviation::CAircraftSituationChangeList, BlackMisc::Aviation::CAircraftSituationChange>;

    } // namespace
} // namespace
