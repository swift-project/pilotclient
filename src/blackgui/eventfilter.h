// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_EVENTFILTER_H
#define BLACKGUI_EVENTFILTER_H

#include <QObject>

namespace BlackGui
{
    //! Uppercase key press
    class CUpperCaseEventFilter : public QObject
    {
        Q_OBJECT

    public:
        //! \copydoc QObject::QObject
        using QObject::QObject;

        //! Not allowed characters
        void setIllegalCharacters(const QString &illegal) { m_illegalChars = illegal; }

        //! Allow only ASCII
        void setOnlyAscii() { m_onlyAscii = true; }

    protected:
        //! Filter
        virtual bool eventFilter(QObject *object, QEvent *event) override;

    private:
        QString m_illegalChars;
        bool m_onlyAscii = false;
    };
} // namespace

#endif // guard
