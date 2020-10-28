/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_FIRSTMODELSETCOMPONENT_H
#define BLACKGUI_COMPONENTS_FIRSTMODELSETCOMPONENT_H

#include "blackgui/blackguiexport.h"
#include "blackgui/overlaymessagesframe.h"
#include "blackcore/application/applicationsettings.h"
#include "blackmisc/simulation/settings/simulatorsettings.h"
#include "blackmisc/simulation/aircraftmodelloader.h"
#include "blackmisc/simulation/simulatorinfo.h"
#include "blackmisc/logcategorylist.h"
#include <QFrame>
#include <QWizardPage>

namespace Ui { class CFirstModelSetComponent; }
namespace BlackGui
{
    namespace Components
    {
        class CDbOwnModelsDialog;
        class CDbOwnModelsComponent;
        class CDbOwnModelSetDialog;
        class CDbOwnModelSetComponent;

        //! Create a first model set
        class CFirstModelSetComponent : public COverlayMessagesFrame
        {
            Q_OBJECT

        public:
            //! Log categories
            static const QStringList &getLogCategories();

            //! Constructor
            explicit CFirstModelSetComponent(QWidget *parent = nullptr);

            //! Destructor
            virtual ~CFirstModelSetComponent();

        private:
            QScopedPointer<Ui::CFirstModelSetComponent> ui;
            QScopedPointer<CDbOwnModelsDialog>   m_modelsDialog;
            QScopedPointer<CDbOwnModelSetDialog> m_modelSetDialog;
            BlackMisc::Simulation::Settings::CMultiSimulatorSettings m_simulatorSettings { this };

            //! Simulator has been changed
            void onSimulatorChanged(const BlackMisc::Simulation::CSimulatorInfo &simulator);

            //! Simulator settings changed
            void onSettingsChanged(const BlackMisc::Simulation::CSimulatorInfo &simulator);

            //! Models have been loaded
            void onModelsLoaded(const BlackMisc::Simulation::CSimulatorInfo &simulator, int count);

            //! Asynchronously call onSettingsChanged
            void triggerSettingsChanged(const BlackMisc::Simulation::CSimulatorInfo &simulator);

            //! Direct access to component
            const CDbOwnModelsComponent *modelsComponent() const;

            //! Direct access to component
            const CDbOwnModelSetComponent *modelSetComponent() const;

            //! Direct access to component's loader
            BlackMisc::Simulation::IAircraftModelLoader *modelLoader() const;

            //! Open own models dialog
            void openOwnModelsDialog();

            //! Own model set dialog
            void openOwnModelSetDialog();

            //! Change model directory
            void changeModelDirectory();

            //! Create the model set
            void createModelSet();

            //! Main window or this
            QWidget *mainWindow();
        };

        //! Wizard page for CFirstModelSetComponent
        class CFirstModelSetWizardPage : public QWizardPage
        {
            Q_OBJECT

        public:
            //! Constructors
            using QWizardPage::QWizardPage;

            //! Set config
            void setFirstModelSet(CFirstModelSetComponent *firstModelSet) { m_firstModelSet = firstModelSet; }

            //! \copydoc QWizardPage::validatePage
            virtual bool validatePage() override;

        private:
            CFirstModelSetComponent *m_firstModelSet = nullptr;
        };
    } // ns
} // ns
#endif // guard
