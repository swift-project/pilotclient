/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_DISTRIBUTIONINFOCOMPONENT_H
#define BLACKGUI_COMPONENTS_DISTRIBUTIONINFOCOMPONENT_H

#include "blackcore/application/distributionsettings.h"
#include "blackgui/blackguiexport.h"
#include "blackmisc/db/distributionlist.h"
#include "blackmisc/settingscache.h"
#include "blackmisc/digestsignal.h"
#include <QFrame>

namespace Ui { class CDistributionInfoComponent; }
namespace BlackGui
{
    namespace Components
    {
        class CInstallXSwiftBusDialog;

        /**
         * Update info (distributions etc.)
         */
        class BLACKGUI_EXPORT CDistributionInfoComponent : public QFrame
        {
            Q_OBJECT

        public:
            //! Ctor
            explicit CDistributionInfoComponent(QWidget *parent = nullptr);

            //! Dtor
            virtual ~CDistributionInfoComponent();

            //! Is there a new version available return version, else empty string
            QString getNewAvailableVersionForSelection() const { return m_newVersionAvailable; }

            //! Is there a new version available?
            bool isNewVersionAvailable() const;

            //! Current distribution
            BlackMisc::Db::CDistribution getCurrentDistribution() { return m_currentDistribution; }

        signals:
            //! Distribution info loaded
            void distributionInfoAvailable(bool success);

            //! New platfrom or channel
            void selectionChanged();

        private:
            QScopedPointer<Ui::CDistributionInfoComponent> ui;
            QScopedPointer<CInstallXSwiftBusDialog> m_installXSwiftBusDialog; //!< dialog, install XSwiftXBus
            QString m_newVersionAvailable; //!< new version number if any
            BlackMisc::Db::CDistribution m_currentDistribution; //!< current distribution
            BlackMisc::CDataReadOnly<BlackMisc::Db::TDistributionsInfo> m_distributionsInfo { this, &CDistributionInfoComponent::changedDistributionCache }; //!< version cache
            BlackMisc::CSetting<BlackCore::Application::TDistribution> m_distributionSetting { this }; //!< channel/platform selected
            BlackMisc::CDigestSignal m_dsDistributionAvailable { this, &CDistributionInfoComponent::triggerInfoAvailableSignal, 10000, 2 };

            //! Load latest version
            void requestLoadOfSetup();

            //! Loaded latest version
            void loadedDistributionInfo(bool success);

            //! Channel has been changed
            void channelChanged();

            //! Platform changed
            void platformChanged();

            //! Cache values have been changed
            void changedDistributionCache();

            //! Install XSwiftBus dialog
            void installXSwiftBusDialog();

            //! Save the current settings
            void saveSettings();

            //! Trigger the CDistributionInfoComponent::distributionInfoAvailable signal
            void triggerInfoAvailableSignal();

            //! Selected platform from UI or guessed platform
            QString getSelectedOrGuessedPlatform() const;
        };
    } // ns
} // ns
#endif // guard
