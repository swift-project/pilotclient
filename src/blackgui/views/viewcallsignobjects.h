// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_VIEWS_VIEWCALLSIGNOBJECTS_H
#define BLACKGUI_VIEWS_VIEWCALLSIGNOBJECTS_H

#include "blackgui/views/viewbase.h"
#include <QSet>
#include <QObject>
#include <QString>
#include <QtGlobal>

class QAction;
class QIntValidator;
class QLineEdit;
class QWidget;

namespace BlackGui
{
    namespace Menus
    {
        class CMenuActions;
    }
    namespace Views
    {
        //! Base class for views with DB objects
        template <class T>
        class CViewWithCallsignObjects :
            public CViewBase<T>
        {
        public:
            //! Model type
            using ModelClass = T;

            //! Model container type
            using ContainerType = typename T::ContainerType;

            //! Model container element type
            using ObjectType = typename T::ObjectType;

            //! Select callsign
            void selectCallsign(const BlackMisc::Aviation::CCallsign &callsign);

            //! Select given callsigns
            void selectCallsigns(const BlackMisc::Aviation::CCallsignSet &callsigns);

            //! Get selected callsigns
            BlackMisc::Aviation::CCallsignSet selectedCallsigns() const;

            //! Remove callsigns
            int removeCallsigns(const BlackMisc::Aviation::CCallsignSet &callsigns);

            //! Remove callsign
            int removeCallsign(const BlackMisc::Aviation::CCallsign &callsign);

            //! Update or insert data (based on callsign)
            int replaceOrAddObjectsByCallsign(const ContainerType &container);

            //! Update or insert data (based on callsign)
            int replaceOrAddObjectByCallsign(const ObjectType &object);

            //! Reselect by callsigns
            virtual void selectObjects(const ContainerType &selectedObjects) override;

        protected:
            //! Constructor
            explicit CViewWithCallsignObjects(QWidget *parent = nullptr);
        };
    } // namespace
} // namespace
#endif // guard
