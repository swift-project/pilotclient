// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_COMPONENTS_AUTOPUBLISHDIALOG_H
#define BLACKGUI_COMPONENTS_AUTOPUBLISHDIALOG_H

#include "blackgui/blackguiexport.h"

#include <QDialog>
#include <QScopedPointer>

namespace Ui
{
    class CAutoPublishDialog;
}
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
