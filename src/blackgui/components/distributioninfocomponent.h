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

#include "blackgui/blackguiexport.h"
#include "blackmisc/db/distributionlist.h"
#include "blackmisc/settingscache.h"
#include <QFrame>

namespace Ui { class CDistributionInfoComponent; }
namespace BlackGui
{
    namespace Components
    {
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
            QString getNewVersionAvailable() const { return m_newVersionAvailable; }

        signals:
            //! Distribution info loaded
            void distributionInfoAvailable(bool success);

        private slots:
            //! Load latest version
            void ps_loadSetup();

            //! Loaded latest version
            void ps_loadedDistributionInfo(bool success);

            //! Channel has been changed
            void ps_channelChanged();

            //! Platform changed
            void ps_platformChanged();

            //! Cache values have been changed
            void ps_changedDistributionCache();

        private:
            QScopedPointer<Ui::CDistributionInfoComponent> ui;
            bool m_distributionsLoaded = false; //!< distribution info loaded
            QString m_newVersionAvailable; //!< new version number if any
            BlackMisc::Db::CDistribution m_currentDistribution; //!< current distribution
            BlackMisc::CDataReadOnly<BlackMisc::Db::TDistributionInfo> m_distributionInfo { this, &CDistributionInfoComponent::ps_changedDistributionCache }; //!< version cache
            BlackMisc::CSetting<BlackMisc::Db::TDistributionSetting> m_distributionSettings { this }; //!< channel/platform selected

            //! Save the current settings
            void saveSettings();
        };
    } // ns
} // ns
#endif // guard
