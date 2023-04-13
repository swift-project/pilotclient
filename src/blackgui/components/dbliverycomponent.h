/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_DBLIVERYCOMPONENT_H
#define BLACKGUI_COMPONENTS_DBLIVERYCOMPONENT_H

#include "blackgui/enablefordockwidgetinfoarea.h"
#include "blackgui/enableforviewbasedindicator.h"
#include "blackgui/overlaymessagesframe.h"
#include "blackgui/blackguiexport.h"
#include "blackmisc/aviation/airlineicaocode.h"
#include "blackmisc/network/entityflags.h"

#include <QFrame>
#include <QObject>
#include <QScopedPointer>

namespace Ui
{
    class CDbLiveryComponent;
}
namespace BlackMisc::Aviation
{
    class CLivery;
}
namespace BlackGui
{
    namespace Views
    {
        class CLiveryView;
    }
    namespace Components
    {
        /*!
         * Liveries from DB
         */
        class BLACKGUI_EXPORT CDbLiveryComponent :
            public COverlayMessagesFrame,
            public CEnableForDockWidgetInfoArea,
            public CEnableForViewBasedIndicator
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CDbLiveryComponent(QWidget *parent = nullptr);

            //! Destructor
            virtual ~CDbLiveryComponent();

            //! The livery view
            BlackGui::Views::CLiveryView *view();

            //! Filter by livery
            void filter(const BlackMisc::Aviation::CLivery &livery);

            //! Filter by airline ICAO
            void filterByAirline(const BlackMisc::Aviation::CAirlineIcaoCode &icao);

        private:
            //! Liveries codes have been read
            void onLiveriesRead(BlackMisc::Network::CEntityFlags::Entity entity, BlackMisc::Network::CEntityFlags::ReadState readState, int count);

            //! Download progress for an entity
            void onEntityDownloadProgress(BlackMisc::Network::CEntityFlags::Entity entity, int logId, int progress, qint64 current, qint64 max, const QUrl &url);

            //! Reload models
            void onReload();

        private:
            QScopedPointer<Ui::CDbLiveryComponent> ui;
        };
    } // ns
} // ns

#endif // guard
