/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_VIEWDBOBJECTS_H
#define BLACKGUI_VIEWDBOBJECTS_H

#include "blackgui/blackguiexport.h"
#include "viewbase.h"
#include <QMenu>

namespace BlackGui
{
    namespace Views
    {
        //! Base class for views with DB objects
        template <class ModelClass, class ContainerType, class ObjectType, class KeyType> class CViewWithDbObjects :
            public CViewBase<ModelClass, ContainerType, ObjectType>
        {
        public:
            //! Get latest object
            ObjectType latestObject() const;

            //! Get oldets object
            ObjectType oldestObject() const;

            //! Select given DB keys
            void selectDbKeys(const QList<KeyType> &keys);

            //! Remove keys
            int removeDbKeys(const QList<KeyType> &keys);

            //! Update or insert data (based on DB key)
            int replaceOrAddObjectsByKey(const ContainerType &container);

        protected:
            //! Constructor
            explicit CViewWithDbObjects(QWidget *parent = nullptr);

            //! \copydoc BlackGui::Views::CViewBaseNonTemplate::customMenu
            virtual void customMenu(BlackGui::Menus::CMenuActions &menuActions) override;

        protected slots:
            //! \copydoc BlackGui::Views::CViewBase::ps_toggleHighlightDbData
            virtual void ps_toggleHighlightDbData() override;
        };

        //! Base class for views with DB objects
        template <class ModelClass, class ContainerType, class ObjectType, class KeyType> class COrderableViewWithDbObjects :
            public CViewWithDbObjects<ModelClass, ContainerType, ObjectType, KeyType>
        {
        protected:
            //! Constructor
            explicit COrderableViewWithDbObjects(QWidget *parent = nullptr);

            //! \copydoc BlackGui::Views::CViewBaseNonTemplate::customMenu
            virtual void customMenu(BlackGui::Menus::CMenuActions &menuActions) override;

            //! Move selected items
            void moveSelectedItems(int order);

        protected slots:
            //! Order to top
            void ps_orderToTop();

            //! Order to bottom
            void ps_orderToBottom();

            //! Order to line edit
            void ps_orderToLineEdit();

            //! Current order set as order
            void ps_freezeCurrentOrder();

        private:
            QList<QAction *> m_menuActions;
            QLineEdit       *m_leOrder = nullptr;
            QIntValidator   *m_validator = nullptr;
        };
    } // namespace
} // namespace
#endif // guard
