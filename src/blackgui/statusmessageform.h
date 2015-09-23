/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_STATUSMESSAGEFORM_H
#define BLACKGUI_STATUSMESSAGEFORM_H

#include "blackgui/blackguiexport.h"
#include "blackmisc/statusmessage.h"
#include <QFrame>

namespace Ui { class CStatusMessageForm; }

namespace BlackGui
{
    /*!
     * Display details about a single status message
     */
    class BLACKGUI_EXPORT CStatusMessageForm : public QFrame
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CStatusMessageForm(QWidget *parent = nullptr);

        //! Destructor
        ~CStatusMessageForm();

    public slots:
        //! Set message
        void setValue(const BlackMisc::CStatusMessage &message);

        //! Toggle visibility
        void toggleVisibility();

    private:
        QScopedPointer<Ui::CStatusMessageForm> ui;
    };

} // ns
#endif // guard
