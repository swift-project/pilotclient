/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_DBLIVERYCOMPONENT_H
#define BLACKGUI_COMPONENTS_DBLIVERYCOMPONENT_H

#include "blackgui/blackguiexport.h"
#include "blackgui/components/enablefordockwidgetinfoarea.h"
#include "blackgui/enableforviewbasedindicator.h"
#include "blackmisc/network/entityflags.h"
#include "blackmisc/aviation/airlineicaocode.h"

#include <QFrame>
#include <QObject>
#include <QScopedPointer>

class QWidget;

namespace BlackMisc { namespace Aviation { class CLivery; } }
namespace Ui { class CDbLiveryComponent; }
namespace BlackGui
{
    namespace Views { class CLiveryView; }
    namespace Components
    {
        /*!
         * Liveries from DB
         */
        class BLACKGUI_EXPORT CDbLiveryComponent :
            public QFrame,
            public CEnableForDockWidgetInfoArea,
            public BlackGui::CEnableForViewBasedIndicator
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CDbLiveryComponent(QWidget *parent = nullptr);

            //! Destructor
            virtual ~CDbLiveryComponent();

            //! The livery view
            BlackGui::Views::CLiveryView *view();

        public slots:
            //! Filter by livery
            void filter(const BlackMisc::Aviation::CLivery &livery);

            //! Filter by airline ICAO
            void filterByAirline(const BlackMisc::Aviation::CAirlineIcaoCode &icao);

        private:
            //! Liveries codes have been read
            void onLiveriesRead(BlackMisc::Network::CEntityFlags::Entity entity, BlackMisc::Network::CEntityFlags::ReadState readState, int count);

            //! Reload models
            void onReload();

        private:
            QScopedPointer<Ui::CDbLiveryComponent> ui;
        };
    } // ns
} // ns

#endif // guard
