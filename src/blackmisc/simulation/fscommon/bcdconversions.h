/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SIMULATION_FSCOMMON_BCDCONVERSIONS_H
#define BLACKMISC_SIMULATION_FSCOMMON_BCDCONVERSIONS_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/pq/frequency.h"
#include <QtGlobal>

namespace BlackMisc
{
    namespace Aviation
    {
        class CTransponder;
    }
    namespace Simulation::FsCommon
    {
        //! BCD conversions for FS
        class BLACKMISC_EXPORT CBcdConversions
        {
        public:
            //! BCD -> decimal
            static quint32 bcd2Dec(qint32 bcdNum) { return bcd2Dec(static_cast<quint32>(bcdNum)); }

            //! BCD -> decimal
            static quint32 bcd2Dec(quint32 bcdNum) { return hornerScheme(bcdNum, 0x10, 10); }

            //! Decimal -> BCD
            static quint32 dec2Bcd(quint32 decNum) { return hornerScheme(decNum, 10, 0x10); }

            //! COM Frequency to BCD
            static quint32 comFrequencyToBcdHz(const BlackMisc::PhysicalQuantities::CFrequency &comFrequency);

            //! Transponder code to BCD
            static quint32 transponderCodeToBcd(const BlackMisc::Aviation::CTransponder &transponder);

        private:
            //! Constructor, only static methods
            CBcdConversions() {}

            //! Horner scheme
            static quint32 hornerScheme(quint32 num, quint32 divider, quint32 factor);
        };
    } // ns
} // ns

#endif // guard
