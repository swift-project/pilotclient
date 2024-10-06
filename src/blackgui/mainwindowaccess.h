// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#ifndef BLACKGUI_MAINWINDOWACCESS_H
#define BLACKGUI_MAINWINDOWACCESS_H

#include "blackgui/blackguiexport.h"
#include "blackmisc/statusmessagelist.h"

#include <QObject>
#include <QString>

namespace BlackGui
{
    class CManagedStatusBar;
    class COverlayMessagesFrame;
    namespace Components
    {
        class CLogComponent;
    }

    /*!
     * Direct acccess to main window`s status bar, info bar and such
     */
    class BLACKGUI_EXPORT IMainWindowAccess
    {
    public:
        //! Destructor
        virtual ~IMainWindowAccess();

        //! Ctor
        IMainWindowAccess() {}

        //! Copy constructor
        IMainWindowAccess(const IMainWindowAccess &) = delete;

        //! Copy assignment operator
        IMainWindowAccess &operator=(const IMainWindowAccess &) = delete;

        //! Display in status bar
        virtual bool displayInStatusBar(const BlackMisc::CStatusMessage &message);

        //! Display in overlay window
        virtual bool displayInOverlayWindow(const BlackMisc::CStatusMessage &message, int timeOutMs = -1);

        //! Display in overlay window
        virtual bool displayInOverlayWindow(const BlackMisc::CStatusMessageList &messages, int timeOutMs = -1);

        //! Display in overlay window
        virtual bool displayInOverlayWindow(const QString &html, int timeOutMs = -1);

    protected:
        Components::CLogComponent *m_mwaLogComponent = nullptr; //!< the log component if any
        CManagedStatusBar *m_mwaStatusBar = nullptr; //!< status bar if any
        COverlayMessagesFrame *m_mwaOverlayFrame = nullptr; //!< overlay messages if any
    };
} // ns

Q_DECLARE_INTERFACE(BlackGui::IMainWindowAccess, "org.swift-project.blackgui.mainwindowaccess")

#endif // guard
