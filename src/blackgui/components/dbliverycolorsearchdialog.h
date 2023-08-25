// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_COMPONENTS_DBLIVERYCOLORSEARCHDIALOG_H
#define BLACKGUI_COMPONENTS_DBLIVERYCOLORSEARCHDIALOG_H

#include "blackmisc/aviation/livery.h"
#include <QDialog>
#include <QScopedPointer>

namespace Ui
{
    class CDbLiveryColorSearchDialog;
}
namespace BlackGui::Components
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
        const BlackMisc::Aviation::CLivery &getLivery() const;

    private:
        //! Dialog has been accepted
        void onAccepted();

        BlackMisc::Aviation::CLivery m_foundLivery; //!< last livery found

    private:
        QScopedPointer<Ui::CDbLiveryColorSearchDialog> ui;
    };
} // ns

#endif // guard
