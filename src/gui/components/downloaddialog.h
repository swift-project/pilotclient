// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_DOWNLOADDIALOG_H
#define SWIFT_GUI_COMPONENTS_DOWNLOADDIALOG_H

#include "gui/swiftguiexport.h"
#include "gui/components/downloadcomponent.h"

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
namespace swift::gui::components
{
    /*!
     * CDownloadComponent as dialog
     */
    class SWIFT_GUI_EXPORT CDownloadDialog : public QDialog
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
