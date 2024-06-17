// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_COMPONENTS_COPYSETTINGSANDCACHESCOMPONENT_H
#define BLACKGUI_COMPONENTS_COPYSETTINGSANDCACHESCOMPONENT_H

#include "blackgui/settings/guisettings.h"
#include "blackgui/settings/dockwidgetsettings.h"
#include "blackgui/settings/viewupdatesettings.h"
#include "blackgui/settings/textmessagesettings.h"
#include "blackgui/overlaymessagesframe.h"
#include "blackgui/blackguiexport.h"
#include "blackcore/data/launchersetup.h"
#include "blackcore/data/vatsimsetup.h"
#include "blackcore/audio/audiosettings.h"
#include "blackcore/application/applicationsettings.h"
#include "blackcore/application/updatesettings.h"
#include "blackmisc/simulation/data/modelcaches.h"
#include "blackmisc/simulation/data/lastmodel.h"
#include "blackmisc/simulation/settings/simulatorsettings.h"
#include "blackmisc/simulation/settings/modelsettings.h"
#include "blackmisc/network/data/lastserver.h"
#include "blackmisc/network/settings/serversettings.h"
#include "blackmisc/audio/audiosettings.h"
#include "blackmisc/directories.h"
#include "blackmisc/applicationinfo.h"

#include <QWizardPage>
#include <QScopedPointer>
#include <QCheckBox>
#include <QList>

namespace Ui
{
    class CCopySettingsAndCachesComponent;
}
namespace BlackGui::Components
{
    /*!
     * Copy settings and caches
     */
    class CCopySettingsAndCachesComponent : public COverlayMessagesFrame
    {
        Q_OBJECT

    public:
        //! Ctor
        explicit CCopySettingsAndCachesComponent(QWidget *parent = nullptr);

        //! Dtor
        virtual ~CCopySettingsAndCachesComponent();

        //! Reload other versions
        void reloadOtherVersions(int deferMs = -1);

    private:
        //! Other version has been changed
        void onOtherVersionChanged(const BlackMisc::CApplicationInfo &info);

        //! Init UI
        void initAll();

        //! Audio init
        void initAudio();

        //! Network init
        void initNetwork();

        //! UI init
        void initUi();

        //! Simulator init
        void initSimulator();

        //! Misc. init
        void initMisc();

        //! Model init
        void initModel();

        //! Copy
        int copy();

        //! Select all
        void selectAll();

        //! Deselect all
        void deselectAll();

        //! All checkboxes
        QList<QCheckBox *> checkBoxes() const;

        //! Display parsing message
        bool parsingMessage(bool success, const QString &msg, const QString &value);

        //! Display status message
        void displayStatusMessage(const BlackMisc::CStatusMessage &msg, const QString &value);

        //! All check boxes read only
        void allCheckBoxesReadOnly();

        //! Enable checkbox
        static void readOnlyCheckbox(QCheckBox *cb, bool readOnly);

        //! Checkbox text
        static QString checkBoxText(const QString &text, bool setting);

        QScopedPointer<Ui::CCopySettingsAndCachesComponent> ui;

        BlackMisc::CSetting<BlackCore::Audio::TInputDevice> m_settingsAudioInputDevice { this };
        BlackMisc::CSetting<BlackCore::Audio::TOutputDevice> m_settingsAudioOutputDevice { this };
        BlackMisc::CSetting<Settings::TGeneralGui> m_settingsGuiGeneral { this };
        BlackMisc::CSetting<Settings::TDockWidget> m_settingsDockWidget { this };
        BlackMisc::CSetting<Settings::TViewUpdateSettings> m_settingsViewUpdate { this };
        BlackMisc::CSetting<Settings::TBackgroundConsolidation> m_settingsConsolidation { this }; //!< consolidation time
        BlackMisc::CSetting<Settings::TextMessageSettings> m_settingsTextMessage { this };
        BlackMisc::CSetting<BlackCore::Application::TEnabledSimulators> m_settingsEnabledSimulators { this };
        BlackMisc::CSetting<BlackCore::Application::TActionHotkeys> m_settingsActionHotkeys { this };
        BlackMisc::CSetting<BlackMisc::Audio::TSettings> m_settingsAudio { this };
        BlackMisc::CSetting<BlackMisc::Settings::TDirectorySettings> m_settingsDirectories { this };
        BlackMisc::CSetting<BlackMisc::Network::Settings::TTrafficServers> m_settingsNetworkServers { this };
        BlackMisc::CSetting<BlackMisc::Simulation::Settings::TSimulatorFsx> m_settingsSimulatorFsx { this }; //!< FSX settings
        BlackMisc::CSetting<BlackMisc::Simulation::Settings::TSimulatorFs9> m_settingsSimulatorFs9 { this }; //!< FS9 settings
        BlackMisc::CSetting<BlackMisc::Simulation::Settings::TSimulatorP3D> m_settingsSimulatorP3D { this }; //!< P3D settings
        BlackMisc::CSetting<BlackMisc::Simulation::Settings::TSimulatorXP> m_settingsSimulatorXPlane { this }; //!< XP settings
        BlackMisc::CSetting<BlackMisc::Simulation::Settings::TModel> m_settingsModel { this }; //!< model setting

        BlackMisc::CData<BlackCore::Data::TVatsimLastServer> m_cacheLastVatsimServer { this }; //!< recently used VATSIM server
        BlackMisc::CData<BlackCore::Data::TLauncherSetup> m_cacheLauncherSetup { this };
        BlackMisc::CData<BlackCore::Data::TVatsimSetup> m_cacheVatsimSetup { this };
        BlackMisc::CData<BlackMisc::Network::Data::TLastServer> m_cacheLastNetworkServer { this }; //!< recently used server (VATSIM, other)
        BlackMisc::CData<BlackMisc::Simulation::Data::TSimulatorLastSelection> m_cacheModelSetCurrentSimulator { this };
        BlackMisc::CData<BlackMisc::Simulation::Data::TModelCacheLastSelection> m_cacheModelsCurrentSimulator { this };
        BlackMisc::CData<BlackMisc::Simulation::Data::TLastModel> m_cacheLastAircraftModel { this }; //!< recently used aircraft model
    };

    /*!
     * Wizard page for CCopySettingsAndCaches
     */
    class CCopySettingsAndCachesWizardPage : public QWizardPage
    {
    public:
        //! Constructors
        using QWizardPage::QWizardPage;

        //! Set config
        void setConfigComponent(CCopySettingsAndCachesComponent *config) { m_copyCachesAndSettings = config; }

        //! \copydoc QWizardPage::initializePage
        virtual void initializePage() override;

        //! \copydoc QWizardPage::validatePage
        virtual bool validatePage() override;

    private:
        CCopySettingsAndCachesComponent *m_copyCachesAndSettings = nullptr;
    };
} // ns

#endif // guard
