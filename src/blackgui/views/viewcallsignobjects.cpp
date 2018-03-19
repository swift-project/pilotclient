/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackgui/views/viewcallsignobjects.h"
#include "blackgui/models/atcstationlistmodel.h"
#include "blackgui/models/interpolationsetupmodel.h"
#include "blackgui/models/simulatedaircraftlistmodel.h"
#include "blackmisc/icons.h"

#include <QAction>
#include <QIntValidator>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QLabel>
#include <QWidgetAction>

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackGui;
using namespace BlackGui::Models;

namespace BlackGui
{
    namespace Views
    {
        template <class ModelClass, class ContainerType, class ObjectType>
        CViewWithCallsignObjects<ModelClass, ContainerType, ObjectType>::CViewWithCallsignObjects(QWidget *parent) :
            CViewBase<ModelClass, ContainerType, ObjectType>(parent)
        {
            // void
        }

        template <class ModelClass, class ContainerType, class ObjectType>
        void CViewWithCallsignObjects<ModelClass, ContainerType, ObjectType>::selectCallsign(const CCallsign &callsign)
        {
            const CCallsignSet cs({callsign});
            this->selectCallsigns(cs);
        }

        template <class ModelClass, class ContainerType, class ObjectType>
        void CViewWithCallsignObjects<ModelClass, ContainerType, ObjectType>::selectCallsigns(const CCallsignSet &callsigns)
        {
            if (callsigns.isEmpty()) { return; }
            this->clearSelection();
            int r = -1;
            QSet<int> rows;
            for (const ObjectType &obj : CViewBase<ModelClass, ContainerType, ObjectType>::containerOrFilteredContainer())
            {
                r++;
                if (callsigns.contains(obj.getCallsign()))
                {
                    rows.insert(r);
                }
            }
            this->selectRows(rows);
        }

        template <class ModelClass, class ContainerType, class ObjectType>
        CCallsignSet CViewWithCallsignObjects<ModelClass, ContainerType, ObjectType>::selectedCallsigns() const
        {
            if (!this->hasSelection()) { return CCallsignSet(); }
            const ContainerType selected(this->selectedObjects());
            return selected.getCallsigns();
        }

        template<class ModelClass, class ContainerType, class ObjectType>
        int CViewWithCallsignObjects<ModelClass, ContainerType, ObjectType>::removeCallsign(const CCallsign &callsign)
        {
            if (callsign.isEmpty()) { return 0; }
            const CCallsignSet set({ callsign });
            return this->removeCallsigns(set);
        }

        template <class ModelClass, class ContainerType, class ObjectType>
        int CViewWithCallsignObjects<ModelClass, ContainerType, ObjectType>::removeCallsigns(const CCallsignSet &callsigns)
        {
            if (callsigns.isEmpty()) { return 0; }
            if (this->isEmpty()) { return 0; }

            ContainerType newObjects(this->container());
            int delta = newObjects.removeByCallsigns(callsigns);
            if (delta > 0)
            {
                this->updateContainerMaybeAsync(newObjects);
            }
            return delta;
        }

        template <class ModelClass, class ContainerType, class ObjectType>
        int CViewWithCallsignObjects<ModelClass, ContainerType, ObjectType>::replaceOrAddObjectsByCallsign(const ContainerType &container)
        {
            if (container.isEmpty()) { return 0; }
            ContainerType copy(this->container());
            const int c = copy.replaceOrAddObjectsByCallsign(container);
            if (c == 0) { return 0; }
            this->updateContainerMaybeAsync(copy);
            return c;
        }

        template<class ModelClass, class ContainerType, class ObjectType>
        int CViewWithCallsignObjects<ModelClass, ContainerType, ObjectType>::replaceOrAddObjectByCallsign(const ObjectType &object)
        {
            ContainerType copy(this->container());
            const int c = copy.replaceOrAddObjectByCallsign(object);
            if (c == 0) { return 0; }
            this->updateContainerMaybeAsync(copy);
            return c;
        }

        template<class ModelClass, class ContainerType, class ObjectType>
        void CViewWithCallsignObjects<ModelClass, ContainerType, ObjectType>::selectObjects(const ContainerType &selectedObjects)
        {
            if (!selectedObjects.isEmpty())
            {
                this->selectCallsigns(selectedObjects.getCallsigns());
            }
        }

        template class CViewWithCallsignObjects<BlackGui::Models::CAtcStationListModel, BlackMisc::Aviation::CAtcStationList, BlackMisc::Aviation::CAtcStation>;
        template class CViewWithCallsignObjects<BlackGui::Models::CInterpolationSetupListModel, BlackMisc::Simulation::CInterpolationSetupList, BlackMisc::Simulation::CInterpolationAndRenderingSetupPerCallsign>;
        template class CViewWithCallsignObjects<BlackGui::Models::CSimulatedAircraftListModel, BlackMisc::Simulation::CSimulatedAircraftList, BlackMisc::Simulation::CSimulatedAircraft>;

    } // namespace
} // namespace
