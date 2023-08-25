// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_COMPONENTS_COPYCONFIGURATION_H
#define BLACKGUI_COMPONENTS_COPYCONFIGURATION_H

#include "blackgui/blackguiexport.h"
#include "blackcore/data/launchersetup.h"
#include "blackcore/data/vatsimsetup.h"
#include "blackmisc/simulation/data/modelcaches.h"
#include "blackmisc/simulation/data/lastmodel.h"
#include "blackmisc/network/data/lastserver.h"
#include <QFrame>
#include <QWizardPage>
#include <QDir>

namespace Ui
{
    class CCopyConfigurationComponent;
}
namespace BlackGui::Components
{
    //! Copy configuration (i.e. settings and cache files)
    //! \deprecated replaced by CCopySettingsAndCachesComponent
    class BLACKGUI_EXPORT CCopyConfigurationComponent : public QFrame
    {
        Q_OBJECT

    public:
        //! Log.categories
        static const QStringList &getLogCategories();

        //! Constructor
        explicit CCopyConfigurationComponent(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CCopyConfigurationComponent() override;

        //! Cache mode
        void setCacheMode();

        //! Settings mode
        void setSettingsMode();

        //! Selected files are copied
        int copySelectedFiles();

        //! Init file content
        void initCurrentDirectories(bool preselectMissingOrOutdated = false);

        //! Are there other versions to copy from?
        bool hasOtherVersionData() const;

        //! Allow to toggle cache and settings
        void allowToggleCacheSettings(bool allow);

        //! Log copied files
        void logCopiedFiles(bool log) { m_logCopiedFiles = log; }

        //! Select all
        void selectAll();

        //! \copydoc QFileSystemModel::setNameFilterDisables
        void setNameFilterDisables(bool disable);

        //! Show bootstrap file?
        void setWithBootstrapFile(bool withBootstrapFile) { m_withBootstrapFile = withBootstrapFile; }

    protected:
        //! \copydoc QWidget::resizeEvent
        virtual void resizeEvent(QResizeEvent *event) override;

    private:
        //! Preselect newer files
        void preselectMissingOrOutdated();

        //! Filter out items from preselection
        //! \remark formally newer files are preselected
        bool preselectActiveFiles(const QString &file) const;

        //! Source file filter
        const QStringList &getSourceFileFilter();

        //! The current version changed
        void currentVersionChanged(const QString &text);

        //! This version's directory (cache or setting)
        const QString &getThisVersionDirectory() const;

        //! Get the selected directory
        QString getOtherVersionsSelectedDirectory() const;

        //! Other the directory of that other version
        void openOtherVersionsSelectedDirectory();

        //! Get the selected files
        QStringList getSelectedFiles() const;

        //! Init caches if required (create .rev entries with high level functions)
        void initCaches(const QStringList &files);

        //! Init a multi simulator cache (modelset/models)
        void initMultiSimulatorCache(BlackMisc::Simulation::Data::IMultiSimulatorModelCaches *cache, const QString &fileName);

        //! Init the other swift versions
        void initOtherSwiftVersions();

        //! Set name filter disables from ui
        void changeNameFilterDisables();

        //! Set widths
        void setWidths();

        QScopedPointer<Ui::CCopyConfigurationComponent> ui;
        QStringList m_otherVersionDirs;
        QString m_initializedSourceDir;
        QString m_initializedDestinationDir;
        bool m_logCopiedFiles = true;
        bool m_nameFilterDisables = false; //!< name filter disables or hides
        bool m_withBootstrapFile = false;
        bool m_hasOtherSwiftVersions = false;

        // caches will be explicitly initialized in initCaches
        BlackMisc::Simulation::Data::CModelCaches m_modelCaches { false, this };
        BlackMisc::Simulation::Data::CModelSetCaches m_modelSetCaches { false, this };

        // caches will be initialized so they can be overriden
        // those caches do not harm if they exists default initialized
        //! \fixme this is a workaround, as it creates files on disk even if those are not copied. It was much nicer if the cache would init themself if the file appears
        BlackMisc::CData<BlackMisc::Network::Data::TLastServer> m_lastServer { this }; //!< recently used server (VATSIM, other)
        BlackMisc::CData<BlackMisc::Simulation::Data::TSimulatorLastSelection> m_modelSetCurrentSimulator { this };
        BlackMisc::CData<BlackMisc::Simulation::Data::TModelCacheLastSelection> m_modelsCurrentSimulator { this };
        BlackMisc::CData<BlackMisc::Simulation::Data::TLastModel> m_lastAircraftModel { this }; //!< recently used aircraft model
        BlackMisc::CData<BlackCore::Data::TLauncherSetup> m_launcherSetup { this };
        BlackMisc::CData<BlackCore::Data::TVatsimSetup> m_vatsimSetup { this };
        BlackMisc::CData<BlackCore::Data::TVatsimLastServer> m_lastVatsimServer { this }; //!< recently used VATSIM server
    };

    /*!
     * Wizard page for CCopyConfigurationComponent
     */
    class CCopyConfigurationWizardPage : public QWizardPage
    {
    public:
        //! Constructors
        using QWizardPage::QWizardPage;

        //! Log.categories
        static const QStringList &getLogCategories();

        //! Set config
        void setConfigComponent(CCopyConfigurationComponent *config) { m_config = config; }

        //! \copydoc QWizardPage::initializePage
        virtual void initializePage() override;

        //! \copydoc QWizardPage::validatePage
        virtual bool validatePage() override;

    private:
        CCopyConfigurationComponent *m_config = nullptr;
    };
} // ns

#endif // guard
