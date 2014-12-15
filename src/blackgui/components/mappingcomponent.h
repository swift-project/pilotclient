/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_MAPPINGCOMPONENT_H
#define BLACKGUI_MAPPINGCOMPONENT_H

#include "blackgui/components/enableforruntime.h"
#include "blackgui/components/enablefordockwidgetinfoarea.h"
#include "blackmisc/nwaircraftmodel.h"
#include <QTabWidget>
#include <QScopedPointer>

namespace Ui { class CMappingComponent; }

namespace BlackGui
{
    namespace Components
    {
        //! Mappings, models etc.
        class CMappingComponent :
            public QTabWidget,
            public CEnableForDockWidgetInfoArea,
            public CEnableForRuntime
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CMappingComponent(QWidget *parent = nullptr);

            //! Destructor
            ~CMappingComponent();

            //! Number of current mappings
            int countCurrentMappings() const;

            //! Numer of models
            int countAircraftModels() const;

        protected:
            //! \copydoc CRuntimeBasedComponent::runtimeHasBeenSet
            void runtimeHasBeenSet() override;

        private slots:
            //! Aircraft models available
            void ps_aircraftModelsLoaded();

            //! Mappings changed
            void ps_onMappingsChanged();

            //! Model matched
            void ps_modelMatched(const BlackMisc::Network::CAircraftModel &model);

        private:
            QScopedPointer<Ui::CMappingComponent> ui;

            //! Changed count
            void ps_countChanged(int count);
        };

    } // namespace
} // namespace

#endif // guard
