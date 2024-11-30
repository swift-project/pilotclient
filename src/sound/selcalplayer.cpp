// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "selcalplayer.h"

#include <QTimer>

using namespace swift::misc;
using namespace swift::misc::audio;
using namespace swift::misc::aviation;
using namespace swift::misc::physical_quantities;

namespace swift::sound
{
    CSelcalPlayer::CSelcalPlayer(const CAudioDeviceInfo &device, QObject *parent)
        : QObject(parent), m_threadedPlayer(new CThreadedTonePairPlayer(this, "CSelcalPlayer", device))
    {
        m_threadedPlayer->start();
    }

    CSelcalPlayer::~CSelcalPlayer() { this->gracefulShutdown(); }

    void CSelcalPlayer::gracefulShutdown() { m_threadedPlayer->quitAndWait(); }

    std::chrono::milliseconds CSelcalPlayer::play(int volume, const CSelcal &selcal)
    {
        std::chrono::milliseconds duration {};
        if (selcal.isValid())
        {
            const QList<CFrequency> frequencies = selcal.getFrequencies();
            Q_ASSERT(frequencies.size() == 4);
            const std::chrono::milliseconds oneSec(1000);
            const CTonePair t1(frequencies.at(0), frequencies.at(1), oneSec);
            const CTonePair t2({}, {}, oneSec / 5);
            const CTonePair t3(frequencies.at(2), frequencies.at(3), oneSec);
            QList<CTonePair> tonePairs;
            tonePairs << t1 << t2 << t3;
            m_threadedPlayer->play(volume, tonePairs);
            duration = std::chrono::milliseconds(2500);
        }
        return duration;
    }
} // namespace swift::sound
