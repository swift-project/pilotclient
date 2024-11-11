// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_ALTITUDEEDIT_H
#define BLACKGUI_ALTITUDEEDIT_H

#include "misc/aviation/altitude.h"
#include "misc/statusmessagelist.h"
#include "blackgui/blackguiexport.h"
#include <QLineEdit>
#include <QRegularExpression>

namespace BlackGui
{
    /*!
     * Line edit for altitude as used in flight plan
     */
    class BLACKGUI_EXPORT CAltitudeEdit : public QLineEdit
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
} // ns

#endif // guard
