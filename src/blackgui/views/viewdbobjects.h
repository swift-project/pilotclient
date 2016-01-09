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

            //! \copydoc QWidget::customMenu
            virtual void customMenu(QMenu &menu) const override;

        protected slots:
            //! \copydoc CViewBaseNonTemplate::ps_toggleHighlightDbData
            virtual void ps_toggleHighlightDbData() override;
        };
    } // namespace
} // namespace
#endif // guard
