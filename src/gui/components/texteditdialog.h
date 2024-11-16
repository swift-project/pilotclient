// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_TEXTEDITDIALOG_H
#define SWIFT_GUI_COMPONENTS_TEXTEDITDIALOG_H

#include <QDialog>
#include <QScopedPointer>

#include "gui/swiftguiexport.h"

class QTextEdit;

namespace Ui
{
    class CTextEditDialog;
}
namespace swift::gui::components
{
    //! Text edit as dialog
    class SWIFT_GUI_EXPORT CTextEditDialog : public QDialog
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CTextEditDialog(QWidget *parent = nullptr);

        //! Dtor
        virtual ~CTextEditDialog() override;

        //! Access to text edit
        QTextEdit *textEdit() const;

        //! Set read only
        void setReadOnly();

    private:
        QScopedPointer<Ui::CTextEditDialog> ui;
    };
} // namespace swift::gui::components
#endif // guard
