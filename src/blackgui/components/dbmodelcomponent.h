/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKUI_COMPONENTS_DBMODELCOMPONENT_H
#define BLACKUI_COMPONENTS_DBMODELCOMPONENT_H

#include "blackgui/blackguiexport.h"
#include "blackgui/components/enablefordockwidgetinfoarea.h"
#include "blackmisc/network/webdataservicesprovider.h"
#include <QFrame>
#include <QScopedPointer>

namespace Ui { class CDbModelComponent; }

namespace BlackGui
{
    namespace Components
    {
        /**
         * Database models from DB
         */
        class BLACKGUI_EXPORT CDbModelComponent :
            public QFrame,
            public CEnableForDockWidgetInfoArea,
            public BlackMisc::Network::CWebDataServicesAware
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CDbModelComponent(QWidget *parent = nullptr);

            //! Destructor
            virtual ~CDbModelComponent();

            //! Set the provider
            virtual void setProvider(BlackMisc::Network::IWebDataServicesProvider *webDataReaderProvider) override;

        private slots:
            //! Models have been read
            void ps_modelsRead(BlackMisc::Network::CDbFlags::Entity entity, BlackMisc::Network::CDbFlags::ReadState readState, int count);

            //! Reload models
            void ps_reload();

            //! Style sheet changed
            void ps_onStyleSheetChanged();

        private:
            QScopedPointer<Ui::CDbModelComponent> ui;
        };
    } // ns
} // ns

#endif // guard
