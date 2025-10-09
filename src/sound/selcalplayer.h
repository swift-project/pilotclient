// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_SOUND_SELCALPLAYER_H
#define SWIFT_SOUND_SELCALPLAYER_H

#include <chrono>

#include "misc/audio/audiodeviceinfo.h"
#include "misc/aviation/selcal.h"
#include "misc/worker.h"
#include "sound/swiftsoundexport.h"
#include "sound/threadedtonepairplayer.h"
#include "sound/tonepair.h"

namespace swift::sound
{
    //! SELCAL player
    class SWIFT_SOUND_EXPORT CSelcalPlayer : public QObject
    {
        Q_OBJECT

    public:
        //! Constructor
        CSelcalPlayer(const swift::misc::audio::CAudioDeviceInfo &device, QObject *parent = nullptr);

        //! Destructor
        ~CSelcalPlayer() override;

        //! Stop the player thread
        void gracefulShutdown();

        //! Play SELCAL
        //! \return Time of the played tone
        std::chrono::milliseconds play(int volume, const swift::misc::aviation::CSelcal &selcal);

    private:
        CThreadedTonePairPlayer *m_threadedPlayer = nullptr;
    };
} // namespace swift::sound

#endif // SWIFT_SOUND_SELCALPLAYER_H
