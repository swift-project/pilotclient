// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_SOUND_SELCALPLAYER_H
#define SWIFT_SOUND_SELCALPLAYER_H

#include "sound/threadedtonepairplayer.h"
#include "sound/tonepair.h"
#include "sound/swiftsoundexport.h"
#include "misc/audio/audiodeviceinfo.h"
#include "misc/aviation/selcal.h"
#include "misc/worker.h"

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
        virtual ~CSelcalPlayer() override;

        //! Stop the player thread
        void gracefulShutdown();

        //! Play SELCAL
        //! \return Time of the played tone
        swift::misc::physical_quantities::CTime play(int volume, const swift::misc::aviation::CSelcal &selcal);

    private:
        CThreadedTonePairPlayer *m_threadedPlayer = nullptr;
    };
}

#endif // guard
