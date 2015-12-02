/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_DBSTASHCOMPONENT_H
#define BLACKGUI_COMPONENTS_DBSTASHCOMPONENT_H

#include "blackgui/components/enablefordockwidgetinfoarea.h"
#include "blackgui/menudelegate.h"
#include "blackgui/views/aircraftmodelview.h"
#include "blackmisc/network/webdataservicesprovider.h"
#include <QFrame>
#include <QScopedPointer>

namespace Ui { class CDbStashComponent; }

namespace BlackGui
{
    namespace Components
    {
        /*!
         * Stashed objects
         */
        class BLACKGUI_EXPORT CDbStashComponent :
            public QFrame,
            public CEnableForDockWidgetInfoArea,
            public BlackMisc::Network::CWebDataServicesAware
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CDbStashComponent(QWidget *parent = nullptr);

            //! Destructor
            ~CDbStashComponent();

            //! \copydoc CWebDataReaderAware::setProvider
            virtual void setProvider(BlackMisc::Network::IWebDataServicesProvider *provider) override;

            //! Graceful shutdown
            void gracefulShutdown();

            //! Test the given model if it can be stashed
            BlackMisc::CStatusMessage validateStashModel(const BlackMisc::Simulation::CAircraftModel &model) const;

            //! Stash given model
            BlackMisc::CStatusMessage stashModel(const BlackMisc::Simulation::CAircraftModel &model);

            //! Stash given models
            BlackMisc::CStatusMessageList stashModels(const BlackMisc::Simulation::CAircraftModelList &models);

            //! The embedded view
            const BlackGui::Views::CAircraftModelView *getView() const;

        signals:
            //! Unstash
            void unstashed(BlackMisc::Simulation::CAircraftModel &model);

        private slots:
            //! Unstash pressed
            void ps_onUnstashPressed();

            //! Validate pressed
            void ps_onValidatePressed();

        private:
            QScopedPointer<Ui::CDbStashComponent> ui;

            //! Custom menu for the stashed models
            class CStashModelsMenu : public BlackGui::IMenuDelegate
            {
            public:
                //! Constructor
                CStashModelsMenu(CDbStashComponent *parent, bool separatorAtEnd) : IMenuDelegate(parent, separatorAtEnd) {}

                //! \copydoc IMenuDelegate::customMenu
                virtual void customMenu(QMenu &menu) const override;
            };
        };
    } // ns
} // ns

#endif // guard
