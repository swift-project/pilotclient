/* Copyright (C) 2014
 * Swift Project Community / Contributors
 *
 * This file is part of Swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of Swift Project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef BLACKMISC_LOGMESSAGE_H
#define BLACKMISC_LOGMESSAGE_H

//! \file

#include "statusmessage.h"
#include "index_sequence.h"
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
    class CLogMessageHelper
    {
    public:
        //! Deleted constructor.
        CLogMessageHelper() = delete;

        //! Returns an encoded category string with the debug flag appended.
        static QString addDebugFlag(const QString &category);

        //! Returns an encoded category string with the redundant flag appended.
        static QString addRedundantFlag(const QString &category);

        //! Strips all flags from an encoded category string, returning only the plain category string.
        static QString stripFlags(const QString &category);

        //! Returns true if the given encoded category string has the debug flag.
        static bool hasDebugFlag(const QString &category);

        //! Returns true if the given encoded category string has the redundant flag.
        static bool hasRedundantFlag(const QString &category);
    };

    /*!
     * Class for emitting a log message. Works similar to the qDebug, qWarning, qCritical family of functions.
     *
     * The member functions debug, info, warning, error, and the stream operators all return a reference to <tt>*this</tt>,
     * so they can be chained together.
     *
     * The category string identifies the origin of the message (e.g. network system) or its subtype (e.g. validation).
     */
    class CLogMessage
    {
    public:
        //! Constructor.
        CLogMessage() {}

        //! Constructor taking filename, line number, and function name, for verbose debug messages.
        CLogMessage(const char *file, int line, const char *function): m_logger(file, line, function) {}

        //! Destructor. This actually emits the message.
        ~CLogMessage();

        //! Convert to CStatusMessage for returning the message directly from the function which generated it.
        operator CStatusMessage();

        //! Convert to CVariant for returning the message directly from the function which generated it.
        operator CVariant();

        //! Set the severity to debug, with the default category.
        CLogMessage &debug() { return debugImpl(""); }

        //! Set the severity to debug, with a category string.
        CLogMessage &debug(QString category) { return debugImpl("", category); }

        //! Set the severity to debug, with the category string obtained from the getMessageCategory method of the sender.
        //! \note To avoid overload ambiguity, this method is disabled if T is not a class type.
        template <class T, class = typename std::enable_if<std::is_class<typename std::decay<T>::type>::value>::type>
        CLogMessage &debug(T *sender) { Q_UNUSED(sender); return debugImpl("", sender->getMessageCategory()); }

        //! Set the severity to info, providing a format string, with the default category.
        CLogMessage &info(QString format) { return infoImpl(format); }

        //! Set the severity to info, providing a format string and category string.
        CLogMessage &info(QString category, QString format) { return infoImpl(format, category); }

        //! Set the severity to info, providing a format string, with the category string obtained from the getMessageCategory method of the sender.
        //! \note To avoid overload ambiguity, this method is disabled if T is not a class type.
        template <class T, class = typename std::enable_if<std::is_class<typename std::decay<T>::type>::value>::type>
        CLogMessage &info(T *sender, QString format) { Q_UNUSED(sender); return infoImpl(format, sender->getMessageCategory()); }

        //! Set the severity to warning, providing a format string, with the default category.
        CLogMessage &warning(QString format) { return warningImpl(format); }

        //! Set the severity to warning, providing a format string and category string.
        CLogMessage &warning(QString category, QString format) { return warningImpl(format, category); }

        //! Set the severity to warning, providing a format string, with the category string obtained from the getMessageCategory method of the sender.
        //! \note To avoid overload ambiguity, this method is disabled if T is not a class type.
        template <class T, class = typename std::enable_if<std::is_class<typename std::decay<T>::type>::value>::type>
        CLogMessage &warning(T *sender, QString format) { Q_UNUSED(sender); return warningImpl(format, sender->getMessageCategory()); }

        //! Set the severity to error, providing a format string, with the default category.
        CLogMessage &error(QString format) { return errorImpl(format); }

        //! Set the severity to error, providing a format string and category string.
        CLogMessage &error(QString category, QString format) { return errorImpl(format, category); }

        //! Set the severity to error, providing a format string, with the category string obtained from the getMessageCategory method of the sender.
        template <class T, class = typename std::enable_if<std::is_class<typename std::decay<T>::type>::value>::type>
        CLogMessage &error(T *sender, QString format) { Q_UNUSED(sender); return errorImpl(format, sender->getMessageCategory()); }

        //! Streaming operators.
        //! \details If the format string is empty, the message will consist of all streamed values separated by spaces.
        //!          Otherwise, the streamed values will replace the place markers %1, %2, %3... in the format string.
        //! \see QString::arg
        //! @{
        CLogMessage &operator <<(const QString &v) { return arg(v); }
        CLogMessage &operator <<(int v) { return arg(QString::number(v)); }
        CLogMessage &operator <<(uint v) { return arg(QString::number(v)); }
        CLogMessage &operator <<(long v) { return arg(QString::number(v)); }
        CLogMessage &operator <<(ulong v) { return arg(QString::number(v)); }
        CLogMessage &operator <<(qlonglong v) { return arg(QString::number(v)); }
        CLogMessage &operator <<(qulonglong v) { return arg(QString::number(v)); }
        CLogMessage &operator <<(short v) { return arg(QString::number(v)); }
        CLogMessage &operator <<(ushort v) { return arg(QString::number(v)); }
        CLogMessage &operator <<(QChar v) { return arg(v); }
        CLogMessage &operator <<(char v) { return arg(QChar(v)); }
        CLogMessage &operator <<(double v) { return arg(QString::number(v)); }
        CLogMessage &operator <<(const CValueObject &v) { return arg(v.toQString()); }
        //! @}

        //! The default message category which is used if a category is not provided.
        static const char *defaultMessageCategory() { return "swift"; }

    private:
        QMessageLogger m_logger;
        CStatusMessage::StatusSeverity m_severity { CStatusMessage::SeverityDebug };
        QString m_category;
        QString m_message;
        QStringList m_args;
        bool m_redundant = false;

        CLogMessage &debugImpl(QString format, QString category = defaultMessageCategory());
        CLogMessage &infoImpl(QString format, QString category = defaultMessageCategory());
        CLogMessage &warningImpl(QString format, QString category = defaultMessageCategory());
        CLogMessage &errorImpl(QString format, QString category = defaultMessageCategory());
        CLogMessage &arg(QString value) { m_args.push_back(value); return *this; }
        QString message() const;
        QByteArray encodedCategory() const;
        QDebug ostream(const QByteArray &category) const;
    };
}

/*!
 * Convenience macro to construct a CLogMessage with the filename, line number, and function name,
 * for verbose debug messages.
 */
#define BLACK_LOG (BlackMisc::CLogMessage{ __FILE__, __LINE__, Q_FUNC_INFO })

#endif
