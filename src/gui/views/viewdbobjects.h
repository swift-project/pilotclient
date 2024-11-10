// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_VIEWS_VIEWDBOBJECTS_H
#define SWIFT_GUI_VIEWS_VIEWDBOBJECTS_H

#include "gui/views/viewbase.h"
#include <QSet>
#include <QObject>
#include <QString>
#include <QtGlobal>

class QAction;
class QIntValidator;
class QFrame;
class QLineEdit;
class QWidget;

namespace swift::gui
{
    namespace menus
    {
        class CMenuActions;
    }
    namespace views
    {
        //! Base class for views with DB objects
        template <class T>
        class CViewWithDbObjects :
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

            //! \copydoc swift::gui::views::CViewBaseNonTemplate::customMenu
            virtual void customMenu(swift::gui::menus::CMenuActions &menuActions) override;
        };

        //! Base class for views with DB objects also orderable (based on swift::misc::IOrderableList )
        template <class T>
        class COrderableViewWithDbObjects :
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

            //! \copydoc swift::gui::views::CViewBaseNonTemplate::customMenu
            virtual void customMenu(swift::gui::menus::CMenuActions &menuActions) override;

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
            QLineEdit *m_leOrder = nullptr;
            QFrame *m_frame = nullptr;
            QIntValidator *m_validator = nullptr;
        };
    } // namespace
} // namespace

#endif // guard
