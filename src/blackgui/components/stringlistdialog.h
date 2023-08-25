// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_COMPONENTS_STRINGLISTDIALOG_H
#define BLACKGUI_COMPONENTS_STRINGLISTDIALOG_H

#include <QDialog>
#include <QScopedPointer>

namespace Ui
{
    class CStringListDialog;
}
namespace BlackGui::Components
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
} // ns

#endif // guard
