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

#include "blackcore/data/authenticateduser.h"
#include "blackcore/data/aircraftmodels.h"
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
            //! Tab index
            //! Must match real tab index
            enum TabIndex
            {
                NoValidTab   = -1,
                TabStash     =  0,
                TabOwnModels =  1,
                TabVPliot    =  2
            };

            //! Constructor
            explicit CDbMappingComponent(QWidget *parent = nullptr);

            //! Destructor
            ~CDbMappingComponent();

            //! \copydoc CWebDataReaderAware::setProvider
            virtual void setProvider(BlackMisc::Network::IWebDataServicesProvider *provider) override;

            //! Graceful shutdown
            void gracefulShutdown();

            //! With vPilot rules
            bool withVPilot() const { return m_withVPilot; }

            //! And models which can be stashed
            bool hasModelsForStash() const;

            //! The model to be stashed
            BlackMisc::Simulation::CAircraftModelList getModelsForStash() const;

            //! Current tab index
            TabIndex currentTabIndex() const;

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
            void saveSingleModelToDb();

            //! Resize so that selection is easy (larger table view)
            void resizeForSelect();

            //! Resize so that mapping is easier
            void resizeForMapping();

            //! Stash models
            void stashSelectedModels();

        private slots:
            //! Load the vPilot rules
            void ps_loadVPilotData();

            //! Data for vPilot have been loaded
            void ps_onLoadVPilotDataFinished(bool success);

            //! Row count for vPilot data changed
            void ps_onVPilotCountChanged(int count, bool withFilter);

            //! Stash count has been changed
            void ps_onStashCountChanged(int count, bool withFilter);

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

            //! User object changed
            void ps_userChanged();

        private:
            QScopedPointer<Ui::CDbMappingComponent> ui;
            BlackMisc::Simulation::FsCommon::CVPilotRulesReader           m_vPilotReader;                //!< read vPilot rules
            BlackCore::CData<BlackCore::Data::VPilotAircraftModels>       m_cachedVPilotModels { this }; //!< cache for latest vPilot rules
            std::unique_ptr<BlackMisc::Simulation::IAircraftModelLoader>  m_modelLoader;                 //!< read own aircraft models
            BlackCore::CData<BlackCore::Data::OwnSimulatorAircraftModels> m_cachedOwnModels { this };    //!< cache for latest models
            BlackCore::CData<BlackCore::Data::AuthenticatedUser>          m_user {this, &CDbMappingComponent::ps_userChanged};
            bool m_vPilot1stInit = true;
            bool m_withVPilot    = false;

            //! Consolidated aircraft model
            BlackMisc::Simulation::CAircraftModel getAircraftModel() const;

            //! Init vPilot if rights and suitable
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
            //! \note This is a specific menu for that very component
            class CMappingVPilotMenu : public BlackGui::IMenuDelegate
            {
            public:
                //! Constructor
                CMappingVPilotMenu(CDbMappingComponent *mappingComponent, bool separatorAtEnd) :
                    BlackGui::IMenuDelegate(mappingComponent, separatorAtEnd)
                {}

                //! \copydoc IMenuDelegate::customMenu
                virtual void customMenu(QMenu &menu) const override;

            private:
                CDbMappingComponent *mappingComponent() const;
            };

            //! The menu for stashing models
            //! \note This is a specific menu for that very component
            class CStashMenu : public BlackGui::IMenuDelegate
            {
            public:
                //! Constructor
                CStashMenu(CDbMappingComponent *mappingComponent, bool separatorAtEnd) :
                    BlackGui::IMenuDelegate(mappingComponent, separatorAtEnd)
                {}

                //! \copydoc IMenuDelegate::customMenu
                virtual void customMenu(QMenu &menu) const override;

                CDbMappingComponent *mappingComponent() const;
            };
        };
    } // ns
} // ns
#endif // guard
