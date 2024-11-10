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
#include "misc/network/entityflags.h"
#include "misc/simulation/aircraftmodel.h"
#include "misc/simulation/aircraftmodellist.h"

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
        swift::misc::simulation::CAircraftModel getLatestModel() const;

        //! Models loaded?
        bool hasModels() const;

        //! Load new data
        void requestUpdatedData();

    signals:
        //! Request to stash the selected models
        void requestStash(const swift::misc::simulation::CAircraftModelList &models);

    private:
        //! Models have been read
        void onModelsRead(swift::misc::network::CEntityFlags::Entity entity, swift::misc::network::CEntityFlags::ReadState readState, int count, const QUrl &url);

        //! Reload models
        void onReload();

        //! Style sheet changed
        void onStyleSheetChanged();

        //! Download progress for an entity
        void onEntityDownloadProgress(swift::misc::network::CEntityFlags::Entity entity, int logId, int progress, qint64 current, qint64 max, const QUrl &url);

    private:
        QScopedPointer<Ui::CDbModelComponent> ui;
    };
} // ns

#endif // guard
