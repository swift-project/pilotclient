/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_UPDATEINFOCOMPONENT_H
#define BLACKGUI_COMPONENTS_UPDATEINFOCOMPONENT_H

#include "blackcore/application/updatesettings.h"
#include "blackgui/blackguiexport.h"
#include "blackmisc/db/updateinfo.h"
#include "blackmisc/db/artifact.h"
#include "blackmisc/settingscache.h"
#include "blackmisc/digestsignal.h"
#include <QFrame>

namespace Ui { class CUpdateInfoComponent; }
namespace BlackGui
{
    namespace Components
    {
        class CInstallXSwiftBusDialog;
        class CDownloadDialog;

        /**
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
            BlackMisc::Db::CArtifact getLatestAvailablePilotClientArtifactForSelection() const;

            //! Is there a new version available?
            bool isNewPilotClientVersionAvailable() const;

            //! Trigger download
            void triggerDownload();

            //! Current distribution
            BlackMisc::Db::CDistribution getCurrentDistribution() const { return this->getSelectedOrDefaultDistribution(); }

        signals:
            //! Update info loaded
            void updateInfoAvailable();

            //! A newer pilot client is available
            void newerPilotClientAvailable(const BlackMisc::Db::CArtifact &latestPilotClient);

            //! New platfrom or channel
            void selectionChanged();

        private:
            QScopedPointer<Ui::CUpdateInfoComponent> ui;
            QScopedPointer<CInstallXSwiftBusDialog> m_installXSwiftBusDialog; //!< dialog, install XSwiftXBus
            QScopedPointer<CDownloadDialog> m_downloadDialog; //!< dialog, download installer
            BlackMisc::CDataReadOnly<BlackMisc::Db::TUpdateInfo> m_updateInfo { this, &CUpdateInfoComponent::changedUpdateInfo }; //!< version cache
            BlackMisc::CSetting<BlackCore::Application::TUpdatePreferences> m_updateSettings { this }; //!< channel/platform selected
            BlackMisc::CDigestSignal m_dsDistributionAvailable { this, &CUpdateInfoComponent::updateInfoAvailable, 15000, 2 };

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

            //! Install XSwiftBus dialog
            void downloadXSwiftBusDialog();

            //! Download installer dialog
            void downloadInstallerDialog();

            //! Save the current settings
            void saveSettings();

            //! Selected platform from UI or guessed platform
            const BlackMisc::CPlatform &getSelectedOrDefaultPlatform() const;

            //! Selected or default distribution
            BlackMisc::Db::CDistribution getSelectedOrDefaultDistribution() const;
        };
    } // ns
} // ns
#endif // guard
