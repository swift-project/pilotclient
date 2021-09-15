/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_TEXTEDITDIALOG_H
#define BLACKGUI_COMPONENTS_TEXTEDITDIALOG_H

#include "blackgui/blackguiexport.h"

#include <QDialog>
#include <QScopedPointer>

class QTextEdit;

namespace Ui { class CTextEditDialog; }
namespace BlackGui::Components
{
    //! Text edit as dialog
    class BLACKGUI_EXPORT CTextEditDialog : public QDialog
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
} // ns
#endif // guard
