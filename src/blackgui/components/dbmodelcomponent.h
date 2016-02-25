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
#include "blackgui/menudelegate.h"
#include "blackmisc/network/entityflags.h"
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
            public BlackGui::CEnableForViewBasedIndicator
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CDbModelComponent(QWidget *parent = nullptr);

            //! Destructor
            virtual ~CDbModelComponent();

            //! Get latest model if any
            BlackMisc::Simulation::CAircraftModel getLatestModel() const;

            //! Models loaded?
            bool hasModels() const;

        signals:
            //! Request to stash the selected models
            void requestStash(const BlackMisc::Simulation::CAircraftModelList &models);

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

            //! Stash the selected models
            void ps_stashSelectedModels();

        private:
            QScopedPointer<Ui::CDbModelComponent> ui;

            //! The menu for stashing models
            //! \note This is a specific menu for that very component
            class CStashMenu : public BlackGui::IMenuDelegate
            {
            public:
                //! Constructor
                CStashMenu(CDbModelComponent *modelComponent, bool separatorAtEnd) :
                    BlackGui::IMenuDelegate(modelComponent, separatorAtEnd)
                {}

                //! \copydoc IMenuDelegate::customMenu
                virtual void customMenu(QMenu &menu) const override;

            private:
                //! Mapping component
                CDbModelComponent *modelComponent() const;
            };
        };
    } // ns
} // ns

#endif // guard
