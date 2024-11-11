// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_COMPONENTS_STATUSMESSAGEFORMSMALL_H
#define BLACKGUI_COMPONENTS_STATUSMESSAGEFORMSMALL_H

#include "blackgui/blackguiexport.h"
#include "misc/statusmessage.h"
#include "misc/variant.h"

#include <QFrame>
#include <QObject>
#include <QScopedPointer>

namespace Ui
{
    class CStatusMessageFormSmall;
}
namespace BlackGui::Components
{
    /*!
     * Display details about a single status message
     */
    class BLACKGUI_EXPORT CStatusMessageFormSmall : public QFrame
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CStatusMessageFormSmall(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CStatusMessageFormSmall();

        //! Set message
        void setVariant(const swift::misc::CVariant &messageVariant);

        //! Set message
        void setValue(const swift::misc::CStatusMessage &message);

        //! Toggle visibility
        void toggleVisibility();

        //! Minimize by hiding status ...
        void setReducedInfo(bool minimized);

    private:
        QScopedPointer<Ui::CStatusMessageFormSmall> ui;
    };
} // ns
#endif // guard
