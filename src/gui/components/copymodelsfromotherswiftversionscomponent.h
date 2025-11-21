// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_COPYMODELSFROMOTHERSWIFTVERSIONS_H
#define SWIFT_GUI_COMPONENTS_COPYMODELSFROMOTHERSWIFTVERSIONS_H

#include <QFrame>
#include <QScopedPointer>
#include <QWizardPage>

#include "gui/overlaymessagesframe.h"
#include "misc/applicationinfo.h"
#include "misc/simulation/data/modelcaches.h"

namespace Ui
{
    class CCopyModelsFromOtherSwiftVersionsComponent;
}
namespace swift::gui::components
{
    /*!
     * Copy models from another swift version
     */
    class CCopyModelsFromOtherSwiftVersionsComponent : public COverlayMessagesFrame
    {
        Q_OBJECT

    public:
        //! Ctor
        explicit CCopyModelsFromOtherSwiftVersionsComponent(QWidget *parent = nullptr);

        //! Dtor
        ~CCopyModelsFromOtherSwiftVersionsComponent() override;

        //! Reload other versions
        void reloadOtherVersions(int deferMs = -1);

        //! Load enabled sims
        void load();

    private:
        //! Copy as per UI settings
        void copy();

        //! Read data file
        bool readDataFile(const QString &modelFile, swift::misc::simulation::CAircraftModelList &models,
                          const swift::misc::CApplicationInfo &otherVersion,
                          const swift::misc::simulation::CSimulatorInfo &sim);

        //! Confirm override
        bool confirmOverride(const QString &msg);

        //! Init the simulators which can be copied
        void onVersionChanged(const swift::misc::CApplicationInfo &otherVersion);

        QScopedPointer<Ui::CCopyModelsFromOtherSwiftVersionsComponent> ui;

        // caches will be explicitly initialized in copy
        swift::misc::simulation::data::CModelCaches m_modelCaches { false, this };
        swift::misc::simulation::data::CModelSetCaches m_modelSetCaches { false, this };
        swift::misc::CSetting<swift::core::application::TEnabledSimulators> m_enabledSimulators { this };
    };

    /*!
     * Wizard page for CCopyModelsFromOtherSwiftVersionsComponent
     */
    class CCopyModelsFromOtherSwiftVersionsWizardPage : public QWizardPage
    {
        Q_OBJECT

    public:
        //! Constructors
        using QWizardPage::QWizardPage;

        //! Set config
        void setConfigComponent(CCopyModelsFromOtherSwiftVersionsComponent *config) { m_copyModels = config; }

        //! \copydoc QWizardPage::initializePage
        void initializePage() override;

        //! \copydoc QWizardPage::validatePage
        bool validatePage() override;

    private:
        CCopyModelsFromOtherSwiftVersionsComponent *m_copyModels = nullptr;
    };
} // namespace swift::gui::components

#endif // SWIFT_GUI_COMPONENTS_COPYMODELSFROMOTHERSWIFTVERSIONS_H
