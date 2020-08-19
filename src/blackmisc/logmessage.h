/* Copyright (C) 2014
 * Swift Project Community / Contributors
 *
 * This file is part of Swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#ifndef BLACKMISC_LOGMESSAGE_H
#define BLACKMISC_LOGMESSAGE_H

//! \file

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/statusmessagelist.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/logcategorylist.h"
#include "blackmisc/integersequence.h"
#include <QDebug>
#include <QLoggingCategory>
#include <QList>
#include <type_traits>

namespace BlackMisc
{
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
