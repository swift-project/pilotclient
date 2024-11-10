// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_DBAIRCRAFTICAOCOMPONENT_H
#define SWIFT_GUI_COMPONENTS_DBAIRCRAFTICAOCOMPONENT_H

#include "gui/enablefordockwidgetinfoarea.h"
#include "gui/enableforviewbasedindicator.h"
#include "gui/overlaymessagesframe.h"
#include "gui/swiftguiexport.h"
#include "misc/network/entityflags.h"

#include <QObject>
#include <QScopedPointer>

namespace Ui
{
    class CDbAircraftIcaoComponent;
}
namespace swift::misc::aviation
{
    class CAircraftIcaoCode;
}
namespace swift::gui
{
    namespace views
    {
        class CAircraftIcaoCodeView;
    }
    namespace components
    {
        /*!
         * DB aircraft ICAO data
         */
        class SWIFT_GUI_EXPORT CDbAircraftIcaoComponent :
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
            swift::gui::views::CAircraftIcaoCodeView *view() const;

            //! Filter by ICAO as default
            void filter(const swift::misc::aviation::CAircraftIcaoCode &icao);

            //! Select aircraft ICAO object
            bool selectAircraftIcao(const swift::misc::aviation::CAircraftIcaoCode &icao);

        private:
            //! ICAO codes have been read
            void onIcaoRead(swift::misc::network::CEntityFlags::Entity entity, swift::misc::network::CEntityFlags::ReadState readState, int count);

            //! Download progress for an entity
            void onEntityDownloadProgress(swift::misc::network::CEntityFlags::Entity entity, int logId, int progress, qint64 current, qint64 max, const QUrl &url);

            //! Reload models
            void onReload();

        private:
            QScopedPointer<Ui::CDbAircraftIcaoComponent> ui;
        };
    } // ns
} // ns

#endif // guard
