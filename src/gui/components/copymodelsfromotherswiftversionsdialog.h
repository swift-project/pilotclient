// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_COPYMODELSFROMOTHERSWIFTVERSIONSDIALOG_H
#define SWIFT_GUI_COMPONENTS_COPYMODELSFROMOTHERSWIFTVERSIONSDIALOG_H

#include <QDialog>
#include <QScopedPointer>

namespace Ui
{
    class CCopyModelsFromOtherSwiftVersionsDialog;
}
namespace swift::gui::components
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
        ~CCopyModelsFromOtherSwiftVersionsDialog() override;

    private:
        QScopedPointer<Ui::CCopyModelsFromOtherSwiftVersionsDialog> ui;
    };
} // namespace swift::gui::components

#endif // SWIFT_GUI_COMPONENTS_COPYMODELSFROMOTHERSWIFTVERSIONSDIALOG_H
