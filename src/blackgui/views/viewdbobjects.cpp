/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "viewdbobjects.h"
#include "blackgui/models/allmodels.h"
#include <QAction>
#include <QWidgetAction>
#include <QLineEdit>
#include <QIntValidator>

using namespace BlackMisc;
using namespace BlackGui;
using namespace BlackGui::Models;

namespace BlackGui
{
    namespace Views
    {
        template <class ModelClass, class ContainerType, class ObjectType, class KeyType>
        CViewWithDbObjects<ModelClass, ContainerType, ObjectType, KeyType>::CViewWithDbObjects(QWidget *parent) :
            CViewBase<ModelClass, ContainerType, ObjectType>(parent)
        {
            // void
        }

        template <class ModelClass, class ContainerType, class ObjectType, class KeyType>
        ObjectType CViewWithDbObjects<ModelClass, ContainerType, ObjectType, KeyType>::latestObject() const
        {
            return this->container().latestObject();
        }

        template <class ModelClass, class ContainerType, class ObjectType, class KeyType>
        ObjectType CViewWithDbObjects<ModelClass, ContainerType, ObjectType, KeyType>::oldestObject() const
        {
            return this->container().oldestObject();
        }

        template <class ModelClass, class ContainerType, class ObjectType, class KeyType>
        void CViewWithDbObjects<ModelClass, ContainerType, ObjectType, KeyType>::selectDbKeys(const QList<KeyType> &keys)
        {
            if (keys.isEmpty()) { return; }
            this->clearSelection();
            int r = -1;
            for (const ObjectType &obj : CViewBase<ModelClass, ContainerType, ObjectType>::containerOrFilteredContainer())
            {
                r++;
                if (!obj.hasValidDbKey()) { continue; }
                if (keys.contains(obj.getDbKey()))
                {
                    this->selectRow(r);
                }
            }
        }

        template <class ModelClass, class ContainerType, class ObjectType, class KeyType>
        int CViewWithDbObjects<ModelClass, ContainerType, ObjectType, KeyType>::removeDbKeys(const QList<KeyType> &keys)
        {
            if (keys.isEmpty()) { return 0; }
            if (this->isEmpty()) { return 0; }

            ContainerType newObjects(this->container());
            int delta = newObjects.removeObjectsWithKeys(keys);
            if (delta > 0)
            {
                this->updateContainerMaybeAsync(newObjects);
            }
            return delta;
        }

        template <class ModelClass, class ContainerType, class ObjectType, class KeyType>
        int CViewWithDbObjects<ModelClass, ContainerType, ObjectType, KeyType>::replaceOrAddObjectsByKey(const ContainerType &container)
        {
            if (container.isEmpty()) { return 0; }
            ContainerType copy(this->container());
            int c = copy.replaceOrAddObjectsByKey(container);
            if (c == 0) { return 0; }
            this->updateContainerMaybeAsync(copy);
            return c;
        }

        template <class ModelClass, class ContainerType, class ObjectType, class KeyType>
        void CViewWithDbObjects<ModelClass, ContainerType, ObjectType, KeyType>::customMenu(QMenu &menu) const
        {
            if (this->m_menus.testFlag(CViewBaseNonTemplate::MenuHighlightDbData))
            {
                QAction *a = menu.addAction(CIcons::database16(), "Highlight DB data", this, &CViewWithDbObjects<ModelClass, ContainerType, ObjectType, KeyType>::ps_toggleHighlightDbData);
                a->setCheckable(true);
                a->setChecked(this->derivedModel()->highlightDbData());
            }
            CViewBase<ModelClass, ContainerType, ObjectType>::customMenu(menu);
        }

        template <class ModelClass, class ContainerType, class ObjectType, class KeyType>
        void CViewWithDbObjects<ModelClass, ContainerType, ObjectType, KeyType>::ps_toggleHighlightDbData()
        {
            bool h = this->derivedModel()->highlightDbData();
            this->derivedModel()->setHighlightDbData(!h);
        }

        template <class ModelClass, class ContainerType, class ObjectType, class KeyType>
        COrderableViewWithDbObjects<ModelClass, ContainerType, ObjectType, KeyType>::COrderableViewWithDbObjects(QWidget *parent) :
            CViewWithDbObjects<ModelClass, ContainerType, ObjectType, KeyType>::CViewWithDbObjects(parent)
        {
            // void
        }

        template <class ModelClass, class ContainerType, class ObjectType, class KeyType>
        void COrderableViewWithDbObjects<ModelClass, ContainerType, ObjectType, KeyType>::customMenu(QMenu &menu) const
        {
            if (this->m_menus.testFlag(CViewBaseNonTemplate::MenuOrderable) && this->hasSelection())
            {
                const int maxOrder = this->rowCount() - 1;
                QMenu *menuOrder = menu.addMenu(CIcons::arrowMediumEast16(), "Order");

                QLineEdit *leOrder = new QLineEdit(&menu);
                leOrder->setPlaceholderText("New order 0-" + QString::number(maxOrder));
                const QIntValidator *v = new QIntValidator(0, maxOrder, leOrder);
                leOrder->setValidator(v);
                QWidgetAction *orderAction = new QWidgetAction(&menu);
                orderAction->setDefaultWidget(leOrder);
                menuOrder->addAction(orderAction);
                QObject::connect(leOrder, &QLineEdit::returnPressed, this, &COrderableViewWithDbObjects<ModelClass, ContainerType, ObjectType, KeyType>::ps_orderToLineEdit);

                menuOrder->addAction(CIcons::arrowMediumNorth16(), "To top", this, &COrderableViewWithDbObjects<ModelClass, ContainerType, ObjectType, KeyType>::ps_orderToTop);
                menuOrder->addAction(CIcons::arrowMediumSouth16(), "To bottom", this, &COrderableViewWithDbObjects<ModelClass, ContainerType, ObjectType, KeyType>::ps_orderToBottom);
                menuOrder->addAction(CIcons::arrowMediumWest16(), "Freeze current order", this, &COrderableViewWithDbObjects<ModelClass, ContainerType, ObjectType, KeyType>::ps_freezeCurrentOrder);
                menu.addSeparator();
            }
            CViewWithDbObjects<ModelClass, ContainerType, ObjectType, KeyType>::customMenu(menu);
        }

        template <class ModelClass, class ContainerType, class ObjectType, class KeyType>
        void COrderableViewWithDbObjects<ModelClass, ContainerType, ObjectType, KeyType>::moveSelectedItems(int order)
        {
            if (this->isEmpty()) { return; }
            const ContainerType objs(this->selectedObjects());
            if (objs.isEmpty()) { return; }
            this->m_model->moveItems(objs, order);
        }

        template <class ModelClass, class ContainerType, class ObjectType, class KeyType>
        void COrderableViewWithDbObjects<ModelClass, ContainerType, ObjectType, KeyType>::ps_orderToTop()
        {
            this->moveSelectedItems(0);
        }

        template <class ModelClass, class ContainerType, class ObjectType, class KeyType>
        void COrderableViewWithDbObjects<ModelClass, ContainerType, ObjectType, KeyType>::ps_orderToBottom()
        {
            int c = this->model()->rowCount() - 1;
            if (c >= 0)
            {
                this->moveSelectedItems(c);
            }
        }

        template <class ModelClass, class ContainerType, class ObjectType, class KeyType>
        void COrderableViewWithDbObjects<ModelClass, ContainerType, ObjectType, KeyType>::ps_orderToLineEdit()
        {
            if (this->isEmpty()) { return; }
            QLineEdit *le = qobject_cast<QLineEdit *>(QObject::sender());
            if (!le || le->text().isEmpty()) { return; }
            const int order = le->text().toInt();
            this->moveSelectedItems(order);
        }

        template <class ModelClass, class ContainerType, class ObjectType, class KeyType>
        void COrderableViewWithDbObjects<ModelClass, ContainerType, ObjectType, KeyType>::ps_freezeCurrentOrder()
        {
            ContainerType objects = this->container();
            objects.freezeOrder();
            this->updateContainerAsync(objects, false);
        }

        // see here for the reason of thess forward instantiations
        // http://www.parashift.com/c++-faq/separate-template-class-defn-from-decl.html
        template class CViewWithDbObjects<BlackGui::Models::CAircraftIcaoCodeListModel, BlackMisc::Aviation::CAircraftIcaoCodeList, BlackMisc::Aviation::CAircraftIcaoCode, int>;
        template class CViewWithDbObjects<BlackGui::Models::CAirlineIcaoCodeListModel, BlackMisc::Aviation::CAirlineIcaoCodeList, BlackMisc::Aviation::CAirlineIcaoCode, int>;
        template class CViewWithDbObjects<BlackGui::Models::CCountryListModel, BlackMisc::CCountryList, BlackMisc::CCountry, QString>;
        template class CViewWithDbObjects<BlackGui::Models::CLiveryListModel, BlackMisc::Aviation::CLiveryList, BlackMisc::Aviation::CLivery, int>;
        template class CViewWithDbObjects<BlackGui::Models::CDistributorListModel, BlackMisc::Simulation::CDistributorList, BlackMisc::Simulation::CDistributor, QString>;
        template class CViewWithDbObjects<BlackGui::Models::CAircraftModelListModel, BlackMisc::Simulation::CAircraftModelList, BlackMisc::Simulation::CAircraftModel, int>;
        template class COrderableViewWithDbObjects<BlackGui::Models::CDistributorListModel, BlackMisc::Simulation::CDistributorList, BlackMisc::Simulation::CDistributor, QString>;
        template class COrderableViewWithDbObjects<BlackGui::Models::CAircraftModelListModel, BlackMisc::Simulation::CAircraftModelList, BlackMisc::Simulation::CAircraftModel, int>;

    } // namespace
} // namespace
