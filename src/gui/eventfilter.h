// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_EVENTFILTER_H
#define SWIFT_GUI_EVENTFILTER_H

#include <QObject>

namespace swift::gui
{
    //! Uppercase key press
    class CUpperCaseEventFilter : public QObject
    {
        Q_OBJECT

    public:
        //! \copydoc QObject::QObject
        using QObject::QObject;

    protected:
        //! Filter
        bool eventFilter(QObject *object, QEvent *event) override;
    };
} // namespace swift::gui

#endif // SWIFT_GUI_EVENTFILTER_H
