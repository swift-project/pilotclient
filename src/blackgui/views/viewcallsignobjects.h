/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_VIEWS_VIEWCALLSIGNOBJECTS_H
#define BLACKGUI_VIEWS_VIEWCALLSIGNOBJECTS_H

#include "viewbase.h"
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
    namespace Menus { class CMenuActions; }
    namespace Views
    {
        //! Base class for views with DB objects
        template <class ModelClass, class ContainerType, class ObjectType> class CViewWithCallsignObjects :
            public CViewBase<ModelClass, ContainerType, ObjectType>
        {
        public:
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
