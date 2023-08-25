// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_COMPONENTS_OTHERSWIFTVERSIONSDIALOG_H
#define BLACKGUI_COMPONENTS_OTHERSWIFTVERSIONSDIALOG_H

#include <QDialog>
#include <QScopedPointer>

namespace Ui
{
    class COtherSwiftVersionsDialog;
}
namespace BlackGui::Components
{
    /*!
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
