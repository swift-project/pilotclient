// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

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
