/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_LOGINMODEBUTTONS_H
#define BLACKMISC_LOGINMODEBUTTONS_H

#include "blackcore/network.h"
#include <QGroupBox>
#include <QScopedPointer>


namespace Ui { class CLoginModeButtons; }

namespace BlackGui
{

    class CLoginModeButtons : public QGroupBox
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CLoginModeButtons(QWidget *parent = nullptr);

        //! Destructor
        ~CLoginModeButtons();

        //! Get login mode, \sa BlackCore::INetwork::LoginMode
        BlackCore::INetwork::LoginMode getLoginMode() const;

    private:
        QScopedPointer<Ui::CLoginModeButtons> ui;
    };

}

#endif // guard
