/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "selcalplayer.h"
#include <QTimer>

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::PhysicalQuantities;

namespace BlackSound
{
    CSelcalPlayer::CSelcalPlayer(const QAudioDeviceInfo &device, QObject *parent)
        : QObject(parent),
          m_threadedPlayer(this, "CSelcalPlayer", device)
    {
        m_threadedPlayer.start();
    }

    CSelcalPlayer::~CSelcalPlayer()
    {
        m_threadedPlayer.quitAndWait();
    }

    void CSelcalPlayer::play(int volume, const BlackMisc::Aviation::CSelcal &selcal)
    {
        if (selcal.isValid())
        {
            QList<CFrequency> frequencies = selcal.getFrequencies();
            Q_ASSERT(frequencies.size() == 4);
            const BlackMisc::PhysicalQuantities::CTime oneSec(1000.0, BlackMisc::PhysicalQuantities::CTimeUnit::ms());
            CTonePair t1(frequencies.at(0), frequencies.at(1), oneSec);
            CTonePair t2({}, {}, oneSec / 5.0);
            CTonePair t3(frequencies.at(2), frequencies.at(3), oneSec);
            QList<CTonePair> tonePairs;
            tonePairs << t1 << t2 << t3;
            m_threadedPlayer.play(volume, tonePairs);
        }
    }
}
