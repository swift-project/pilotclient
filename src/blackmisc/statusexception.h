/* Copyright (C) 2015
 * Swift Project Community / Contributors
 *
 * This file is part of Swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_STATUSEXCEPTION_H
#define BLACKMISC_STATUSEXCEPTION_H

#include "statusmessage.h"
#include <stdexcept>

namespace BlackMisc
{

    /*!
     * Throwable exception class containing a CStatusMessage.
     *
     * This is the exception type which may be thrown by CStatusMessage::maybeThrow().
     */
    class BLACKMISC_EXPORT CStatusException : public std::exception
    {
    public:
        //! Constructor.
        explicit CStatusException(const CStatusMessage &payload) :
            m_payload(payload)
        {}

        //! Return null-terminated message string.
        virtual const char *what() const Q_DECL_NOEXCEPT override
        {
            return m_temp = m_payload.getMessage().toLocal8Bit();
        }

        //! Return the contained status message.
        const CStatusMessage &status() const
        {
            return m_payload;
        }

        //! Destructor.
        ~CStatusException() Q_DECL_NOEXCEPT {}

    private:
        const CStatusMessage m_payload;
        mutable QByteArray m_temp;
    };

}

#endif
