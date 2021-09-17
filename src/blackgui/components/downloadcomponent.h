/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_DOWNLOADCOMPONENT_H
#define BLACKGUI_COMPONENTS_DOWNLOADCOMPONENT_H

#include "blackgui/blackguiexport.h"
#include "blackgui/overlaymessagesframe.h"
#include "blackgui/loadindicator.h"
#include "blackmisc/network/remotefilelist.h"

#include <QNetworkReply>
#include <QFileDialog>
#include <QWizard>
#include <QScopedPointer>
#include <QFlags>

namespace Ui { class CDownloadComponent; }
namespace BlackGui::Components
{
    /*!
     * Download a file
     */
    class BLACKGUI_EXPORT CDownloadComponent :
        public COverlayMessagesFrame,
        public CLoadIndicatorEnabled
    {
        Q_OBJECT

    public:
        //! How to run
        enum ModeFlag
        {
            JustDownload,       //!< download, that's it
            StartAfterDownload, //!< download, then install
            ShutdownSwift,      //!< for installers, stop swift before running
            SwiftInstaller = StartAfterDownload | ShutdownSwift
        };
        Q_DECLARE_FLAGS(Mode, ModeFlag)

        //! Default constructor
        explicit CDownloadComponent(QWidget *parent = nullptr);

        //! Dtor
        virtual ~CDownloadComponent();

        //! Set file to be downloaded
        bool setDownloadFile(const BlackMisc::Network::CRemoteFile &remoteFile);

        //! Set files to be downloaded
        bool setDownloadFiles(const BlackMisc::Network::CRemoteFileList &remoteFiles);

        //! Set donwload directory
        bool setDownloadDirectory(const QString &path);

        //! Trigger downloading of the file
        bool triggerDownloadingOfFiles(int delayMs = -1);

        //! Downloads in progress
        bool isDownloading() const;

        //! Have all downloads completed?
        bool haveAllDownloadsCompleted() const;

        //! Get the mode
        Mode getMode() const;

        //! Set the mode
        void setMode(Mode mode);

        //! Clear all values
        void clear();

        //! Cancel ongoing downloads
        void cancelOngoingDownloads();

    signals:
        //! All downloads have been completed
        void allDownloadsCompleted();

    private:
        static constexpr int OverlayMsgTimeoutMs = 5000; //!< how long overlay is displayed
        QScopedPointer<Ui::CDownloadComponent> ui;
        const QFileDialog::Options m_fileDialogOptions { QFileDialog::ShowDirsOnly | QFileDialog::ReadOnly | QFileDialog::DontResolveSymlinks };
        BlackMisc::Network::CRemoteFile m_fileInProgress;  //!< file currently downloading
        BlackMisc::Network::CRemoteFileList m_remoteFiles; //!< files for download
        BlackMisc::Network::CRemoteFileList m_waitingForDownload; //!< files currently waiting for download
        QNetworkReply *m_reply = nullptr;

        //! Select download directory
        void selectDownloadDirectory();

        //! Trigger the download of the next file
        bool triggerDownloadingOfNextFile();

        //! Trigger downloading of the file
        bool triggerDownloadingOfFile(const BlackMisc::Network::CRemoteFile &remoteFile);

        //! Downloaded file
        void downloadedFile(const BlackMisc::CStatusMessage &status);

        //! Last file was downloaded
        void lastFileDownloaded();

        //! Start download
        void startDownloadedExecutable();

        //! Is the download dir existing?
        bool existsDownloadDir() const;

        //! Show download dir
        void openDownloadDir();

        //! Reset the download directory
        void resetDownloadDir();

        //! Show started file download
        void showStartedFileMessage(const BlackMisc::Network::CRemoteFile &rf);

        //! Show completed file download
        void showCompletedFileMessage(const BlackMisc::Network::CRemoteFile &rf);

        //! Download progress
        void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);

        //! File info
        void showFileInfo();
    };
} // ns

Q_DECLARE_METATYPE(BlackGui::Components::CDownloadComponent::Mode)
Q_DECLARE_METATYPE(BlackGui::Components::CDownloadComponent::ModeFlag)
Q_DECLARE_OPERATORS_FOR_FLAGS(BlackGui::Components::CDownloadComponent::Mode)

#endif // guard
