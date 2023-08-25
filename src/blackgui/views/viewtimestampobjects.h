// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_VIEWS_VIEWTIMESTAMPOBJECTS_H
#define BLACKGUI_VIEWS_VIEWTIMESTAMPOBJECTS_H

#include "blackgui/views/viewbase.h"
#include <QObject>
#include <QString>
#include <QtGlobal>

namespace BlackGui
{
    namespace Menus
    {
        class CMenuActions;
    }
    namespace Views
    {
        //! Base class for views timestamp offset objects
        template <class T>
        class CViewWithTimestampWithOffsetObjects :
            public CViewBase<T>
        {
        public:
            //! Model type
            using ModelClass = T;

            //! Model container type
            using ContainerType = typename T::ContainerType;

            //! Model container element type
            using ObjectType = typename T::ObjectType;

            //! Insert as first element by keeping maxElements and the latest first
            void push_frontKeepLatestAdjustedFirst(const ObjectType &object, int max);

        protected:
            //! Constructor
            explicit CViewWithTimestampWithOffsetObjects(QWidget *parent = nullptr);
        };
    } // namespace
} // namespace
#endif // guard
