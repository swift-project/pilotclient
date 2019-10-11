/* Copyright (C) 2019
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_FSD_PBH_H
#define BLACKCORE_FSD_PBH_H

#include <QtGlobal>
#include <QtMath>

namespace BlackCore
{
    namespace Fsd
    {
        //! Pitch bank heading union
        union PBH
        {
            unsigned int pbh = 0; //!< Pitch/Bank/Heading as integer value
            struct
            {
                unsigned int unused   : 1; //!< unused bit
                unsigned int onground : 1; //!< Onground flag
                unsigned int hdg      : 10; //!< Heading
                int bank              : 10; //!< Bank
                int pitch             : 10; //!< Pitch
            };
        };

        //! Pitch multiplier
        constexpr double pitchMultiplier()
        {
            return 256.0  / 90.0;
        }

        //! Bank multiplier
        constexpr double bankMultiplier()
        {
            return 512.0  / 180.0;
        }

        //! Heading multiplier
        constexpr double headingMultiplier()
        {
            return 1024.0 / 360.0;
        }

        //! Pack pitch, bank, heading and onGround into 32 bit integer
        inline void packPBH(double pitch, double bank, double heading, bool onGround, quint32 &pbh)
        {
            PBH pbhstrct;
            pbhstrct.pitch = qFloor(pitch * pitchMultiplier());
            pbhstrct.bank = qFloor(bank * bankMultiplier());
            pbhstrct.hdg = static_cast<unsigned int>(heading * headingMultiplier());

            // FSD has inverted pitch and bank angles
            pbhstrct.pitch = ~pbhstrct.pitch;
            pbhstrct.bank = ~pbhstrct.bank;

            pbhstrct.onground = onGround ? 1 : 0;

            pbh = pbhstrct.pbh;
        }

        //! Unpack pitch, bank, heading and onGround from 32 bit integer
        inline void unpackPBH(quint32 pbh, double &pitch, double &bank, double &heading, bool &onGround)
        {
            PBH pbhstrct;
            pbhstrct.pbh = pbh;
            int iPitch = qFloor(pbhstrct.pitch / pitchMultiplier());
            int iBank = qFloor(pbhstrct.bank / bankMultiplier());

            // MSFS has inverted pitch and bank angles
            iPitch = ~iPitch;
            iBank = ~iBank;

            pitch = iPitch;
            bank = iBank;
            heading = pbhstrct.hdg / headingMultiplier();

            onGround = pbhstrct.onground == 1;
        }
    }
}

#endif // guard
