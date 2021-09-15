/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_INSTALLXSWIFTBUSCOMPONENT_H
#define BLACKGUI_COMPONENTS_INSTALLXSWIFTBUSCOMPONENT_H

#include "blackgui/overlaymessagesframe.h"
#include "blackgui/loadindicator.h"
#include "blackcore/application/applicationsettings.h"
#include "blackcore/application/updatesettings.h"
#include "blackmisc/db/updateinfo.h"
#include "blackmisc/simulation/settings/simulatorsettings.h"
#include "blackmisc/network/remotefilelist.h"
#include <QNetworkReply>
#include <QFileDialog>
#include <QWizard>
#include <QScopedPointer>

namespace Ui { class CInstallXSwiftBusComponent; }
namespace BlackGui::Components
{
    /**
     * Download and install XSwiftBus
     */
    class CInstallXSwiftBusComponent :
        public COverlayMessagesFrame,
        public CLoadIndicatorEnabled
    {
        Q_OBJECT

    public:
        //! Default constructor
        explicit CInstallXSwiftBusComponent(QWidget *parent = nullptr);

        //! Dtor
        virtual ~CInstallXSwiftBusComponent();

        //! Set a default name for download
        void setDefaultDownloadName(const QString &defaultDownload);

    private:
        static constexpr int OverlayMsgTimeoutMs = 5000; //!< how long overlay is displayed

        QScopedPointer<Ui::CInstallXSwiftBusComponent> ui;
        BlackMisc::Simulation::Settings::CMultiSimulatorSettings m_simulatorSettings { this }; //!< for directories of XPlane
        BlackMisc::CDataReadOnly<BlackMisc::Db::TUpdateInfo> m_updates { this, &CInstallXSwiftBusComponent::updatesChanged };
        BlackMisc::CSettingReadOnly<BlackCore::Application::TUpdatePreferences> m_updateSettings { this }; //!< channel/platform selected
        const QFileDialog::Options m_fileDialogOptions { QFileDialog::ShowDirsOnly | QFileDialog::ReadOnly | QFileDialog::DontResolveSymlinks };

        // the xSwiftBus artifacts
        QString m_defaultDownloadName; //!< default name for download
        BlackMisc::Db::CArtifactList m_xSwiftBusArtifacts; //!< selectable artifacts

        //! Select X-Plane plugin directory
        void selectPluginDirectory();

        //! Select download directory
        void selectDownloadDirectory();

        //! Install from download directory to X-Plane directory
        void installXSwiftBus();

        //! Trigger downloading of the XSwiftBus file
        void triggerDownloadingOfXSwiftBusFile();

        //! Downloaded XSwiftBus file
        void downloadedXSwiftBusFile(const BlackMisc::CStatusMessage &status);

        //! Full filename + path for the downloaded XSwiftBus file
        BlackMisc::Network::CRemoteFile getRemoteFileSelected() const;

        //! Download dir from UI
        QString downloadDir() const;

        //! XSwiftBus dir from UI
        QString xSwiftBusDir() const;

        //! Is the download dir existing?
        bool existsDownloadDir() const;

        //! Is the install dir existing?
        bool existsXSwiftBusPluginDir() const;

        //! X-Plane directory from settings of default directory
        QString getXPlanePluginDirectory() const;

        //! Updates have been changed
        void updatesChanged();

        //! Show install dir
        void openInstallDir();

        //! Show download dir
        void openDownloadDir();
    };

    /**
     * Wizard page for CInstallXSwiftBusWizardPage
     */
    class CInstallXSwiftBusWizardPage : public QWizardPage
    {
        Q_OBJECT

    public:
        //! Constructors
        using QWizardPage::QWizardPage;

        //! Set config
        void setConfigComponent(CInstallXSwiftBusComponent *config) { m_config = config; }

        //! \copydoc QWizardPage::validatePage
        virtual bool validatePage() override;

    private:
        CInstallXSwiftBusComponent *m_config = nullptr;
    };
} // ns
#endif // guard
