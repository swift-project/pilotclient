// SPDX-FileCopyrightText: Copyright (C) 2014 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#ifndef SWIFT_MISC_LOGMESSAGE_H
#define SWIFT_MISC_LOGMESSAGE_H

//! \file

#include <type_traits>

#include <QDebug>
#include <QList>
#include <QLoggingCategory>

#include "misc/integersequence.h"
#include "misc/statusmessage.h"
#include "misc/statusmessagelist.h"
#include "misc/swiftmiscexport.h"

namespace swift::misc
{
    /*!
     * Class for emitting a log message. Works similar to the qDebug, qWarning, qCritical family of functions.
     *
     * The member functions debug, info, warning, error, and the stream operators all return a reference to
     * <tt>*this</tt>, so they can be chained together.
     *
     * The categories are arbitrary string tags which can be attached to the message to categorize it.
     * A message can have more than one category. The categories can be used for filtering by message handlers.
     */
    class SWIFT_MISC_EXPORT CLogMessage : public CMessageBase<CLogMessage>
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

        //! Sends a verbatim, preformatted message to the log.
        static void preformatted(const CStatusMessage &statusMessage);

        //! Sends a list of verbatim, preformatted messages to the log.
        static void preformatted(const CStatusMessageList &statusMessages);

    private:
        QMessageLogger m_logger;

        QByteArray qtCategory() const;
        QDebug ostream(const QByteArray &category) const;
    };
} // namespace swift::misc

/*!
 * Convenience macro to construct a CLogMessage with the filename, line number, and function name,
 * for verbose debug messages.
 */
#define SWIFT_LOG (swift::misc::CLogMessage { __FILE__, __LINE__, Q_FUNC_INFO })

#endif
