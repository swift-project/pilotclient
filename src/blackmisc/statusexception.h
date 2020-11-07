/* Copyright (C) 2015
 * swift Project Community / Contributors
 *
 * This file is part of Swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_STATUSEXCEPTION_H
#define BLACKMISC_STATUSEXCEPTION_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/statusmessage.h"

#include <QByteArray>
#include <QReadWriteLock>
#include <exception>

namespace BlackMisc
{
    /*!
     * Throwable exception class containing a CStatusMessage.
     */
    class BLACKMISC_EXPORT CStatusException : public std::exception
    {
    public:
        //! Constructor.
        explicit CStatusException(const CStatusMessage &payload);

        //! Copy constructor (because of mutex)
        CStatusException(const CStatusException &other);

        //! Copy assignment (because of mutex)
        CStatusException &operator=(const CStatusException &) = delete;

        //! Return null-terminated message string.
        virtual const char *what() const noexcept override;

        //! Return the contained status message.
        const CStatusMessage &status() const { return m_payload; }

        //! Destructor.
        ~CStatusException() override {}

        //! If the message is not empty then throw it.
        static void maybeThrow(const CStatusMessage &);

    private:
        const CStatusMessage   m_payload;
        mutable QByteArray     m_temp;
        mutable QReadWriteLock m_lock;  //!< lock (because of mutable members)
    };
} // ns

#endif
