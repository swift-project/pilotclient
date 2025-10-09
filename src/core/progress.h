// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#ifndef SWIFT_CORE_PROGRESS_H
#define SWIFT_CORE_PROGRESS_H

#include "core/application.h"

namespace swift::core
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
        virtual ~IProgressIndicator() = default;

        //! Same as updateProgressIndicator but processing events
        void updateProgressIndicatorAndProcessEvents(int percentage)
        {
            this->updateProgressIndicator(percentage);
            swift::core::CApplication::processEventsFor(10);
        }
    };
} // namespace swift::core

#endif // SWIFT_CORE_PROGRESS_H
