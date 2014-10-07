/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "dockwidgetinfoareacomponent.h"

namespace BlackGui
{
    namespace Components
    {

        CDockWidgetInfoAreaComponent::CDockWidgetInfoAreaComponent(QWidget *parent)
        {
            // it the parent is already an info area at this time, we keep it
            // otherwise we expect the info area to set it later
            CDockWidgetInfoArea *ia = dynamic_cast<CDockWidgetInfoArea *>(parent);
            if (ia)
            {
                this->m_parentDockableInfoArea = ia;
            }
        }

        bool CDockWidgetInfoAreaComponent::setParentDockWidgetInfoArea(CDockWidgetInfoArea *parentDockableWidget)
        {
            // sanity check
            if (this->m_parentDockableInfoArea)
            {
                // we already have a value
                // changes should not happen
                Q_ASSERT(this->m_parentDockableInfoArea == parentDockableWidget);
                return this->m_parentDockableInfoArea == parentDockableWidget;
            }

            m_parentDockableInfoArea = parentDockableWidget;
            return true;
        }

        const CInfoArea *CDockWidgetInfoAreaComponent::getParentInfoArea() const
        {
            Q_ASSERT(this->m_parentDockableInfoArea);
            if (!this->m_parentDockableInfoArea) return nullptr;
            return this->m_parentDockableInfoArea->getParentInfoArea();
        }

        CInfoArea *CDockWidgetInfoAreaComponent::getParentInfoArea()
        {
            Q_ASSERT(this->m_parentDockableInfoArea);
            if (!this->m_parentDockableInfoArea) return nullptr;
            return this->m_parentDockableInfoArea->getParentInfoArea();
        }

        bool CDockWidgetInfoAreaComponent::isParentDockWidgetFloating() const
        {
            Q_ASSERT(this->m_parentDockableInfoArea);
            if (!this->m_parentDockableInfoArea) return false;
            return this->m_parentDockableInfoArea->isFloating();
        }

        bool CDockWidgetInfoAreaComponent::isVisibleWidget() const
        {
            Q_ASSERT(this->m_parentDockableInfoArea);
            if (!this->m_parentDockableInfoArea) return false;
            return this->m_parentDockableInfoArea->isVisibleWidget();
        }

    } // namespace
} // namespace
