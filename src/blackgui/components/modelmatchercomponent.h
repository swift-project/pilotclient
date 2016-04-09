/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENT_MODELMATCHERCOMPONENT_H
#define BLACKGUI_COMPONENT_MODELMATCHERCOMPONENT_H

#include "blackgui/blackguiexport.h"
#include "blackcore/aircraftmatcher.h"
#include "blackmisc/simulation/aircraftmodelsetloader.h"
#include "blackmisc/simulation/simulatedaircraft.h"
#include "blackmisc/network/entityflags.h"

#include <QFrame>
#include <QScopedPointer>

namespace Ui { class CModelMatcherComponent; }

namespace BlackGui
{
    namespace Components
    {
        /*!
         * Model matcher testing and configuration
         */
        class BLACKGUI_EXPORT CModelMatcherComponent : public QFrame
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CModelMatcherComponent(QWidget *parent = nullptr);

            //! Destructor
            ~CModelMatcherComponent();

        public slots:
            //! Tab where this is embedded has been changed
            void tabIndexChanged(int index);

        private slots:
            //! Simulator switched
            void ps_simulatorChanged(const BlackMisc::Simulation::CSimulatorInfo &simulator);

            //! Cache changed
            void ps_cacheChanged(BlackMisc::Simulation::CSimulatorInfo &simulator);

            //! Run the matcher
            void ps_test();

            //! Web data have been read
            void ps_webDataRed(BlackMisc::Network::CEntityFlags::Entity entity, BlackMisc::Network::CEntityFlags::ReadState state, int number);

        private:
            //! Init
            void redisplay();

            //! Pseudo aircraft created from entries
            BlackMisc::Simulation::CSimulatedAircraft createAircraft() const;

            //! Pseudo default aircraft
            BlackMisc::Simulation::CAircraftModel defaultModel() const;

            QScopedPointer<Ui::CModelMatcherComponent>     ui;
            BlackMisc::Simulation::CAircraftModelSetLoader m_modelSetLoader { BlackMisc::Simulation::CSimulatorInfo(BlackMisc::Simulation::CSimulatorInfo::FSX), this };
            BlackCore::CAircraftMatcher                    m_matcher { BlackCore::CAircraftMatcher::All, this };
        };
    } // ns
} // ns

#endif // guard
