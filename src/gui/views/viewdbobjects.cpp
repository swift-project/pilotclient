// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/views/viewdbobjects.h"

#include <QAction>
#include <QHBoxLayout>
#include <QIntValidator>
#include <QLabel>
#include <QLineEdit>
#include <QWidgetAction>

#include "gui/menus/menuaction.h"
#include "gui/models/aircraftcategorylistmodel.h"
#include "gui/models/aircrafticaolistmodel.h"
#include "gui/models/aircraftmodellistmodel.h"
#include "gui/models/airlineicaolistmodel.h"
#include "gui/models/countrylistmodel.h"
#include "gui/models/distributorlistmodel.h"
#include "gui/models/liverylistmodel.h"
#include "misc/aviation/aircrafticaocodelist.h"
#include "misc/aviation/airlineicaocodelist.h"
#include "misc/aviation/liverylist.h"
#include "misc/countrylist.h"
#include "misc/icons.h"
#include "misc/simulation/aircraftmodellist.h"
#include "misc/simulation/distributor.h"
#include "misc/simulation/distributorlist.h"

using namespace swift::misc;
using namespace swift::gui;
using namespace swift::gui::models;
using namespace swift::gui::menus;

namespace swift::gui::views
{
    template <class T>
    CViewWithDbObjects<T>::CViewWithDbObjects(QWidget *parent) : CViewBase<ModelClass>(parent)
    {
        // void
        CViewBaseNonTemplate::m_enabledLoadIndicator = true; // indicator for DB views
    }

    template <class T>
    typename CViewWithDbObjects<T>::ObjectType CViewWithDbObjects<T>::latestObject() const
    {
        return this->container().latestObject();
    }

    template <class T>
    typename CViewWithDbObjects<T>::ObjectType CViewWithDbObjects<T>::oldestObject() const
    {
        return this->container().oldestObject();
    }

    template <class T>
    bool CViewWithDbObjects<T>::selectDbKey(const KeyType &key)
    {
        const QSet<KeyType> set({ key });
        return this->selectDbKeys(set) > 0;
    }

    template <class T>
    int CViewWithDbObjects<T>::selectDbKeys(const QSet<KeyType> &keys)
    {
        if (keys.isEmpty()) { return 0; }
        this->clearSelection();
        int r = -1;
        QSet<int> rows;
        for (const ObjectType &obj : CViewBase<ModelClass>::containerOrFilteredContainer())
        {
            r++;
            if (!obj.hasValidDbKey()) { continue; }
            if (keys.contains(obj.getDbKey()))
            {
                rows.insert(r);
            }
        }
        return this->selectRows(rows);
    }

    template <class T>
    QSet<typename CViewWithDbObjects<T>::KeyType> CViewWithDbObjects<T>::selectedDbKeys() const
    {
        if (!this->hasSelection()) { return QSet<KeyType>(); }
        const ContainerType selected(this->selectedObjects());
        return selected.toDbKeySet();
    }

    template <class T>
    int CViewWithDbObjects<T>::removeDbKeys(const QSet<KeyType> &keys)
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

    template <class T>
    int CViewWithDbObjects<T>::replaceOrAddObjectsByKey(const ContainerType &container)
    {
        if (container.isEmpty()) { return 0; }
        ContainerType copy(this->container());
        int c = copy.replaceOrAddObjectsByKey(container);
        if (c == 0) { return 0; }
        this->updateContainerMaybeAsync(copy);
        return c;
    }

    template <class T>
    void CViewWithDbObjects<T>::selectObjects(const ContainerType &selectedObjects)
    {
        if (selectedObjects.isEmpty()) { return; }
        this->selectDbKeys(selectedObjects.toDbKeySet());
    }

    template <class T>
    void CViewWithDbObjects<T>::customMenu(menus::CMenuActions &menuActions)
    {
        // extensions would go here
        CViewBase<ModelClass>::customMenu(menuActions);
    }

    template <class T>
    COrderableViewWithDbObjects<T>::COrderableViewWithDbObjects(QWidget *parent) : CViewWithDbObjects<T>::CViewWithDbObjects(parent)
    {
        // void
    }

    template <class T>
    void COrderableViewWithDbObjects<T>::customMenu(CMenuActions &menuActions)
    {
        if (this->m_menus.testFlag(CViewBaseNonTemplate::MenuOrderable) && this->hasSelection())
        {
            const int maxOrder = this->rowCount() - 1;
            CMenuAction menu = menuActions.addMenuViewOrder();
            if (m_menuActions.isEmpty())
            {
                // predefine menus
                m_menuActions = QList<QAction *>({ nullptr, nullptr, nullptr, nullptr });

                if (!m_menuActions[0])
                {
                    m_frame = new QFrame(this);
                    QHBoxLayout *layout = new QHBoxLayout(m_frame);
                    layout->setContentsMargins(2, 2, 2, 2);
                    m_frame->setLayout(layout);
                    m_leOrder = new QLineEdit(m_frame);
                    QLabel *icon = new QLabel(m_frame);
                    icon->setPixmap(menu.getPixmap());
                    layout->addWidget(icon);
                    QLabel *label = new QLabel(m_frame);
                    label->setText("Order:");
                    layout->addWidget(label);
                    layout->addWidget(m_leOrder);
                    m_validator = new QIntValidator(0, maxOrder, this);
                    m_leOrder->setValidator(m_validator);
                    QWidgetAction *orderAction = new QWidgetAction(this);
                    orderAction->setDefaultWidget(m_frame);
                    QObject::connect(m_leOrder, &QLineEdit::returnPressed, this, &COrderableViewWithDbObjects<T>::orderToLineEdit);
                    m_menuActions[0] = orderAction;
                }
            }

            m_validator->setRange(0, maxOrder);
            m_leOrder->setPlaceholderText("New order 0-" + QString::number(maxOrder));

            menuActions.addAction(m_menuActions[0], CMenuAction::pathViewOrder());
            m_menuActions[1] = menuActions.addAction(CIcons::arrowMediumNorth16(), "To top", CMenuAction::pathViewOrder(), { this, &COrderableViewWithDbObjects<T>::orderToTop });
            m_menuActions[2] = menuActions.addAction(CIcons::arrowMediumSouth16(), "To bottom", CMenuAction::pathViewOrder(), { this, &COrderableViewWithDbObjects<T>::orderToBottom });
            m_menuActions[3] = menuActions.addAction(CIcons::arrowMediumWest16(), "Freeze current order", CMenuAction::pathViewOrder(), { this, &COrderableViewWithDbObjects<T>::freezeCurrentOrder });
        }
        CViewWithDbObjects<T>::customMenu(menuActions);
    }

    template <class T>
    void COrderableViewWithDbObjects<T>::selectObjects(const ContainerType &selectedObjects)
    {
        if (selectedObjects.isEmpty()) { return; }
        this->selectDbKeys(selectedObjects.toDbKeySet());
    }

    template <class T>
    void COrderableViewWithDbObjects<T>::moveSelectedItems(int order)
    {
        if (this->isEmpty()) { return; }
        const ContainerType objs(this->selectedObjects());
        if (objs.isEmpty()) { return; }
        this->m_model->moveItems(objs, order);
    }

    template <class T>
    void COrderableViewWithDbObjects<T>::orderToTop()
    {
        this->moveSelectedItems(0);
    }

    template <class T>
    void COrderableViewWithDbObjects<T>::orderToBottom()
    {
        int c = this->model()->rowCount() - 1;
        if (c >= 0)
        {
            this->moveSelectedItems(c);
        }
    }

    template <class T>
    void COrderableViewWithDbObjects<T>::orderToLineEdit()
    {
        if (this->isEmpty()) { return; }
        QLineEdit *le = qobject_cast<QLineEdit *>(QObject::sender());
        if (!le || le->text().isEmpty()) { return; }
        const int order = le->text().toInt();
        this->moveSelectedItems(order);
    }

    template <class T>
    void COrderableViewWithDbObjects<T>::freezeCurrentOrder()
    {
        ContainerType objects = this->container();
        objects.freezeOrder();
        this->updateContainerAsync(objects, false);
    }

    // see here for the reason of thess forward instantiations
    // https://isocpp.org/wiki/faq/templates#separate-template-fn-defn-from-decl

    template class CViewWithDbObjects<swift::gui::models::CAircraftIcaoCodeListModel>;
    template class CViewWithDbObjects<swift::gui::models::CAircraftCategoryListModel>;
    template class CViewWithDbObjects<swift::gui::models::CAirlineIcaoCodeListModel>;
    template class CViewWithDbObjects<swift::gui::models::CCountryListModel>;
    template class CViewWithDbObjects<swift::gui::models::CLiveryListModel>;
    template class CViewWithDbObjects<swift::gui::models::CDistributorListModel>;
    template class CViewWithDbObjects<swift::gui::models::CAircraftModelListModel>;
    template class COrderableViewWithDbObjects<swift::gui::models::CDistributorListModel>;
    template class COrderableViewWithDbObjects<swift::gui::models::CAircraftModelListModel>;

} // namespace swift::gui::views
