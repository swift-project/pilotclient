/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_DBAIRCRAFTICAOCOMPONENT_H
#define BLACKGUI_COMPONENTS_DBAIRCRAFTICAOCOMPONENT_H

#include "blackgui/blackguiexport.h"
#include "blackgui/components/enablefordockwidgetinfoarea.h"
#include "blackgui/enableforviewbasedindicator.h"
#include "blackmisc/network/entityflags.h"

#include <QFrame>
#include <QObject>
#include <QScopedPointer>

class QWidget;

namespace BlackMisc { namespace Aviation { class CAircraftIcaoCode; } }
namespace Ui { class CDbAircraftIcaoComponent; }

namespace BlackGui
{
    namespace Components
    {
        /**
         * DB aircraft ICAO data
         */
        class BLACKGUI_EXPORT CDbAircraftIcaoComponent :
            public QFrame,
            public CEnableForDockWidgetInfoArea,
            public CEnableForViewBasedIndicator
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CDbAircraftIcaoComponent(QWidget *parent = nullptr);

            //! Destructor
            ~CDbAircraftIcaoComponent();

        public slots:
            //! Filter by ICAO as default
            void filter(const BlackMisc::Aviation::CAircraftIcaoCode &icao);

        private slots:
            //! ICAO codes have been read
            void ps_icaoRead(BlackMisc::Network::CEntityFlags::Entity entity, BlackMisc::Network::CEntityFlags::ReadState readState, int count);

            //! Reload models
            void ps_reload();

        private:
            QScopedPointer<Ui::CDbAircraftIcaoComponent> ui;
        };
    } // ns
} // ns

#endif // guard
