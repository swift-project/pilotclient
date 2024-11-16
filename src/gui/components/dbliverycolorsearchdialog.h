// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_DBLIVERYCOLORSEARCHDIALOG_H
#define SWIFT_GUI_COMPONENTS_DBLIVERYCOLORSEARCHDIALOG_H

#include <QDialog>
#include <QScopedPointer>

#include "misc/aviation/livery.h"

namespace Ui
{
    class CDbLiveryColorSearchDialog;
}
namespace swift::gui::components
{
    /*!
     * Search for best matching color livery
     */
    class CDbLiveryColorSearchDialog : public QDialog
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CDbLiveryColorSearchDialog(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CDbLiveryColorSearchDialog();

        //! Found livery if any, otherwise default
        const swift::misc::aviation::CLivery &getLivery() const;

    private:
        //! Dialog has been accepted
        void onAccepted();

        swift::misc::aviation::CLivery m_foundLivery; //!< last livery found

    private:
        QScopedPointer<Ui::CDbLiveryColorSearchDialog> ui;
    };
} // namespace swift::gui::components

#endif // guard
