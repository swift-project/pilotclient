/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#ifndef BLACKCORE_PROGRESS_H
#define BLACKCORE_PROGRESS_H

#include "blackcore/application.h"

namespace BlackCore
{
    /*!
     * Implementing class features a progress bar or something similar.
     * Hence such an object can be passed and the progress monitored.
     */
    class IProgressIndicator
    {
    public:
        //! Update the progress indicator 0..100
        virtual void updateProgressIndicator(int percentage) = 0;

        //! Dtor
        virtual ~IProgressIndicator() {}

        //! Same as updateProgressIndicator but processing events
        void updateProgressIndicatorAndProcessEvents(int percentage)
        {
            this->updateProgressIndicator(percentage);
            BlackCore::CApplication::processEventsFor(10);
        }
    };
} // ns

#endif // guard
