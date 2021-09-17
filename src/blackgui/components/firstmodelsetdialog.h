/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_FIRSTMODELSETDIALOG_H
#define BLACKGUI_COMPONENTS_FIRSTMODELSETDIALOG_H

#include <QDialog>
#include <QScopedPointer>

namespace Ui { class CFirstModelSetDialog; }
namespace BlackGui::Components
{
    /*!
     * First model set dialog
     */
    class CFirstModelSetDialog : public QDialog
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CFirstModelSetDialog(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CFirstModelSetDialog();

    private:
        QScopedPointer<Ui::CFirstModelSetDialog> ui;
    };
} // ns
#endif // guard
