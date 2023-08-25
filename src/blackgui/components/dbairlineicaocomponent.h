// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_COMPONENTS_DBAIRLINEICAOCOMPONENT_H
#define BLACKGUI_COMPONENTS_DBAIRLINEICAOCOMPONENT_H

#include "blackgui/enablefordockwidgetinfoarea.h"
#include "blackgui/enableforviewbasedindicator.h"
#include "blackgui/overlaymessagesframe.h"
#include "blackgui/blackguiexport.h"
#include "blackmisc/network/entityflags.h"

#include <QFrame>
#include <QObject>
#include <QScopedPointer>

namespace Ui
{
    class CDbAirlineIcaoComponent;
}
namespace BlackGui
{
    namespace Views
    {
        class CAirlineIcaoCodeView;
    }
    namespace Components
    {
        /*!
         * Airline ICAO code
         */
        class BLACKGUI_EXPORT CDbAirlineIcaoComponent :
            public COverlayMessagesFrame,
            public CEnableForDockWidgetInfoArea,
            public CEnableForViewBasedIndicator
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CDbAirlineIcaoComponent(QWidget *parent = nullptr);

            //! Destructor
            virtual ~CDbAirlineIcaoComponent();

            //! Get the view
            BlackGui::Views::CAirlineIcaoCodeView *view() const;

        private:
            //! ICAO codes have been read
            void onIcaoRead(BlackMisc::Network::CEntityFlags::Entity entity, BlackMisc::Network::CEntityFlags::ReadState readState, int count, const QUrl &url);

            //! Download progress for an entity
            void onEntityDownloadProgress(BlackMisc::Network::CEntityFlags::Entity entity, int logId, int progress, qint64 current, qint64 max, const QUrl &url);

            //! Reload models
            void onReload();

        private:
            QScopedPointer<Ui::CDbAirlineIcaoComponent> ui;
        };
    } // ns
} // ns
#endif // guard
