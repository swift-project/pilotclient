/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_ALTITUDEEDIT_H
#define BLACKGUI_ALTITUDEEDIT_H

#include "blackmisc/aviation/altitude.h"
#include "blackmisc/statusmessagelist.h"
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
        BlackMisc::Aviation::CAltitude getAltitude() const;

        //! Set altitude
        void setAltitude(const BlackMisc::Aviation::CAltitude &altitude);

        //! Valid altitude
        bool isValid(BlackMisc::CStatusMessageList *msgs = nullptr) const;
    };
} // ns

#endif // guard
