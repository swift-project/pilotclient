/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_COPYMODELSFROMOTHERSWIFTVERSIONSDIALOG_H
#define BLACKGUI_COMPONENTS_COPYMODELSFROMOTHERSWIFTVERSIONSDIALOG_H

#include <QDialog>
#include <QScopedPointer>

namespace Ui { class CCopyModelsFromOtherSwiftVersionsDialog; }
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
