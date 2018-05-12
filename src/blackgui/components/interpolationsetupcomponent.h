/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_INTERPOLATIONSETUPCOMPONENT_H
#define BLACKGUI_COMPONENTS_INTERPOLATIONSETUPCOMPONENT_H

#include "blackmisc/simulation/interpolationsetuplist.h"
#include "blackgui/overlaymessagesframe.h"
#include <QFrame>
#include <QScopedPointer>
#include <QModelIndex>

namespace Ui { class CInterpolationSetupComponent; }
namespace BlackGui
{
    namespace Components
    {
        //! Setup component
        class CInterpolationSetupComponent : public COverlayMessagesFrame
        {
            Q_OBJECT

        public:
            //! Mode of setup
            enum Mode
            {
                Global,
                PerCallsign
            };

            //! Constructor
            explicit CInterpolationSetupComponent(QWidget *parent = nullptr);

            //! Destructor
            virtual ~CInterpolationSetupComponent();

            //! Setup mode
            Mode getSetupMode() const;

        signals:
            //! Request restrictions UI
            void requestRenderingRestrictionsWidget();

        private:
            QScopedPointer<Ui::CInterpolationSetupComponent> ui;

            //! Double clicked in view
            void onRowDoubleClicked(const QModelIndex &index);

            //! Mode has been changed
            void onModeChanged();

            //! View has been changed
            void onModelChanged();

            //! Reload
            void reloadSetup();

            //! Save a setup
            void saveSetup();

            //! Delete a setup
            void removeSetup();

            //! Values from global setup
            void setUiValuesFromGlobal();

            //! Values per callsign
            void displaySetupsPerCallsign();

            //! Check prerequisites before saving etc.
            bool checkPrerequisites(bool showOverlay);

            //! Send to context
            bool setSetupsToContext(const BlackMisc::Simulation::CInterpolationSetupList &setups);

            //! Setup chaged
            void onSetupChanged();

            BlackMisc::Simulation::CInterpolationSetupList m_lastSetSetups; //!< last setups set to context
        };
    } // ns
}// ns

#endif // guard
