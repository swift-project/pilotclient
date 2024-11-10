// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_SHAREDSTRINGLISTCOMPLETER_H
#define SWIFT_GUI_SHAREDSTRINGLISTCOMPLETER_H

#include "gui/swiftguiexport.h"

#include <QCompleter>
#include <QStringListModel>
#include <QScopedPointer>

namespace swift::gui
{
    /*!
     * Completer shared among multiple UI elements.
     * Normally used as static member
     */
    class SWIFT_GUI_EXPORT CSharedStringListCompleter
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
    class SWIFT_GUI_EXPORT CompleterUtils
    {
    public:
        //! No Ctor
        CompleterUtils() = delete;

        //! How completer behaves
        static void setCompleterParameters(QCompleter *completer);
    };
} // ns

#endif // guard
