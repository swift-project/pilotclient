// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_STATUSEXCEPTION_H
#define SWIFT_MISC_STATUSEXCEPTION_H

#include <exception>

#include <QByteArray>
#include <QReadWriteLock>

#include "misc/statusmessage.h"
#include "misc/swiftmiscexport.h"

namespace swift::misc
{
    /*!
     * Throwable exception class containing a CStatusMessage.
     */
    class SWIFT_MISC_EXPORT CStatusException : public std::exception
    {
    public:
        //! Constructor.
        explicit CStatusException(const CStatusMessage &payload);

        //! Copy constructor (because of mutex)
        CStatusException(const CStatusException &other);

        //! Copy assignment (because of mutex)
        CStatusException &operator=(const CStatusException &) = delete;

        //! Return null-terminated message string.
        const char *what() const noexcept override;

        //! Return the contained status message.
        const CStatusMessage &status() const { return m_payload; }

        //! Destructor.
        ~CStatusException() override = default;

        //! If the message is not empty then throw it.
        static void maybeThrow(const CStatusMessage &);

    private:
        const CStatusMessage m_payload;
        mutable QByteArray m_temp;
        mutable QReadWriteLock m_lock; //!< lock (because of mutable members)
    };
} // namespace swift::misc

#endif // SWIFT_MISC_STATUSEXCEPTION_H
