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
#include "blackgui/loadindicator.h"
#include "blackmisc/digestsignal.h"
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

            //! Initialize
            void display();

            //! Visible DB refresh buttons
            void setVisibleDbRefreshButtons(bool visible);

            //! Visible shared refresh buttons
            void setVisibleSharedRefreshButtons(bool visible);

            //! Showing load indicator?
            bool isShowingLoadIndicator() const;

        signals:
            //! Trigger GUI update
            //! \private
            void ps_triggerDigestGuiUpdate();

        protected:
            //! \copydoc QWidget::resizeEvent
            virtual void resizeEvent(QResizeEvent *event) override;

            //! Center load indicator
            void centerLoadIndicator();

        private:
            QScopedPointer<Ui::CDbLoadOverviewComponent> ui;
            BlackGui::CLoadIndicator *m_loadIndicator = nullptr; //!< load indicator if needed
            bool m_reloading = false;
            BlackMisc::CDigestSignal m_dsTriggerGuiUpdate  { this, &CDbLoadOverviewComponent::ps_triggerDigestGuiUpdate, &CDbLoadOverviewComponent::ps_setValues, 750, 4 };

            //! Show loading
            void showLoading();

            //! Values at least set once
            bool isInitialized() const;

            //! Timestamp
            static QString formattedTimestamp(const QDateTime &dateTime);

            //! Formatted ts for entity
            static QString cacheTimestampForEntity(BlackMisc::Network::CEntityFlags::Entity entity);

            //! Formatted ts for entity
            static QString dbTimestampForEntity(BlackMisc::Network::CEntityFlags::Entity entity);

            //! Formatted count for entity
            static QString cacheCountForEntity(BlackMisc::Network::CEntityFlags::Entity entity);

            //! Formatted ts for entity
            static QString sharedFileTimestampForEntity(BlackMisc::Network::CEntityFlags::Entity entity);

            //! Formatted count for entity
            static QString dbCountForEntity(BlackMisc::Network::CEntityFlags::Entity entity);

            //! Admit caches
            static void admitCaches();

        private slots:
            //! Refresh directly from DB
            void ps_refreshDbPressed();

            //! Refresh directly from DB
            void ps_refreshSharedPressed();

            //! Init the value panel
            void ps_setValues();

            //! Data have been loaded
            void ps_dataLoaded(BlackMisc::Network::CEntityFlags::Entity entities, BlackMisc::Network::CEntityFlags::ReadState state, int number);
        };
    } // ns
} // ns
#endif // guard
