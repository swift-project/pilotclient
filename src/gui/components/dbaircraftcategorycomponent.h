// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_DBAIRCRAFTCATEGORYCOMPONENT_H
#define SWIFT_GUI_COMPONENTS_DBAIRCRAFTCATEGORYCOMPONENT_H

#include "gui/overlaymessagesframe.h"
#include "gui/enablefordockwidgetinfoarea.h"
#include "gui/enableforviewbasedindicator.h"
#include "gui/swiftguiexport.h"
#include "misc/network/entityflags.h"

#include <QScopedPointer>

namespace Ui
{
    class CDbAircraftCategoryComponent;
}
namespace swift::gui::components
{
    /*!
     * DB aircraft categories
     */
    class SWIFT_GUI_EXPORT CDbAircraftCategoryComponent :
        public COverlayMessagesFrame,
        public CEnableForDockWidgetInfoArea,
        public CEnableForViewBasedIndicator
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CDbAircraftCategoryComponent(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CDbAircraftCategoryComponent();

    private:
        //! ICAO codes have been read
        void onCategoryRead(swift::misc::network::CEntityFlags::Entity entity, swift::misc::network::CEntityFlags::ReadState readState, int count);

        //! Download progress for an entity
        void onEntityDownloadProgress(swift::misc::network::CEntityFlags::Entity entity, int logId, int progress, qint64 current, qint64 max, const QUrl &url);

        //! Reload models
        void onReload();

        QScopedPointer<Ui::CDbAircraftCategoryComponent> ui;
    };
} // ns

#endif // guard
