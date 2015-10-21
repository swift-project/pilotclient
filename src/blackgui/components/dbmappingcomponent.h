/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_DBMAPPINGCOMPONENT_H
#define BLACKGUI_COMPONENTS_DBMAPPINGCOMPONENT_H

#include "blackcore/databaseauthentication.h"
#include "blackgui/blackguiexport.h"
#include "blackgui/overlaymessagesframe.h"
#include "blackgui/menudelegate.h"
#include "blackgui/enableforviewbasedindicator.h"
#include "blackgui/components/enablefordockwidgetinfoarea.h"
#include "blackmisc/simulation/aircraftmodelloader.h"
#include "blackmisc/simulation/fscommon/vpilotrulesreader.h"
#include "blackmisc/network/webdataservicesprovider.h"
#include "blackmisc/statusmessagelist.h"
#include <QFrame>
#include <QScopedPointer>

namespace Ui { class CDbMappingComponent; }

namespace BlackGui
{
    namespace Components
    {
        /*!
         * Mapping component
         */
        class BLACKGUI_EXPORT CDbMappingComponent :
            public BlackGui::COverlayMessagesFrame,
            public CEnableForDockWidgetInfoArea,
            public BlackGui::CEnableForViewBasedIndicator,
            public BlackMisc::Network::CWebDataServicesAware
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CDbMappingComponent(QWidget *parent = nullptr);

            //! Destructor
            ~CDbMappingComponent();

            //! \copydoc CWebDataReaderAware::setProvider
            virtual void setProvider(BlackMisc::Network::IWebDataServicesProvider *provider) override;

            //! Graceful shutdown
            void gracefulShutdown();

        signals:
            //! Request to filter by livery
            void filterByLivery(const BlackMisc::Aviation::CLivery &livery);

            //! Request to filter by aircraft ICAO
            void filterByAircraftIcao(const BlackMisc::Aviation::CAircraftIcaoCode &icao);

            //! Request to filter by distributor
            void filterByDistributor(const BlackMisc::Simulation::CDistributor &distributor);

        public slots:
            //! Validate, empty list means OK
            BlackMisc::CStatusMessageList validate(bool withNestedForms) const;

            //! Save
            void save();

            //! Resize so that selection is easy (larger table view)
            void resizeForSelect();

            //! Resize so that mapping is easier
            void resizeForMapping();

        private slots:
            //! Load the vPilot rules
            void ps_loadVPilotData();

            //! Data for vPilot have been loaded
            void ps_onLoadVPilotDataFinished(bool success);

            //! Row count for vPilot data changed
            void ps_onVPilotCountChanged(int count, bool withFilter);

            //! Row has been selected
            void ps_onModelRowSelected(const QModelIndex &index);

            //! Load the models
            void ps_loadInstalledModels(const BlackMisc::Simulation::CSimulatorInfo &simInfo);

            //! Model loading finished
            void ps_onInstalledModelLoadingFinished(bool success);

            //! Own model count changed
            void ps_onOwnModelsCountChanged(int count, bool withFilter);

            //! Request simulator models
            void ps_requestSimulatorModels();

        private:
            QScopedPointer<Ui::CDbMappingComponent> ui;
            BlackMisc::Simulation::FsCommon::CVPilotRulesReader m_vPilotReader;
            std::unique_ptr<BlackMisc::Simulation::IAircraftModelLoader> m_modelLoader;
            BlackCore::CDatabaseAuthenticationService *m_authenticationService = nullptr;
            bool m_withVPilot = false;

            //! Consolidated aircraft model
            BlackMisc::Simulation::CAircraftModel getAircraftModel() const;

            //! Init vPilot loading is suitable
            void initVPilotLoading();

            //! Init model loader
            bool initModelLoader(const BlackMisc::Simulation::CSimulatorInfo &simInfo);

            // -------------------- component specific menus --------------------------

            //! The menu for loading and handling own models for mapping
            //! \note This is specific for that very component
            class CMappingSimulatorModelMenu : public BlackGui::IMenuDelegate
            {
            public:
                //! Constructor
                CMappingSimulatorModelMenu(CDbMappingComponent *mappingComponent) :
                    BlackGui::IMenuDelegate(mappingComponent)
                {}

                //! \copydoc IMenuDelegate::customMenu
                virtual void customMenu(QMenu &menu) const override;
            };

            //! The menu for loading and handling VPilot rules for mapping
            //! \note This is specific for that very component
            class CMappingVPilotMenu : public BlackGui::IMenuDelegate
            {
            public:
                //! Constructor
                CMappingVPilotMenu(CDbMappingComponent *mappingComponent) :
                    BlackGui::IMenuDelegate(mappingComponent)
                {}

                //! \copydoc IMenuDelegate::customMenu
                virtual void customMenu(QMenu &menu) const override;

            private:
                CDbMappingComponent *mappingComponent() const;
            };
        };

    } // ns
} // ns
#endif // guard
