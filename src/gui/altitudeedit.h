// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_ALTITUDEEDIT_H
#define SWIFT_GUI_ALTITUDEEDIT_H

#include <QLineEdit>
#include <QRegularExpression>

#include "gui/swiftguiexport.h"
#include "misc/aviation/altitude.h"
#include "misc/statusmessagelist.h"

namespace swift::gui
{
    /*!
     * Line edit for altitude as used in flight plan
     */
    class SWIFT_GUI_EXPORT CAltitudeEdit : public QLineEdit
    {
        Q_OBJECT

    public:
        //! Constructor
        CAltitudeEdit(QWidget *parent = nullptr);

        //! Get altitude
        swift::misc::aviation::CAltitude getAltitude() const;

        //! Set altitude
        void setAltitude(const swift::misc::aviation::CAltitude &altitude);

        //! Valid altitude
        bool isValid(swift::misc::CStatusMessageList *msgs = nullptr) const;
    };
} // namespace swift::gui

#endif // guard
