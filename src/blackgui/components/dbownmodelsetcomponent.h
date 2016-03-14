/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef BLACKGUI_COMPONENTS_DBOWNMODELSETCOMPONENT_H
#define BLACKGUI_COMPONENTS_DBOWNMODELSETCOMPONENT_H

#include "blackmisc/simulation/aircraftmodellist.h"
#include "dbmappingcomponentaware.h"
#include <QFrame>
#include <QScopedPointer>


namespace Ui { class CDbOwnModelSetComponent; }

namespace BlackGui
{
    namespace Views { class CAircraftModelView; }
    namespace Components
    {
        class CDbOwnModelSetDialog;

        /*!
         * Handling of the own model set
         */
        class CDbOwnModelSetComponent :
            public QFrame,
            public CDbMappingComponentAware
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CDbOwnModelSetComponent(QWidget *parent = nullptr);

            //! Destructor
            virtual ~CDbOwnModelSetComponent();

            //! Corresponding view
            Views::CAircraftModelView *getView() const;

            //! Set the model set
            void setModelSet(const BlackMisc::Simulation::CAircraftModelList &models, const BlackMisc::Simulation::CSimulatorInfo &simulator);

            //! \copydoc CDbMappingComponentAware::setMappingComponent
            virtual void setMappingComponent(CDbMappingComponent *component) override;

        private slots:
            //! Tab has been changed
            void ps_tabChanged(int index);

            //! Button was clicked
            void ps_buttonClicked();

        private:
            QScopedPointer<Ui::CDbOwnModelSetComponent> ui;
            QScopedPointer<CDbOwnModelSetDialog>        m_modelSetDialog;
            BlackMisc::Simulation::CSimulatorInfo       m_simulator;
        };

    } // ns
} // ns

#endif // guard
