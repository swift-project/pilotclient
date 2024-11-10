// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_COMPONENTS_DOWNLOADDIALOG_H
#define BLACKGUI_COMPONENTS_DOWNLOADDIALOG_H

#include "blackgui/blackguiexport.h"
#include "blackgui/components/downloadcomponent.h"

#include <QDialog>
#include <QScopedPointer>

namespace Ui
{
    class CDownloadDialog;
}
namespace swift::misc::network
{
    class CRemoteFile;
    class CRemoteFileList;
}
namespace BlackGui::Components
{
    /*!
     * CDownloadComponent as dialog
     */
    class BLACKGUI_EXPORT CDownloadDialog : public QDialog
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CDownloadDialog(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CDownloadDialog() override;

        //! \copydoc CDownloadComponent::setDownloadFile
        void setDownloadFile(const swift::misc::network::CRemoteFile &remoteFile);

        //! \copydoc CDownloadComponent::setDownloadFiles
        void setDownloadFiles(const swift::misc::network::CRemoteFileList &remoteFiles);

        //! \copydoc CDownloadComponent::triggerDownloadingOfFiles
        void triggerDownloadingOfFiles(int delayMs);

        //! \copydoc CDownloadComponent::setMode
        void setMode(CDownloadComponent::Mode mode);

        //! Show and start the downloading
        void showAndStartDownloading();

    protected:
        //! \copydoc QDialog::accept
        virtual void accept() override;

    private:
        QScopedPointer<Ui::CDownloadDialog> ui;
    };
} // ns

#endif // guard
