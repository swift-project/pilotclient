// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_DBLOADOVERVIEWCOMPONENT_H
#define SWIFT_GUI_COMPONENTS_DBLOADOVERVIEWCOMPONENT_H

#include "gui/loadindicator.h"
#include "gui/swiftguiexport.h"
#include "misc/digestsignal.h"
#include "misc/network/entityflags.h"

#include <QFrame>
#include <QScopedPointer>

namespace Ui
{
    class CDbLoadOverviewComponent;
}
namespace swift::gui::components
{
    /*!
     * Overview about load state of DB data
     */
    class SWIFT_GUI_EXPORT CDbLoadOverviewComponent :
        public QFrame,
        public swift::gui::CLoadIndicatorEnabled
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CDbLoadOverviewComponent(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CDbLoadOverviewComponent() override;

        //! Initialize
        void display();

        //! Visible DB refresh buttons
        void showVisibleDbRefreshButtons(bool visible);

        //! Visible shared refresh buttons
        void showVisibleSharedRefreshButtons(bool visible);

        //! Show load all buttons
        void showVisibleLoadAllButtons(bool shared, bool db, bool disk);

        //! Load all from DB
        void loadAllFromDb();

        //! Load all from shared
        void loadAllFromShared();

        //! Load all from disk (as of installation)
        void loadAllFromResourceFiles();

        //! Is loading in progress
        bool isLoadInProgress() const { return m_loadInProgress; }

    protected:
        //! \copydoc QWidget::resizeEvent
        virtual void resizeEvent(QResizeEvent *event) override;

    private:
        QScopedPointer<Ui::CDbLoadOverviewComponent> ui;
        swift::misc::CDigestSignal m_dsTriggerGuiUpdate { this, &CDbLoadOverviewComponent::setGuiValues, 2500, 5 };
        qint64 m_sharedLastCheck = -1; //!< when shared URLs were last checked
        bool m_sharedValueCheckInProgress = false; //!< setting values in progress, needed because of CNetworkUtils::canConnect check (processing events)
        const QString m_imgOk = ":/diagona/icons/diagona/icons/tick.png";
        const QString m_imgFailed = ":/diagona/icons/diagona/icons/cross-script.png";

        //! Trigger loading from DB
        void triggerLoadingFromDb(swift::misc::network::CEntityFlags::Entity entities);

        //! Trigger loading from shared files
        void triggerLoadingFromSharedFiles(swift::misc::network::CEntityFlags::Entity entities);

        //! Values at least set once
        bool isInitialized() const;

        //! Refresh directly from DB
        void refreshDbPressed();

        //! Refresh directly from shared files
        void refreshSharedPressed();

        //! Init the value panel
        void setGuiValues();

        //! Shared URL values
        void setSharedUrlValues();

        //! Data have been loaded
        void dataLoaded(swift::misc::network::CEntityFlags::Entity entities, swift::misc::network::CEntityFlags::ReadState state, int number, const QUrl &url);

        //! Load info objects if not already loaded
        void loadInfoObjects();

        //! Adjust text width
        void adjustTextWidth();

        //! Timestamp
        static QString formattedTimestamp(const QDateTime &dateTime);

        //! Formatted ts for entity (cache)
        static QString cacheTimestampForEntity(swift::misc::network::CEntityFlags::Entity entity);

        //! Formatted count for entity (cache)
        static QString cacheCountForEntity(swift::misc::network::CEntityFlags::Entity entity);

        //! Formatted ts for entity (DB)
        static QString dbTimestampForEntity(swift::misc::network::CEntityFlags::Entity entity);

        //! Formatted count for entity (DB)
        static QString dbCountForEntity(swift::misc::network::CEntityFlags::Entity entity);

        //! Formatted ts for entity (shared)
        static QString sharedFileTimestampForEntity(swift::misc::network::CEntityFlags::Entity entity);

        //! Formatted count for entity (shared)
        static QString sharedCountForEntity(swift::misc::network::CEntityFlags::Entity entity);

        //! Admit caches
        static void admitCaches();
    };
} // ns
#endif // guard
