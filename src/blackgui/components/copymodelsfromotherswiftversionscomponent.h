/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_COPYMODELSFROMOTHERSWIFTVERSIONS_H
#define BLACKGUI_COMPONENTS_COPYMODELSFROMOTHERSWIFTVERSIONS_H

#include "blackgui/overlaymessagesframe.h"
#include "blackmisc/simulation/data/modelcaches.h"
#include "blackmisc/applicationinfo.h"
#include <QFrame>
#include <QWizardPage>
#include <QScopedPointer>

namespace Ui { class CCopyModelsFromOtherSwiftVersionsComponent; }
namespace BlackGui::Components
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
        virtual ~CCopyModelsFromOtherSwiftVersionsComponent();

        //! Reload other versions
        void reloadOtherVersions(int deferMs = -1);

    private:
        //! Copy as per UI settings
        void copy();

        //! Read data file
        bool readDataFile(const QString &modelFile, BlackMisc::Simulation::CAircraftModelList &models, const BlackMisc::CApplicationInfo &otherVersion, const BlackMisc::Simulation::CSimulatorInfo &sim);

        //! Confirm override
        bool confirmOverride(const QString &msg);

        //! Init the simulators which can be copied
        void onVersionChanged(const BlackMisc::CApplicationInfo &otherVersion);

        QScopedPointer<Ui::CCopyModelsFromOtherSwiftVersionsComponent> ui;

        // caches will be explicitly initialized in copy
        BlackMisc::Simulation::Data::CModelCaches    m_modelCaches    { false, this };
        BlackMisc::Simulation::Data::CModelSetCaches m_modelSetCaches { false, this };
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
        virtual void initializePage() override;

        //! \copydoc QWizardPage::validatePage
        virtual bool validatePage() override;

    private:
        CCopyModelsFromOtherSwiftVersionsComponent *m_copyModels = nullptr;
    };
} // ns

#endif // guard
