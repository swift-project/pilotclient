/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_VIEWS_VIEWTIMESTAMPOBJECTS_H
#define BLACKGUI_VIEWS_VIEWTIMESTAMPOBJECTS_H

#include "viewbase.h"
#include <QObject>
#include <QString>
#include <QtGlobal>

namespace BlackGui
{
    namespace Menus { class CMenuActions; }
    namespace Views
    {
        //! Base class for views timestamp offset objects
        template <class ModelClass, class ContainerType, class ObjectType> class CViewWithTimestampWithOffsetObjects :
            public CViewBase<ModelClass, ContainerType, ObjectType>
        {
        public:
            //! Insert as first element by keeping maxElements and the latest first
            void push_frontKeepLatestAdjustedFirst(const ObjectType &object, int max);

        protected:
            //! Constructor
            explicit CViewWithTimestampWithOffsetObjects(QWidget *parent = nullptr);
        };
    } // namespace
} // namespace
#endif // guard
