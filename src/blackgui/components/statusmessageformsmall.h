/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_STATUSMESSAGEFORMSMALL_H
#define BLACKGUI_COMPONENTS_STATUSMESSAGEFORMSMALL_H

#include "blackgui/blackguiexport.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/variant.h"

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
        void setVariant(const BlackMisc::CVariant &messageVariant);

        //! Set message
        void setValue(const BlackMisc::CStatusMessage &message);

        //! Toggle visibility
        void toggleVisibility();

        //! Minimize by hiding status ...
        void setReducedInfo(bool minimized);

    private:
        QScopedPointer<Ui::CStatusMessageFormSmall> ui;
    };
} // ns
#endif // guard
