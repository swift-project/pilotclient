/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef SAMPLES_H
#define SAMPLES_H

#include "blacksound/blacksoundexport.h"
#include "resourcesound.h"

namespace BlackSound
{
    namespace SampleProvider
    {
        class BLACKSOUND_EXPORT Samples
        {
        public:
            //! Singleton
            static Samples &instance();

            //! Various samples (sounds) @{
            CResourceSound crackle() const;
            CResourceSound click() const;
            CResourceSound whiteNoise() const;
            //! @}

        private:
            //! Ctor
            Samples();

            CResourceSound m_crackle;
            CResourceSound m_click;
            CResourceSound m_whiteNoise;
        };
    } // ns
} // ns

#endif // guard
