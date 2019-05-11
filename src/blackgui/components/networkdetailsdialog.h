/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_NETWORKDETAILSDIALOG_H
#define BLACKGUI_COMPONENTS_NETWORKDETAILSDIALOG_H

#include <QDialog>
#include <QScopedPointer>

#include "blackmisc/network/fsdsetup.h"
#include "blackmisc/audio/voicesetup.h"
#include "blackcore/network.h"

namespace Ui { class CNetworkDetailsDialog; }
namespace BlackGui
{
    namespace Components
    {
        //! Network details as dialog
        class CNetworkDetailsDialog : public QDialog
        {
            Q_OBJECT

        public:
            //! Ctor
            explicit CNetworkDetailsDialog(QWidget *parent = nullptr);

            //! Dtor
            virtual ~CNetworkDetailsDialog() override;

            //! FSD setup
            BlackMisc::Network::CFsdSetup getFsdSetup() const;

            //! Voice setup
            BlackMisc::Audio::CVoiceSetup getVoiceSetup() const;

            //! Login mode
            BlackCore::INetwork::LoginMode getLoginMode() const;

        private:
            QScopedPointer<Ui::CNetworkDetailsDialog> ui;
        };
    } // ns
} // ns

#endif // guard
