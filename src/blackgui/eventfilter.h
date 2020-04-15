/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

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
