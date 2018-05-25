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
        void CViewWithTimestampObjects<ModelClass, ContainerType, ObjectType>::push_frontKeepLatestAdjustedFirst(const ObjectType &object, int max)
        {
            this->derivedModel()->push_frontKeepLatestAdjustedFirst(object, max);
        }

        template <class ModelClass, class ContainerType, class ObjectType>
        CViewWithTimestampObjects<ModelClass, ContainerType, ObjectType>::CViewWithTimestampObjects(QWidget *parent) :
            CViewBase<ModelClass, ContainerType, ObjectType>(parent)
        {
            // void
        }

        template class CViewWithTimestampObjects<BlackGui::Models::CAircraftPartsListModel, BlackMisc::Aviation::CAircraftPartsList, BlackMisc::Aviation::CAircraftParts>;
        template class CViewWithTimestampObjects<BlackGui::Models::CAircraftSituationListModel, BlackMisc::Aviation::CAircraftSituationList, BlackMisc::Aviation::CAircraftSituation>;

    } // namespace
} // namespace
