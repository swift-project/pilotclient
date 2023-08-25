// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKUI_COMPONENTS_DBMODELCOMPONENT_H
#define BLACKUI_COMPONENTS_DBMODELCOMPONENT_H

#include "blackgui/components/dbmappingcomponentaware.h"
#include "blackgui/enablefordockwidgetinfoarea.h"
#include "blackgui/enableforviewbasedindicator.h"
#include "blackgui/overlaymessagesframe.h"
#include "blackgui/blackguiexport.h"
#include "blackmisc/network/entityflags.h"
#include "blackmisc/simulation/aircraftmodel.h"
#include "blackmisc/simulation/aircraftmodellist.h"

#include <QObject>
#include <QScopedPointer>

namespace Ui
{
    class CDbModelComponent;
}
namespace BlackGui::Components
{
    /*!
     * Database models. Those are the models loaaded from the DB.
     */
    class BLACKGUI_EXPORT CDbModelComponent :
        public COverlayMessagesFrame,
        public CDbMappingComponentAware,
        public CEnableForDockWidgetInfoArea,
        public CEnableForViewBasedIndicator
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CDbModelComponent(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CDbModelComponent();

        //! Get latest model if any
        BlackMisc::Simulation::CAircraftModel getLatestModel() const;

        //! Models loaded?
        bool hasModels() const;

        //! Load new data
        void requestUpdatedData();

    signals:
        //! Request to stash the selected models
        void requestStash(const BlackMisc::Simulation::CAircraftModelList &models);

    private:
        //! Models have been read
        void onModelsRead(BlackMisc::Network::CEntityFlags::Entity entity, BlackMisc::Network::CEntityFlags::ReadState readState, int count, const QUrl &url);

        //! Reload models
        void onReload();

        //! Style sheet changed
        void onStyleSheetChanged();

        //! Download progress for an entity
        void onEntityDownloadProgress(BlackMisc::Network::CEntityFlags::Entity entity, int logId, int progress, qint64 current, qint64 max, const QUrl &url);

    private:
        QScopedPointer<Ui::CDbModelComponent> ui;
    };
} // ns

#endif // guard
