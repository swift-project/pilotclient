// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "selcalplayer.h"
#include <QTimer>

using namespace BlackMisc;
using namespace BlackMisc::Audio;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::PhysicalQuantities;

namespace swift::sound
{
    CSelcalPlayer::CSelcalPlayer(const CAudioDeviceInfo &device, QObject *parent)
        : QObject(parent),
          m_threadedPlayer(new CThreadedTonePairPlayer(this, "CSelcalPlayer", device))
    {
        m_threadedPlayer->start();
    }

    CSelcalPlayer::~CSelcalPlayer()
    {
        this->gracefulShutdown();
    }

    void CSelcalPlayer::gracefulShutdown()
    {
        m_threadedPlayer->quitAndWait();
    }

    CTime CSelcalPlayer::play(int volume, const CSelcal &selcal)
    {
        CTime duration = CTime::null();
        if (selcal.isValid())
        {
            const QList<CFrequency> frequencies = selcal.getFrequencies();
            Q_ASSERT(frequencies.size() == 4);
            const CTime oneSec(1000.0, CTimeUnit::ms());
            const CTonePair t1(frequencies.at(0), frequencies.at(1), oneSec);
            const CTonePair t2({}, {}, oneSec / 5.0);
            const CTonePair t3(frequencies.at(2), frequencies.at(3), oneSec);
            QList<CTonePair> tonePairs;
            tonePairs << t1 << t2 << t3;
            m_threadedPlayer->play(volume, tonePairs);
            duration = oneSec * 2.5;
        }
        return duration;
    }
}
