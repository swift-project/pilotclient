// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_COMPONENTS_SETTINGSSIMULATORBASICSCOMPONENT_H
#define BLACKGUI_COMPONENTS_SETTINGSSIMULATORBASICSCOMPONENT_H

#include "blackgui/overlaymessagesframe.h"
#include "misc/simulation/settings/simulatorsettings.h"
#include "misc/simulation/fscommon/fscommonutil.h"
#include "misc/logcategories.h"

#include <QFrame>
#include <QFileDialog>

namespace Ui
{
    class CSettingsSimulatorBasicsComponent;
}
namespace BlackGui::Components
{
    /*!
     * Driver independent parts of simulator settings, ie those one are also used independent of the driver.
     * Example: model paths used in loaders
     */
    class CSettingsSimulatorBasicsComponent : public COverlayMessagesFrame
    {
        Q_OBJECT

    public:
        //! Log categories
        static const QStringList &getLogCategories();

        //! Constructor
        explicit CSettingsSimulatorBasicsComponent(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CSettingsSimulatorBasicsComponent() override;

        //! Show / hide selector
        void hideSelector(bool show);

        //! Any values filled in
        bool hasAnyValues() const;

        //! Simulator
        void setSimulator(const swift::misc::simulation::CSimulatorInfo &simulator);

        //! Save data
        void save();

        //! Has unsaved changes
        bool hasUnsavedChanges() const { return m_unsavedChanges; }

        //! Reset unsaved changes
        void resetUnsavedChanges() { m_unsavedChanges = false; }

        //! Settings for given simulator
        swift::misc::simulation::settings::CSpecializedSimulatorSettings getSimulatorSettings(const swift::misc::simulation::CSimulatorInfo &simulator) const { return m_settings.getSpecializedSettings(simulator); }

    private:
        bool m_unsavedChanges = false;
        QScopedPointer<Ui::CSettingsSimulatorBasicsComponent> ui;
        swift::misc::simulation::settings::CMultiSimulatorSettings m_settings { this };
        Qt::CaseSensitivity m_fileCaseSensitivity = swift::misc::CFileUtils::osFileNameCaseSensitivity();

        //! Model file dialog
        void modelFileDialog();

        //! Exclude file dialog
        void excludeFileDialog();

        //! Simulator file dialog
        void simulatorFileDialog();

        //! Simulator directory entered
        void simulatorDirectoryEntered();

        //! Copy the default values
        void copyDefaults();

        //! Adjust model directory
        void adjustModelDirectory();

        //! Reset values
        void reset();

        //! Simulator has been changed
        void onSimulatorChanged();

        //! Simulator settings did change
        void onSimulatorSettingsChanged(const swift::misc::simulation::CSimulatorInfo &simulator);

        //! Optimize for small layout
        void setSmallLayout(bool small);

        //! Model directories from line edit
        QStringList parseModelDirectories() const;

        //! Exclude directories from line edit
        QStringList parseExcludeDirectories() const;

        //! Exclude directories
        QStringList parseDirectories(const QString &rawString) const;

        //! Add a directory
        QStringList addDirectory(const QString &directory, const QStringList &existingDirs);

        //! Remove a directory
        QStringList removeDirectory(const QString &directory, const QStringList &existingDirs);

        //! Remove directories
        QStringList removeDirectories(const QStringList &directories, const QStringList &existingDirs);

        //! Display the directories
        void displayExcludeDirectoryPatterns(const QStringList &dirs);

        //! Display the directories
        void displayModelDirectories(const QStringList &dirs);

        //! Current settings
        swift::misc::simulation::settings::CSpecializedSimulatorSettings getSettings(const swift::misc::simulation::CSimulatorInfo &simulator) const;

        //! Display simulator`s settings
        void displaySettings(const swift::misc::simulation::CSimulatorInfo &simulator);

        //! Default values for simulator displayed as placeholder text
        void displayDefaultValuesAsPlaceholder(const swift::misc::simulation::CSimulatorInfo &simulator);

        //! Model directory either from input or settings or default
        QString getFileBrowserModelDirectory() const;

        //! Simulator directory either from input or settings or default
        QString getFileBrowserSimulatorDirectory() const;
    };
} // ns

#endif // guard
