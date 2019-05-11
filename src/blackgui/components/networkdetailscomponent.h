/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_NETWORKDETAILSCOMPONENT_H
#define BLACKGUI_COMPONENTS_NETWORKDETAILSCOMPONENT_H

#include <QFrame>
#include <QScopedPointer>

#include "blackmisc/network/fsdsetup.h"
#include "blackmisc/audio/voicesetup.h"
#include "blackcore/network.h"

namespace Ui { class CNetworkDetailsComponent; }
namespace BlackGui
{
    namespace Components
    {
        //! FSD details
        class CNetworkDetailsComponent : public QFrame
        {
            Q_OBJECT

        public:
            //! Ctor
            explicit CNetworkDetailsComponent(QWidget *parent = nullptr);

            //! Dtor
            virtual ~CNetworkDetailsComponent() override;

            //! FSD setup
            BlackMisc::Network::CFsdSetup getFsdSetup() const;

            //! Voice setup
            BlackMisc::Audio::CVoiceSetup getVoiceSetup() const;

            //! Login mode
            BlackCore::INetwork::LoginMode getLoginMode() const;

        private:
            QScopedPointer<Ui::CNetworkDetailsComponent> ui;
        };
    } // ns
} // ns

#endif // guard
