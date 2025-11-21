// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#ifndef SWIFT_GUI_MAINWINDOWACCESS_H
#define SWIFT_GUI_MAINWINDOWACCESS_H

#include <QObject>
#include <QString>

#include "gui/swiftguiexport.h"
#include "misc/statusmessagelist.h"

namespace swift::gui
{
    class CManagedStatusBar;
    class COverlayMessagesFrame;
    namespace components
    {
        class CLogComponent;
    }

    /*!
     * Direct acccess to main window`s status bar, info bar and such
     */
    class SWIFT_GUI_EXPORT IMainWindowAccess
    {
    public:
        //! Destructor
        virtual ~IMainWindowAccess();

        //! Ctor
        IMainWindowAccess() = default;

        //! Copy constructor
        IMainWindowAccess(const IMainWindowAccess &) = delete;

        //! Copy assignment operator
        IMainWindowAccess &operator=(const IMainWindowAccess &) = delete;

        //! Display in status bar
        virtual bool displayInStatusBar(const swift::misc::CStatusMessage &message);

        //! Display in overlay window
        virtual bool displayInOverlayWindow(const swift::misc::CStatusMessage &message,
                                            std::chrono::milliseconds timeout = std::chrono::milliseconds(0));

        //! Display in overlay window
        virtual bool displayInOverlayWindow(const swift::misc::CStatusMessageList &messages,
                                            std::chrono::milliseconds timeout = std::chrono::milliseconds(0));

        //! Display in overlay window
        virtual bool displayInOverlayWindow(const QString &html,
                                            std::chrono::milliseconds timeout = std::chrono::milliseconds(0));

    protected:
        components::CLogComponent *m_mwaLogComponent = nullptr; //!< the log component if any
        CManagedStatusBar *m_mwaStatusBar = nullptr; //!< status bar if any
        COverlayMessagesFrame *m_mwaOverlayFrame = nullptr; //!< overlay messages if any
    };
} // namespace swift::gui

Q_DECLARE_INTERFACE(swift::gui::IMainWindowAccess, "org.swift-project.swiftgui.mainwindowaccess")

#endif // SWIFT_GUI_MAINWINDOWACCESS_H
