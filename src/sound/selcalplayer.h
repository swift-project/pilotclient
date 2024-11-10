// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKSOUND_SELCALPLAYER_H
#define BLACKSOUND_SELCALPLAYER_H

#include "sound/threadedtonepairplayer.h"
#include "sound/tonepair.h"
#include "sound/swiftsoundexport.h"
#include "blackmisc/audio/audiodeviceinfo.h"
#include "blackmisc/aviation/selcal.h"
#include "blackmisc/worker.h"

namespace swift::sound
{
    //! SELCAL player
    class SWIFT_SOUND_EXPORT CSelcalPlayer : public QObject
    {
        Q_OBJECT

    public:
        //! Constructor
        CSelcalPlayer(const BlackMisc::Audio::CAudioDeviceInfo &device, QObject *parent = nullptr);

        //! Destructor
        virtual ~CSelcalPlayer() override;

        //! Stop the player thread
        void gracefulShutdown();

        //! Play SELCAL
        //! \return Time of the played tone
        BlackMisc::PhysicalQuantities::CTime play(int volume, const BlackMisc::Aviation::CSelcal &selcal);

    private:
        CThreadedTonePairPlayer *m_threadedPlayer = nullptr;
    };
}

#endif // guard
