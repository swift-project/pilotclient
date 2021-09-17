/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_SHAREDSTRINGLISTCOMPLETER_H
#define BLACKGUI_SHAREDSTRINGLISTCOMPLETER_H

#include "blackgui/blackguiexport.h"

#include <QCompleter>
#include <QStringListModel>
#include <QScopedPointer>

namespace BlackGui
{
    /*!
     * Completer shared among multiple UI elements.
     * Normally used as static member
     */
    class BLACKGUI_EXPORT CSharedStringListCompleter
    {
    public:
        //! Constructor
        CSharedStringListCompleter() {}

        //! Update data
        bool updateData(const QStringList &data, int cacheTimeMs);

        //! Clear completer data
        void clearData();

        //! Was updated within check time
        bool wasUpdatedWithinTime(int checkTimeMs) const;

        //! Contains the string completer the given value
        bool contains(const QString &value, Qt::CaseSensitivity cs = Qt::CaseInsensitive) const;

        //! Values
        QStringList stringList() const;

        //! Completer
        QCompleter *completer() const { return m_completer.data(); }

        //! Convert to completer
        operator QCompleter *() const { return m_completer.data(); }

    private:
        qint64 m_lastUpdated = 0;
        QScopedPointer<QCompleter> m_completer { new QCompleter(QStringList()) }; // empty list required to init model
        QStringListModel *getCompleterModel() const;
    };

    /*!
     * Utility functions
     */
    class BLACKGUI_EXPORT CompleterUtils
    {
    public:
        //! No Ctor
        CompleterUtils() = delete;

        //! How completer behaves
        static void setCompleterParameters(QCompleter *completer);
    };
} // ns

#endif // guard
