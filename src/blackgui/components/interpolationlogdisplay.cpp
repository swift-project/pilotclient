/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "interpolationlogdisplay.h"
#include "ui_interpolationlogdisplay.h"
#include "blackmisc/simulation/interpolationlogger.h"

using namespace BlackCore;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Simulation;

namespace BlackGui
{
    namespace Components
    {
        CInterpolationLogDisplay::CInterpolationLogDisplay(QWidget *parent) :
            QFrame(parent),
            ui(new Ui::CInterpolationLogDisplay)
        {
            ui->setupUi(this);
            constexpr int timeSecs = 5;
            ui->hs_UpdateTime->setValue(timeSecs);
            this->onSliderChanged(timeSecs);
            connect(&m_updateTimer, &QTimer::timeout, this, &CInterpolationLogDisplay::updateLog);
            connect(ui->hs_UpdateTime, &QSlider::valueChanged, this, &CInterpolationLogDisplay::onSliderChanged);
            connect(ui->pb_StartStop, &QPushButton::released, this, &CInterpolationLogDisplay::toggleStartStop);
            connect(ui->comp_CallsignCompleter, &CCallsignCompleter::validCallsignEntered, this, &CInterpolationLogDisplay::onCallsignEntered);

            m_callsign = ui->comp_CallsignCompleter->getCallsign();
        }

        CInterpolationLogDisplay::~CInterpolationLogDisplay()
        { }

        void CInterpolationLogDisplay::setSimulator(CSimulatorCommon *simulatorCommon)
        {
            m_simulatorCommon = simulatorCommon;
        }

        void CInterpolationLogDisplay::updateLog()
        {
            const bool hasLogger = m_simulatorCommon;
            if (hasLogger && !m_callsign.isEmpty())
            {
                const QString log = m_simulatorCommon->latestLoggedDataFormatted(m_callsign);
                ui->te_Log->setText(log);
            }
            else
            {
                ui->te_Log->setText("No logger attached or no callsign");
                this->stop();
            }
        }

        void CInterpolationLogDisplay::onSliderChanged(int timeSecs)
        {
            static const QString time("%1 secs");
            m_updateTimer.setInterval(timeSecs * 1000);
            ui->le_UpdateTime->setText(time.arg(timeSecs));
        }

        void CInterpolationLogDisplay::onCallsignEntered()
        {
            const CCallsign cs = ui->comp_CallsignCompleter->getCallsign();
            if (!m_simulatorCommon)
            {
                this->stop();
                return;
            }

            // clear last callsign
            if (!m_callsign.isEmpty())
            {
                m_simulatorCommon->setLogInterpolation(false, m_callsign); // stop logging "old" callsign
                m_callsign = CCallsign(); // clear callsign
            }

            // set new callsign or stop
            if (cs.isEmpty())
            {
                this->stop();
                return;
            }

            m_callsign = cs;
            m_simulatorCommon->setLogInterpolation(true, cs);
        }

        void CInterpolationLogDisplay::toggleStartStop()
        {
            const bool running = m_updateTimer.isActive();
            if (running)
            {
                this->stop();
            }
            else
            {
                this->start();
            }
        }

        void CInterpolationLogDisplay::start()
        {
            const int interval = ui->hs_UpdateTime->value() * 1000;
            m_updateTimer.start(interval);
            ui->pb_StartStop->setText(stopText());
        }

        void CInterpolationLogDisplay::stop()
        {
            m_updateTimer.stop();
            ui->pb_StartStop->setText(startText());
        }

        const QString &CInterpolationLogDisplay::startText()
        {
            static const QString start("start");
            return start;
        }

        const QString &CInterpolationLogDisplay::stopText()
        {
            static const QString stop("stop");
            return stop;
        }
    } // ns
} // ns
