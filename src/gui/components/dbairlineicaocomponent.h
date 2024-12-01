// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_DBAIRLINEICAOCOMPONENT_H
#define SWIFT_GUI_COMPONENTS_DBAIRLINEICAOCOMPONENT_H

#include <QFrame>
#include <QObject>
#include <QScopedPointer>

#include "gui/enablefordockwidgetinfoarea.h"
#include "gui/enableforviewbasedindicator.h"
#include "gui/overlaymessagesframe.h"
#include "gui/swiftguiexport.h"
#include "misc/network/entityflags.h"

namespace Ui
{
    class CDbAirlineIcaoComponent;
}
namespace swift::gui
{
    namespace views
    {
        class CAirlineIcaoCodeView;
    }
    namespace components
    {
        /*!
         * Airline ICAO code
         */
        class SWIFT_GUI_EXPORT CDbAirlineIcaoComponent :
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
            swift::gui::views::CAirlineIcaoCodeView *view() const;

        private:
            //! ICAO codes have been read
            void onIcaoRead(swift::misc::network::CEntityFlags::Entity entity,
                            swift::misc::network::CEntityFlags::ReadState readState, int count, const QUrl &url);

            //! Download progress for an entity
            void onEntityDownloadProgress(swift::misc::network::CEntityFlags::Entity entity, int logId, int progress,
                                          qint64 current, qint64 max, const QUrl &url);

            //! Reload models
            void onReload();

        private:
            QScopedPointer<Ui::CDbAirlineIcaoComponent> ui;
        };
    } // namespace components
} // namespace swift::gui
#endif // SWIFT_GUI_COMPONENTS_DBAIRLINEICAOCOMPONENT_H
