// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_COMPONENTS_UPDATEINFOCOMPONENT_H
#define BLACKGUI_COMPONENTS_UPDATEINFOCOMPONENT_H

#include "core/application/updatesettings.h"
#include "blackgui/blackguiexport.h"
#include "misc/db/updateinfo.h"
#include "misc/db/artifact.h"
#include "misc/settingscache.h"
#include "misc/digestsignal.h"
#include <QFrame>

namespace Ui
{
    class CUpdateInfoComponent;
}
namespace BlackGui::Components
{
    class CInstallXSwiftBusDialog;
    class CDownloadDialog;

    /*!
     * Update info (distributions, artifacts etc.)
     */
    class BLACKGUI_EXPORT CUpdateInfoComponent : public QFrame
    {
        Q_OBJECT

    public:
        //! Ctor
        explicit CUpdateInfoComponent(QWidget *parent = nullptr);

        //! Dtor
        virtual ~CUpdateInfoComponent() override;

        //! Is there a new version available return version, else empty string
        swift::misc::db::CArtifact getLatestAvailablePilotClientArtifactForSelection() const;

        //! Is there a new version available?
        bool isNewPilotClientVersionAvailable() const;

        //! Trigger download
        void triggerDownload();

        //! Current distribution
        swift::misc::db::CDistribution getCurrentDistribution() const { return this->getSelectedOrDefaultDistribution(); }

    signals:
        //! Update info loaded
        void updateInfoAvailable();

        //! A newer pilot client is available
        void newerPilotClientAvailable(const swift::misc::db::CArtifact &latestPilotClient);

        //! New platfrom or channel
        void selectionChanged();

    private:
        QScopedPointer<Ui::CUpdateInfoComponent> ui;
        QScopedPointer<CInstallXSwiftBusDialog> m_installXSwiftBusDialog; //!< dialog, install XSwiftXBus
        QScopedPointer<CDownloadDialog> m_downloadDialog; //!< dialog, download installer
        swift::misc::CDataReadOnly<swift::misc::db::TUpdateInfo> m_updateInfo { this, &CUpdateInfoComponent::changedUpdateInfo }; //!< version cache
        swift::misc::CSetting<swift::core::application::TUpdatePreferences> m_updateSettings { this }; //!< channel/platform selected
        swift::misc::CDigestSignal m_dsDistributionAvailable { this, &CUpdateInfoComponent::updateInfoAvailable, 15000, 2 };

        //! Load latest version
        void requestLoadOfSetup();

        //! Loaded latest version
        void changedUpdateInfo();

        //! Channel has been changed
        void channelChanged();

        //! Platform changed
        void platformChanged();

        //! Selection changed
        void uiSelectionChanged();

        //! Install xswiftbus dialog
        void downloadXSwiftBusDialog();

        //! Download installer dialog
        void downloadInstallerDialog();

        //! Save the current settings
        void saveSettings();

        //! Selected platform from UI or guessed platform
        const swift::misc::CPlatform &getSelectedOrDefaultPlatform() const;

        //! Selected or default distribution
        swift::misc::db::CDistribution getSelectedOrDefaultDistribution() const;
    };
} // ns
#endif // guard
