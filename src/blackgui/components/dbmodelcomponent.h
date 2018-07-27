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
#include "blackgui/components/dbmappingcomponentaware.h"
#include "blackgui/components/enablefordockwidgetinfoarea.h"
#include "blackgui/enableforviewbasedindicator.h"
#include "blackmisc/network/entityflags.h"
#include "blackmisc/simulation/aircraftmodel.h"
#include "blackmisc/simulation/aircraftmodellist.h"

#include <QFrame>
#include <QObject>
#include <QScopedPointer>

class QWidget;

namespace Ui { class CDbModelComponent; }
namespace BlackGui
{
    namespace Components
    {
        /**
         * Database models. Those are the models loaaded from the DB.
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

        public:
            //! Load new data
            void requestUpdatedData();

        private:
            //! Models have been read
            void onModelsRead(BlackMisc::Network::CEntityFlags::Entity entity, BlackMisc::Network::CEntityFlags::ReadState readState, int count);

            //! Reload models
            void onReload();

            //! Style sheet changed
            void onStyleSheetChanged();

        private:
            QScopedPointer<Ui::CDbModelComponent> ui;
        };
    } // ns
} // ns

#endif // guard
