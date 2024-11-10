// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_COMPONENTS_DBLIVERYCOMPONENT_H
#define BLACKGUI_COMPONENTS_DBLIVERYCOMPONENT_H

#include "blackgui/enablefordockwidgetinfoarea.h"
#include "blackgui/enableforviewbasedindicator.h"
#include "blackgui/overlaymessagesframe.h"
#include "blackgui/blackguiexport.h"
#include "misc/aviation/airlineicaocode.h"
#include "misc/network/entityflags.h"

#include <QFrame>
#include <QObject>
#include <QScopedPointer>

namespace Ui
{
    class CDbLiveryComponent;
}
namespace swift::misc::aviation
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
            void filter(const swift::misc::aviation::CLivery &livery);

            //! Filter by airline ICAO
            void filterByAirline(const swift::misc::aviation::CAirlineIcaoCode &icao);

        private:
            //! Liveries codes have been read
            void onLiveriesRead(swift::misc::network::CEntityFlags::Entity entity, swift::misc::network::CEntityFlags::ReadState readState, int count);

            //! Download progress for an entity
            void onEntityDownloadProgress(swift::misc::network::CEntityFlags::Entity entity, int logId, int progress, qint64 current, qint64 max, const QUrl &url);

            //! Reload models
            void onReload();

        private:
            QScopedPointer<Ui::CDbLiveryComponent> ui;
        };
    } // ns
} // ns

#endif // guard
