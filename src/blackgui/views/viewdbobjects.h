/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_VIEWS_VIEWDBOBJECTS_H
#define BLACKGUI_VIEWS_VIEWDBOBJECTS_H

#include "blackgui/views/viewbase.h"
#include <QSet>
#include <QObject>
#include <QString>
#include <QtGlobal>

class QAction;
class QIntValidator;
class QFrame;
class QLineEdit;
class QWidget;

namespace BlackGui
{
    namespace Menus { class CMenuActions; }
    namespace Views
    {
        //! Base class for views with DB objects
        template <class T> class CViewWithDbObjects :
            public CViewBase<T>
        {
        public:
            //! Model type
            using ModelClass = T;

            //! Model container type
            using ContainerType = typename T::ContainerType;

            //! Model container element type
            using ObjectType = typename T::ObjectType;

            //! Model DB key type
            using KeyType = typename T::KeyType;

            //! Get latest object
            ObjectType latestObject() const;

            //! Get oldets object
            ObjectType oldestObject() const;

            //! Select given DB key
            bool selectDbKey(const KeyType &key);

            //! Select given DB keys
            int selectDbKeys(const QSet<KeyType> &keys);

            //! Get selected DB keys
            QSet<KeyType> selectedDbKeys() const;

            //! Remove keys
            int removeDbKeys(const QSet<KeyType> &keys);

            //! Update or insert data (based on DB key)
            int replaceOrAddObjectsByKey(const ContainerType &container);

            //! Select by DB keys
            virtual void selectObjects(const ContainerType &selectedObjects) override;

        protected:
            //! Constructor
            explicit CViewWithDbObjects(QWidget *parent = nullptr);

            //! \copydoc BlackGui::Views::CViewBaseNonTemplate::customMenu
            virtual void customMenu(BlackGui::Menus::CMenuActions &menuActions) override;
        };

        //! Base class for views with DB objects also orderable (based on BlackMisc::IOrderableList )
        template <class T> class COrderableViewWithDbObjects :
            public CViewWithDbObjects<T>
        {
        public:
            //! Model type
            using ModelClass = T;

            //! Model container type
            using ContainerType = typename T::ContainerType;

            //! Model container element type
            using ObjectType = typename T::ObjectType;

            //! Model DB key type
            using KeyType = typename T::KeyType;

            //! Select by DB keys
            virtual void selectObjects(const ContainerType &selectedObjects) override;

        protected:
            //! Constructor
            explicit COrderableViewWithDbObjects(QWidget *parent = nullptr);

            //! \copydoc BlackGui::Views::CViewBaseNonTemplate::customMenu
            virtual void customMenu(BlackGui::Menus::CMenuActions &menuActions) override;

            //! Move selected items
            void moveSelectedItems(int order);

            //! Order to top
            void orderToTop();

            //! Order to bottom
            void orderToBottom();

            //! Order to line edit
            void orderToLineEdit();

            //! Current order set as order
            void freezeCurrentOrder();

        private:
            QList<QAction *> m_menuActions;
            QLineEdit       *m_leOrder   = nullptr;
            QFrame          *m_frame     = nullptr;
            QIntValidator   *m_validator = nullptr;
        };
    } // namespace
} // namespace

#endif // guard
