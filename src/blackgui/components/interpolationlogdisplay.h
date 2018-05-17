/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_INTERPOLATIONLOGDISPLAY_H
#define BLACKGUI_COMPONENTS_INTERPOLATIONLOGDISPLAY_H

#include "blackgui/blackguiexport.h"
#include "blackcore/simulatorcommon.h"
#include "blackmisc/aviation/callsign.h"
#include <QFrame>
#include <QTimer>
#include <QScopedPointer>
#include <QPointer>

namespace Ui { class CInterpolationLogDisplay; }
namespace BlackGui
{
    namespace Components
    {
        /**
         * Display live data of interpolation
         */
        class BLACKGUI_EXPORT CInterpolationLogDisplay : public QFrame
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CInterpolationLogDisplay(QWidget *parent = nullptr);

            //! Destructor
            virtual ~CInterpolationLogDisplay();

            //! Set simulator
            void setSimulator(BlackCore::CSimulatorCommon *simulatorCommon);

        private:
            //! Update log.
            void updateLog();

            //! Slider interval
            void onSliderChanged(int timeSecs);

            //! Callsign has been changed
            void onCallsignEntered();

            //! Toggle start/stop
            void toggleStartStop();

            //! Start displaying
            void start();

            //! Stop displaying
            void stop();

            QScopedPointer<Ui::CInterpolationLogDisplay> ui;
            QTimer m_updateTimer;
            QPointer<BlackCore::CSimulatorCommon> m_simulatorCommon = nullptr; //!< related simulator
            BlackMisc::Aviation::CCallsign m_callsign; //!< current callsign

            static const QString &startText();
            static const QString &stopText();
        };
    } // ns
} // ns

#endif // guard
