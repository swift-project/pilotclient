// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_STRINGLISTDIALOG_H
#define SWIFT_GUI_COMPONENTS_STRINGLISTDIALOG_H

#include <QDialog>
#include <QScopedPointer>

namespace Ui
{
    class CStringListDialog;
}
namespace swift::gui::components
{
    /*!
     * Select from a list of string
     */
    class CStringListDialog : public QDialog
    {
        Q_OBJECT

    public:
        //! Ctor
        explicit CStringListDialog(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CStringListDialog() override;

        //! Strings
        void setStrings(const QStringList &strings);

        //! Selected value
        QString getSelectedValue() const;

    private:
        QScopedPointer<Ui::CStringListDialog> ui;
    };
} // namespace swift::gui::components

#endif // SWIFT_GUI_COMPONENTS_STRINGLISTDIALOG_H
