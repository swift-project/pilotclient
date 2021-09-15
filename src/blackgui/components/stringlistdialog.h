/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_STRINGLISTDIALOG_H
#define BLACKGUI_COMPONENTS_STRINGLISTDIALOG_H

#include <QDialog>
#include <QScopedPointer>

namespace Ui { class CStringListDialog; }
namespace BlackGui::Components
{
    /**
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
