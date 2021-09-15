/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_DBLIVERYCOLORSEARCHDIALOG_H
#define BLACKGUI_COMPONENTS_DBLIVERYCOLORSEARCHDIALOG_H

#include "blackmisc/aviation/livery.h"
#include <QDialog>
#include <QScopedPointer>

namespace Ui { class CDbLiveryColorSearchDialog; }
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
