/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_JSONEXCEPTION_H
#define BLACKMISC_JSONEXCEPTION_H

#include "blackmisc/blackmiscexport.h"
#include <QString>
#include <stdexcept>

namespace BlackMisc
{
    class CStatusMessage;
    class CLogCategoryList;

    /*!
     * Thrown when a convertFromJson method encounters an unrecoverable error in JSON data.
     */
    class BLACKMISC_EXPORT CJsonException : public std::runtime_error
    {
        virtual void anchor();

    public:
        //! Constructor.
        explicit CJsonException(const QString &message) : std::runtime_error(message.toStdString()) {}

        //! Get a stack trace of where in the JSON object tree the error occurred.
        const QString &getStackTrace() const { return m_stack; }

        //! As string info
        QString toString(const QString &prefix) const;

    private:
        static QString stackString();

        QString m_stack = stackString();
    };

    /*!
     * Pseudo-RAII pattern that tracks the current JSON value being converted.
     *
     * Will be used to construct a stack trace for CJsonException::getStackTrace.
     * Class is optimized to introduce minimal overhead in the unexceptional case.
     */
    class BLACKMISC_EXPORT CJsonScope
    {
    public:
        //! @{
        //! Construct a scope with the given name and optional index subscript.
        CJsonScope(const QString &name, int index = -1) noexcept : m_index(index), m_string(&name) { push(); }
        CJsonScope(QLatin1String name, int index = -1) noexcept : m_index(index), m_latin1(name) { push(); }
        template <size_t N>
        CJsonScope(const char (&name)[N], int index = -1) noexcept : CJsonScope(QLatin1String(name, N), index)
        {}
        //! @}

        //! QString constructor argument must be an lvalue.
        CJsonScope(const QString &&, int = -1) = delete;

        //! @{
        //! Not copyable.
        CJsonScope(const CJsonScope &) = delete;
        CJsonScope &operator=(const CJsonScope &) = delete;
        //! @}

        //! Destructor.
        ~CJsonScope() { pop(); }

    private:
        void push() const noexcept;
        void pop() const noexcept;

        friend class CJsonException;
        int m_index = -1;
        const QString *m_string = nullptr;
        QLatin1String m_latin1;
    };
}

#endif
