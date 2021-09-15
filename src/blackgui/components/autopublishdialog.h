/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_AUTOPUBLISHDIALOG_H
#define BLACKGUI_COMPONENTS_AUTOPUBLISHDIALOG_H

#include "blackgui/blackguiexport.h"

#include <QDialog>
#include <QScopedPointer>

namespace Ui { class CAutoPublishDialog; }
namespace BlackGui::Components
{
    //! CAutoPublishComponent as dialog
    class BLACKGUI_EXPORT CAutoPublishDialog : public QDialog
    {
        Q_OBJECT

    public:
        //! Ctor
        explicit CAutoPublishDialog(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CAutoPublishDialog() override;

        //! \copydoc CAutoPublishComponent::readFiles
        int readFiles();

        //! Read files and show dialog
        int readAndShow();

    private:
        QScopedPointer<Ui::CAutoPublishDialog> ui;
    };
} // ns

#endif // guard
