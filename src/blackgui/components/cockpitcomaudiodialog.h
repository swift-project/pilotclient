/* Copyright (C) 2019 - let's see if there will be a (C) 2020
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_COCKPITCOMAUDIODIALOG_H
#define BLACKGUI_COMPONENTS_COCKPITCOMAUDIODIALOG_H

#include "blackgui/blackguiexport.h"
#include <QDialog>

namespace Ui
{
    class CCockpitComAudioDialog;
}
namespace BlackGui::Components
{
    //! Cockpit COM component as dialog
    class BLACKGUI_EXPORT CCockpitComAudioDialog : public QDialog
    {
        Q_OBJECT

    public:
        //! Ctor
        explicit CCockpitComAudioDialog(QWidget *parent = nullptr);

        //! Dtor
        virtual ~CCockpitComAudioDialog() override;

    private:
        QScopedPointer<Ui::CCockpitComAudioDialog> ui;
    };
} // ns

#endif // guard
