/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_AUDIOADVANCEDDISTRIBUTEDDIALOG_H
#define BLACKGUI_COMPONENTS_AUDIOADVANCEDDISTRIBUTEDDIALOG_H

#include "blackgui/blackguiexport.h"

#include <QDialog>
#include <QScopedPointer>

namespace Ui
{
    class CAudioAdvancedDistributedDialog;
}
namespace BlackGui::Components
{
    //! Audio advanced setup as dialog
    class BLACKGUI_EXPORT CAudioAdvancedDistributedDialog : public QDialog
    {
        Q_OBJECT

    public:
        //! Ctor
        explicit CAudioAdvancedDistributedDialog(QWidget *parent = nullptr);

        //! Dtor
        virtual ~CAudioAdvancedDistributedDialog() override;

        //! Reload registered devices
        void reloadRegisteredDevices();

    private:
        QScopedPointer<Ui::CAudioAdvancedDistributedDialog> ui;
    };
} // ns

#endif // guard
