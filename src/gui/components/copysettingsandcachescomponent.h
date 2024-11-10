// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_COPYSETTINGSANDCACHESCOMPONENT_H
#define SWIFT_GUI_COMPONENTS_COPYSETTINGSANDCACHESCOMPONENT_H

#include "gui/settings/guisettings.h"
#include "gui/settings/dockwidgetsettings.h"
#include "gui/settings/viewupdatesettings.h"
#include "gui/settings/textmessagesettings.h"
#include "gui/overlaymessagesframe.h"
#include "gui/swiftguiexport.h"
#include "core/data/launchersetup.h"
#include "core/data/vatsimsetup.h"
#include "core/audio/audiosettings.h"
#include "core/application/applicationsettings.h"
#include "core/application/updatesettings.h"
#include "misc/simulation/data/modelcaches.h"
#include "misc/simulation/data/lastmodel.h"
#include "misc/simulation/settings/simulatorsettings.h"
#include "misc/simulation/settings/modelsettings.h"
#include "misc/network/data/lastserver.h"
#include "misc/network/settings/serversettings.h"
#include "misc/audio/audiosettings.h"
#include "misc/directories.h"
#include "misc/applicationinfo.h"

#include <QWizardPage>
#include <QScopedPointer>
#include <QCheckBox>
#include <QList>

namespace Ui
{
    class CCopySettingsAndCachesComponent;
}
namespace swift::gui::components
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
        void onOtherVersionChanged(const swift::misc::CApplicationInfo &info);

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
        void displayStatusMessage(const swift::misc::CStatusMessage &msg, const QString &value);

        //! All check boxes read only
        void allCheckBoxesReadOnly();

        //! Enable checkbox
        static void readOnlyCheckbox(QCheckBox *cb, bool readOnly);

        //! Checkbox text
        static QString checkBoxText(const QString &text, bool setting);

        QScopedPointer<Ui::CCopySettingsAndCachesComponent> ui;

        swift::misc::CSetting<swift::core::audio::TInputDevice> m_settingsAudioInputDevice { this };
        swift::misc::CSetting<swift::core::audio::TOutputDevice> m_settingsAudioOutputDevice { this };
        swift::misc::CSetting<settings::TGeneralGui> m_settingsGuiGeneral { this };
        swift::misc::CSetting<settings::TDockWidget> m_settingsDockWidget { this };
        swift::misc::CSetting<settings::TViewUpdateSettings> m_settingsViewUpdate { this };
        swift::misc::CSetting<settings::TBackgroundConsolidation> m_settingsConsolidation { this }; //!< consolidation time
        swift::misc::CSetting<settings::TextMessageSettings> m_settingsTextMessage { this };
        swift::misc::CSetting<swift::core::application::TEnabledSimulators> m_settingsEnabledSimulators { this };
        swift::misc::CSetting<swift::core::application::TActionHotkeys> m_settingsActionHotkeys { this };
        swift::misc::CSetting<swift::misc::audio::TSettings> m_settingsAudio { this };
        swift::misc::CSetting<swift::misc::settings::TDirectorySettings> m_settingsDirectories { this };
        swift::misc::CSetting<swift::misc::network::settings::TTrafficServers> m_settingsNetworkServers { this };
        swift::misc::CSetting<swift::misc::simulation::settings::TSimulatorFsx> m_settingsSimulatorFsx { this }; //!< FSX settings
        swift::misc::CSetting<swift::misc::simulation::settings::TSimulatorFs9> m_settingsSimulatorFs9 { this }; //!< FS9 settings
        swift::misc::CSetting<swift::misc::simulation::settings::TSimulatorP3D> m_settingsSimulatorP3D { this }; //!< P3D settings
        swift::misc::CSetting<swift::misc::simulation::settings::TSimulatorXP> m_settingsSimulatorXPlane { this }; //!< XP settings
        swift::misc::CSetting<swift::misc::simulation::settings::TModel> m_settingsModel { this }; //!< model setting

        swift::misc::CData<swift::core::data::TVatsimLastServer> m_cacheLastVatsimServer { this }; //!< recently used VATSIM server
        swift::misc::CData<swift::core::data::TLauncherSetup> m_cacheLauncherSetup { this };
        swift::misc::CData<swift::core::data::TVatsimSetup> m_cacheVatsimSetup { this };
        swift::misc::CData<swift::misc::network::data::TLastServer> m_cacheLastNetworkServer { this }; //!< recently used server (VATSIM, other)
        swift::misc::CData<swift::misc::simulation::data::TSimulatorLastSelection> m_cacheModelSetCurrentSimulator { this };
        swift::misc::CData<swift::misc::simulation::data::TModelCacheLastSelection> m_cacheModelsCurrentSimulator { this };
        swift::misc::CData<swift::misc::simulation::data::TLastModel> m_cacheLastAircraftModel { this }; //!< recently used aircraft model
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
