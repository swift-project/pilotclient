/* Copyright (C) 2014
 * Swift Project Community / Contributors
 *
 * This file is part of Swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef BLACKMISC_LOGMESSAGE_H
#define BLACKMISC_LOGMESSAGE_H

//! \file

#include "blackmiscexport.h"
#include "statusmessagelist.h"
#include "statusmessage.h"
#include "logcategorylist.h"
#include "indexsequence.h"
#include <QDebug>
#include <QLoggingCategory>
#include <QList>
#include <type_traits>

namespace BlackMisc
{
    /*!
     * Helper with static methods for dealing with metadata embedded in log message category strings.
     *
     * There are certain aspects of log messages which cannot be represented in Qt's native log message machinery.
     * Therefore we are forced to use a special encoding of the message category string to encode these aspects.
     *
     * An encoded category string consists of a plain category string with zero or more flag strings appended.
     * The plain category and the flags are all separated by forward-slash characters ('/').
     *
     * There are currently two flags:
     * \li \c "debug" Qt only has 3 ordinary severities (debug, warning, critical), so we use QtMsgDebug for both
     *                debug and info messages, and we use this flag to distinguish between them.
     * \li \c "redundant" To avoid handling the same message twice, this flag identifies a message which has already
     *                    been directly returned as the return value of the method which generated it.
     */
    class BLACKMISC_EXPORT CLogMessageHelper
    {
    public:
        //! Deleted constructor.
        CLogMessageHelper() = delete;

        //! Returns an encoded category string with the debug flag appended.
        static QString addDebugFlag(const QString &category);

        //! Strips all flags from an encoded category string, returning only the plain category string.
        static QString stripFlags(const QString &category);

        //! Returns true if the given encoded category string has the debug flag.
        static bool hasDebugFlag(const QString &category);
    };

    /*!
     * Class for emitting a log message. Works similar to the qDebug, qWarning, qCritical family of functions.
     *
     * The member functions debug, info, warning, error, and the stream operators all return a reference to <tt>*this</tt>,
     * so they can be chained together.
     *
     * The categories are arbitrary string tags which can be attached to the message to categorize it.
     * A message can have more than one category. The categories can be used for filtering by message handlers.
     */
    class BLACKMISC_EXPORT CLogMessage : public CMessageBase<CLogMessage>
    {
    public:
        //! Inheriting constructors.
        using CMessageBase::CMessageBase;

        //! Construct a message with the "uncategorized" category.
        CLogMessage();

        //! Constructor taking filename, line number, and function name, for uncategorized verbose debug messages.
        CLogMessage(const char *file, int line, const char *function);

        //! Destructor. This actually emits the message.
        ~CLogMessage();

        //! Convert to CStatusMessage for returning the message directly from the function which generated it.
        operator CStatusMessage();

        //! Convert to CVariant for returning the message directly from the function which generated it.
        operator CVariant();

        //! Sends a verbatim, preformatted message to the log.
        static void preformatted(const CStatusMessage &statusMessage);

        //! Sends a list of verbatim, preformatted messages to the log.
        static void preformatted(const CStatusMessageList &statusMessages);

    private:
        QMessageLogger m_logger;

        QByteArray qtCategory() const;
        QDebug ostream(const QByteArray &category) const;
    };
}

/*!
 * Convenience macro to construct a CLogMessage with the filename, line number, and function name,
 * for verbose debug messages.
 */
#define BLACK_LOG (BlackMisc::CLogMessage { __FILE__, __LINE__, Q_FUNC_INFO })

#endif
