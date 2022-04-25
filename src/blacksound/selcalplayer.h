/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKSOUND_SELCALPLAYER_H
#define BLACKSOUND_SELCALPLAYER_H

#include "blacksound/threadedtonepairplayer.h"
#include "blacksound/tonepair.h"
#include "blacksound/blacksoundexport.h"
#include "blackmisc/audio/audiodeviceinfo.h"
#include "blackmisc/aviation/selcal.h"
#include "blackmisc/worker.h"

namespace BlackSound
{
    //! SELCAL player
    class BLACKSOUND_EXPORT CSelcalPlayer : public QObject
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
