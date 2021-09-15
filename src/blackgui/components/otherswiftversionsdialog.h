/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_OTHERSWIFTVERSIONSDIALOG_H
#define BLACKGUI_COMPONENTS_OTHERSWIFTVERSIONSDIALOG_H

#include <QDialog>
#include <QScopedPointer>

namespace Ui { class COtherSwiftVersionsDialog; }
namespace BlackGui::Components
{
    /**
     * COtherVersionsComponent as dialog
     */
    class COtherSwiftVersionsDialog : public QDialog
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit COtherSwiftVersionsDialog(QWidget *parent = nullptr);

        //! Destructor
        virtual ~COtherSwiftVersionsDialog();

    private:
        QScopedPointer<Ui::COtherSwiftVersionsDialog> ui;
    };
} // ns

#endif // guard
