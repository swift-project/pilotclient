/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKUI_COMPONENTS_DBMODELCOMPONENT_H
#define BLACKUI_COMPONENTS_DBMODELCOMPONENT_H

#include "blackgui/blackguiexport.h"
#include "blackgui/enableforviewbasedindicator.h"
#include "blackgui/components/enablefordockwidgetinfoarea.h"
#include "blackgui/components/dbmappingcomponentaware.h"
#include "blackmisc/network/webdataservicesprovider.h"
#include <QFrame>
#include <QScopedPointer>

namespace Ui { class CDbModelComponent; }

namespace BlackGui
{
    namespace Components
    {
        /**
         * Database models from DB
         */
        class BLACKGUI_EXPORT CDbModelComponent :
            public QFrame,
            public CDbMappingComponentAware,
            public CEnableForDockWidgetInfoArea,
            public BlackGui::CEnableForViewBasedIndicator,
            public BlackMisc::Network::CWebDataServicesAware
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CDbModelComponent(QWidget *parent = nullptr);

            //! Destructor
            virtual ~CDbModelComponent();

            //! Set the provider
            virtual void setProvider(BlackMisc::Network::IWebDataServicesProvider *webDataReaderProvider) override;

            //! Get latest model if any
            BlackMisc::Simulation::CAircraftModel getLatestModel() const;

        public slots:
            //! Load new data
            void requestUpdatedData();

        private slots:
            //! Models have been read
            void ps_modelsRead(BlackMisc::Network::CEntityFlags::Entity entity, BlackMisc::Network::CEntityFlags::ReadState readState, int count);

            //! Reload models
            void ps_reload();

            //! Style sheet changed
            void ps_onStyleSheetChanged();

        private:
            QScopedPointer<Ui::CDbModelComponent> ui;
        };
    } // ns
} // ns

#endif // guard
