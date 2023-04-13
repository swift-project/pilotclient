/* Copyright (C) 2017
 * swift project community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_COPYCONFIGURATIONDIALOG_H
#define BLACKGUI_COMPONENTS_COPYCONFIGURATIONDIALOG_H

#include "blackgui/blackguiexport.h"
#include <QDialog>
#include <QScopedPointer>

namespace Ui
{
    class CCopyConfigurationDialog;
}
namespace BlackGui::Components
{
    //! Dialog to copy cache and settings
    //! \deprecated replaced by CCopySettingsAndCachesComponent
    class BLACKGUI_EXPORT CCopyConfigurationDialog : public QDialog
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CCopyConfigurationDialog(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CCopyConfigurationDialog() override;

        //! For cache data
        void setCacheMode();

        //! For settings
        void setSettingsMode();

        //! Select all settings or caches
        void selectAll();

        //! \copydoc QFileSystemModel::setNameFilterDisables
        void setNameFilterDisables(bool disable);

        //! \copydoc CCopyConfigurationComponent::setWithBootstrapFile
        void setWithBootstrapFile(bool withBootstrapFile);

    protected:
        //! \copydoc QObject::event
        virtual bool event(QEvent *event) override;

    private:
        QScopedPointer<Ui::CCopyConfigurationDialog> ui;
    };
} // ns
#endif // guard
