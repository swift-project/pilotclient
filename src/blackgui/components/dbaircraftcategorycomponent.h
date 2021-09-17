/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_DBAIRCRAFTCATEGORYCOMPONENT_H
#define BLACKGUI_COMPONENTS_DBAIRCRAFTCATEGORYCOMPONENT_H

#include "blackgui/overlaymessagesframe.h"
#include "blackgui/enablefordockwidgetinfoarea.h"
#include "blackgui/enableforviewbasedindicator.h"
#include "blackgui/blackguiexport.h"
#include "blackmisc/network/entityflags.h"

#include <QScopedPointer>

namespace Ui { class CDbAircraftCategoryComponent; }
namespace BlackGui::Components
{
    /*!
     * DB aircraft categories
     */
    class BLACKGUI_EXPORT CDbAircraftCategoryComponent :
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
        void onCategoryRead(BlackMisc::Network::CEntityFlags::Entity entity, BlackMisc::Network::CEntityFlags::ReadState readState, int count);

        //! Download progress for an entity
        void onEntityDownloadProgress(BlackMisc::Network::CEntityFlags::Entity entity, int logId, int progress, qint64 current, qint64 max, const QUrl &url);

        //! Reload models
        void onReload();

        QScopedPointer<Ui::CDbAircraftCategoryComponent> ui;
    };
} // ns

#endif // guard
