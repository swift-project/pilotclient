/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_SETTINGSNETWORKSERVERSCOMPONENT_H
#define BLACKGUI_SETTINGSNETWORKSERVERSCOMPONENT_H

#include "blackgui/components/enableforruntime.h"
#include <QFrame>
#include <QScopedPointer>

namespace Ui { class CSettingsNetworkServersComponent; }

namespace BlackGui
{
    namespace Components
    {
        //! Settings for network servers
        class CSettingsNetworkServersComponent :
            public QFrame,
            public CEnableForRuntime
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CSettingsNetworkServersComponent(QWidget *parent = nullptr);

            //! Destructor
            ~CSettingsNetworkServersComponent();

        protected:
            //! \copydoc CRuntimeBasedComponent::runtimeHasBeenSet
            virtual void runtimeHasBeenSet() override;

        public slots:
            //! Reload settings
            void reloadSettings();

        private slots:
            //! Network server selected
            void ps_networkServerSelected(QModelIndex index);

            //! Alter traffic server
            void ps_alterTrafficServer();

            //! Changed settings
            void ps_changedSettings(uint typeValue);

        private:
            QScopedPointer<Ui::CSettingsNetworkServersComponent> ui;
        };
    } // ns
} // ns

#endif // guard
