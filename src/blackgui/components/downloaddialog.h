/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_DOWNLOADDIALOG_H
#define BLACKGUI_COMPONENTS_DOWNLOADDIALOG_H

#include "blackgui/blackguiexport.h"
#include "downloadcomponent.h"

#include <QDialog>
#include <QScopedPointer>

namespace BlackMisc { namespace Network { class CRemoteFile; class CRemoteFileList; }}
namespace Ui { class CDownloadDialog; }
namespace BlackGui
{
    namespace Components
    {
        /**
         * CDownloadComponent as dialog
         */
        class BLACKGUI_EXPORT CDownloadDialog : public QDialog
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CDownloadDialog(QWidget *parent = nullptr);

            //! Destructor
            virtual ~CDownloadDialog();

            //! \copydoc CDownloadComponent::setDownloadFile
            void setDownloadFile(const BlackMisc::Network::CRemoteFile &remoteFile);

            //! \copydoc CDownloadComponent::setDownloadFiles
            void setDownloadFiles(const BlackMisc::Network::CRemoteFileList &remoteFiles);

            //! \copydoc CDownloadComponent::triggerDownloadingOfFiles
            void triggerDownloadingOfFiles(int delayMs);

            //! \copydoc CDownloadComponent::setMode
            void setMode(CDownloadComponent::Mode mode);

            //! Show and start the downloading
            void showAndStartDownloading();

        protected:
            //! \copydoc QDialog::accept
            virtual void accept() override;

            //! \copydoc QDialog::reject
            virtual void reject() override;

        private:
            QScopedPointer<Ui::CDownloadDialog> ui;
        };
    } // ns
} // ns

#endif // guard
