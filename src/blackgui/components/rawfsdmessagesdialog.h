/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_RAWFSDMESSAGESDIALOG_H
#define BLACKGUI_COMPONENTS_RAWFSDMESSAGESDIALOG_H

#include "blackgui/blackguiexport.h"
#include "blackgui/components/rawfsdmessagescomponent.h"

#include <QDialog>
#include <QScopedPointer>

namespace Ui { class CRawFsdMessagesDialog; }
namespace BlackGui::Components
{
    //! CRawFsdMessageComponent as dialog
    class BLACKGUI_EXPORT CRawFsdMessagesDialog : public QDialog
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CRawFsdMessagesDialog(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CRawFsdMessagesDialog();

    private:
        QScopedPointer<Ui::CRawFsdMessagesDialog> ui;
    };
} // ns

#endif // guard
