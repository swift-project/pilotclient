// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_COMPONENTS_COPYMODELSFROMOTHERSWIFTVERSIONSDIALOG_H
#define BLACKGUI_COMPONENTS_COPYMODELSFROMOTHERSWIFTVERSIONSDIALOG_H

#include <QDialog>
#include <QScopedPointer>

namespace Ui
{
    class CCopyModelsFromOtherSwiftVersionsDialog;
}
namespace BlackGui::Components
{
    /*!
     * Copy models from other swift versions as dialog
     */
    class CCopyModelsFromOtherSwiftVersionsDialog : public QDialog
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CCopyModelsFromOtherSwiftVersionsDialog(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CCopyModelsFromOtherSwiftVersionsDialog();

    private:
        QScopedPointer<Ui::CCopyModelsFromOtherSwiftVersionsDialog> ui;
    };
} // ns

#endif // guard
