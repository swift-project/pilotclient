/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_DBLOADOVERVIEWCOMPONENT_H
#define BLACKGUI_COMPONENTS_DBLOADOVERVIEWCOMPONENT_H

#include "blackgui/blackguiexport.h"
#include "blackmisc/network/entityflags.h"
#include <QFrame>
#include <QScopedPointer>

namespace Ui  { class CDbLoadOverviewComponent; }
namespace BlackGui
{
    namespace Components
    {
        /*!
         * Overview about load state of DB data
         */
        class BLACKGUI_EXPORT CDbLoadOverviewComponent : public QFrame
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CDbLoadOverviewComponent(QWidget *parent = nullptr);

            //! Destructor
            virtual ~CDbLoadOverviewComponent();

        private:
            QScopedPointer<Ui::CDbLoadOverviewComponent> ui;

            //! Init the value panel
            void ps_setValues();

            //! Timestamp
            static QString formattedTimestamp(const QDateTime &dateTime);

            //! Formatted ts for entity
            static QString cacheTimestampForEntity(BlackMisc::Network::CEntityFlags::Entity entity);

            //! Formatted ts for entity
            static QString dbTimestampForEntity(BlackMisc::Network::CEntityFlags::Entity entity);

            //! Formatted count for entity
            static QString cacheCountForEntity(BlackMisc::Network::CEntityFlags::Entity entity);

            //! Formatted count for entity
            static QString dbCountForEntity(BlackMisc::Network::CEntityFlags::Entity entity);

        private slots:
            //! Reload
            void ps_reloadPressed();
        };
    } // ns
} // ns
#endif // guard
