/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_DBAIRCRAFTICAOCOMPONENT_H
#define BLACKGUI_COMPONENTS_DBAIRCRAFTICAOCOMPONENT_H

#include "blackgui/enablefordockwidgetinfoarea.h"
#include "blackgui/enableforviewbasedindicator.h"
#include "blackgui/overlaymessagesframe.h"
#include "blackgui/blackguiexport.h"
#include "blackmisc/network/entityflags.h"

#include <QObject>
#include <QScopedPointer>

namespace Ui
{
    class CDbAircraftIcaoComponent;
}
namespace BlackMisc::Aviation
{
    class CAircraftIcaoCode;
}
namespace BlackGui
{
    namespace Views
    {
        class CAircraftIcaoCodeView;
    }
    namespace Components
    {
        /*!
         * DB aircraft ICAO data
         */
        class BLACKGUI_EXPORT CDbAircraftIcaoComponent :
            public COverlayMessagesFrame,
            public CEnableForDockWidgetInfoArea,
            public CEnableForViewBasedIndicator
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CDbAircraftIcaoComponent(QWidget *parent = nullptr);

            //! Destructor
            virtual ~CDbAircraftIcaoComponent() override;

            //! Get the view
            BlackGui::Views::CAircraftIcaoCodeView *view() const;

            //! Filter by ICAO as default
            void filter(const BlackMisc::Aviation::CAircraftIcaoCode &icao);

            //! Select aircraft ICAO object
            bool selectAircraftIcao(const BlackMisc::Aviation::CAircraftIcaoCode &icao);

        private:
            //! ICAO codes have been read
            void onIcaoRead(BlackMisc::Network::CEntityFlags::Entity entity, BlackMisc::Network::CEntityFlags::ReadState readState, int count);

            //! Download progress for an entity
            void onEntityDownloadProgress(BlackMisc::Network::CEntityFlags::Entity entity, int logId, int progress, qint64 current, qint64 max, const QUrl &url);

            //! Reload models
            void onReload();

        private:
            QScopedPointer<Ui::CDbAircraftIcaoComponent> ui;
        };
    } // ns
} // ns

#endif // guard
