#ifndef BLACKSIM_FSCOMMON_BCDCONVERSIONS_H
#define BLACKSIM_FSCOMMON_BCDCONVERSIONS_H

#include "blackmisc/pqfrequency.h"
#include "blackmisc/aviotransponder.h"
#include <QtGlobal>

namespace BlackSim
{
    namespace FsCommon
    {

        /*!
         * \brief BCD conversions for FS
         */
        class CBcdConversions
        {
        public:
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

    }
}

#endif // guard
