/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_DBLIVERYCOMPONENT_H
#define BLACKGUI_COMPONENTS_DBLIVERYCOMPONENT_H

#include "blackgui/blackguiexport.h"
#include "blackgui/enableforviewbasedindicator.h"
#include "blackgui/components/enablefordockwidgetinfoarea.h"
#include "blackmisc/network/webdataservicesprovider.h"
#include <QFrame>
#include <QScopedPointer>

namespace Ui { class CDbLiveryComponent; }

namespace BlackGui
{
    namespace Components
    {
        /*!
         * Liveries from DB
         */
        class BLACKGUI_EXPORT CDbLiveryComponent :
            public QFrame,
            public CEnableForDockWidgetInfoArea,
            public BlackGui::CEnableForViewBasedIndicator,
            public BlackMisc::Network::CWebDataServicesAware
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CDbLiveryComponent(QWidget *parent = nullptr);

            //! Destructor
            virtual ~CDbLiveryComponent();

            //! Set the provider
            virtual void setProvider(BlackMisc::Network::IWebDataServicesProvider *webDataReaderProvider) override;

        public slots:
            //! Filter by livery as default
            void filter(const BlackMisc::Aviation::CLivery &livery);

        private slots:
            //! Liveries codes have been read
            void ps_liveriesRead(BlackMisc::Network::CEntityFlags::Entity entity, BlackMisc::Network::CEntityFlags::ReadState readState, int count);

            //! Reload models
            void ps_reload();

        private:
            QScopedPointer<Ui::CDbLiveryComponent> ui;
        };
    } // ns
} // ns

#endif // guard
