// SPDX-FileCopyrightText: Copyright (C) 2014 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_DOCKWIDGET_H
#define SWIFT_GUI_DOCKWIDGET_H

#include <QDockWidget>
#include <QMargins>
#include <QObject>
#include <QPoint>
#include <QSize>
#include <QString>

#include "gui/enableforframelesswindow.h"
#include "gui/managedstatusbar.h"
#include "gui/overlaymessagesframe.h"
#include "gui/settings/dockwidgetsettings.h"
#include "gui/swiftguiexport.h"
#include "misc/statusmessage.h"
#include "misc/statusmessagelist.h"

class QCloseEvent;
class QMenu;
class QMouseEvent;
class QPaintEvent;
class QWidget;
class QWidgetAction;

namespace swift::gui
{
    namespace components
    {
        class CMarginsInput;
    }
    namespace menus
    {
        class CFontMenu;
    }

    //! \brief Our base class for dockable widgets containing some specialized functionality on top of QDockWidget.
    //! \details We currently use dockable widgets either as "info area" or "info bar" dockable widget.
    //! Info area widgets reside in an info are and represent a larger piece of information (e.g. all ATC stations, all aircrafts in range).
    //! An info bar is meant to be a small info window displaying information about status, menu state etc.
    //!
    //! \sa CInfoArea
    //! \sa CDockWidgetInfoArea
    //! \sa CDockWidgetInfoBar
    class SWIFT_GUI_EXPORT CDockWidget :
        public COverlayMessagesDockWidget,
        public CEnableForFramelessWindow
    {
        Q_OBJECT

    public:
        //! Set original title bar
        void setOriginalTitleBar();

        //! Set empty title bar (empty widget as title bar)
        void setEmptyTitleBar();

        //! Set null (nullptr) title bar
        void setNullTitleBarWidget();

        //! Window title backup
        const QString &windowTitleBackup() const { return m_windowTitleBackup; }

        //! If current window title is empty, use backup
        QString windowTitleOrBackup() const;

        //! Window title when window is docked
        bool showTitleWhenDocked() const { return m_windowTitleWhenDocked; }

        //! Selected when tabbed
        bool isSelected() const { return m_selected; }

        //! Is widget visible? Not to be confused with \sa QWidget::isVisbible
        //! \remarks Logical vsibility as in \sa QDockWidget::visibilityChanged
        bool isWidgetVisible() const;

        //! Allow a status bar to be displayed
        void allowStatusBar(bool allow) { m_allowStatusBar = allow; }

        //! Show the window title when docked
        void showTitleWhenDocked(bool show);

        //! Reset first time floating, marked as never floated before
        void resetWasAlreadyFloating();

        //! Was widget already floating?
        bool wasAlreadyFloating() const { return m_wasAlreadyFloating; }

        //! Size when floating first time
        void setPreferredSizeWhenFloating(const QSize &size);

        //! Position offset when floating first time
        void setOffsetWhenFloating(const QPoint &point, bool frameless);

        //! \copydoc CEnableForFramelessWindow::setFrameless
        virtual void setFrameless(bool frameless) override;

        //! Toggle floating
        void toggleFloating();

        //! Toggle visibility
        void toggleVisibility();

        //! Toggle frameless mode
        void toggleFrameless();

        //! Toggle frameless deferred
        // void toggleFramelessDeferred(int delayMs = 1000);

        //! Window always on top
        void windowAlwaysOnTop();

        //! Window not on top
        void windowNotAlwaysOnTop();

        //! Set always on top
        void setAlwaysOnTop(bool onTop) { m_alwaysOnTop = onTop; }

        //! Restore from settings
        bool restoreFromSettings();

        //! Remember widget state
        void saveCurrentStateToSettings();

        //! Reset margin settings
        void resetSettings();

        //! Reset window position
        void resetPosition();

        //! Set title and internally keep a backup
        void setWindowTitle(const QString &title);

        //! Display status message
        void displayStatusMessage(const swift::misc::CStatusMessage &statusMessage);

        //! Display status messages
        void displayStatusMessages(const swift::misc::CStatusMessageList &statusMessages);

        //! Value for dynamic property "dockwidget"
        static const QString &propertyOuterWidget();

        //! Value for dynamic property "dockwidget"
        static const QString &propertyInnerWidget();

    signals:
        //! Top level has changed for given widget
        void widgetTopLevelChanged(CDockWidget *, bool topLevel);

        //! @{
        //! Font size signals
        void fontSizePlus();
        void fontSizeMinus();
        //! @}

    protected:
        //! Constructor
        explicit CDockWidget(QWidget *parent = nullptr);

        //! Constructor
        CDockWidget(bool allowStatusBar, QWidget *parent = nullptr);

        //! Set margins from settings
        void setMargins();

        //! Margins when window is floating
        void setMarginsWhenFloating(const QMargins &margins);

        //! Margins when window is floating
        void setMarginsWhenFloating(int left, int top, int right, int bottom);

        //! Margins when floating
        QMargins getMarginsWhenFloating() const;

        //! Margins when window is floating (frameless)
        void setMarginsWhenFramelessFloating(const QMargins &margins);

        //! Margins when window is floating (frameless)
        void setMarginsWhenFramelessFloating(int left, int top, int right, int bottom);

        //! Margins when floating and frameless
        QMargins getMarginsWhenFramelessFloating() const;

        //! Margins when widget is floating
        void setMarginsWhenDocked(const QMargins &margins);

        //! Margins when widget is floating
        void setMarginsWhenDocked(int left, int top, int right, int bottom);

        //! Margins when docked
        QMargins getMarginsWhenDocked() const;

        //! Set the on top flag
        void setAlwaysOnTopFlag(bool onTop);

        //! \copydoc QWidget::closeEvent
        virtual void closeEvent(QCloseEvent *event) override;

        //! \copydoc QWidget::paintEvent
        virtual void paintEvent(QPaintEvent *event) override;

        //! \copydoc QWidget::mouseMoveEvent
        virtual void mouseMoveEvent(QMouseEvent *event) override;

        //! \copydoc QWidget::mouseMoveEvent
        virtual void keyPressEvent(QKeyEvent *event) override;

        //! \copydoc QWidget::mousePressEvent
        virtual void mousePressEvent(QMouseEvent *event) override;

        //! Contribute to menu
        virtual void addToContextMenu(QMenu *contextMenu) const;

        //! Widget is initialized by being a floating window for a shot period.
        //! \details Place where - when overidden - post initializations can take place.
        //! The GUI is already initialized, so all widget data are available.
        virtual void initialFloating();

        //! Style sheet has changed
        virtual void onStyleSheetsChanged();

    private:
        QWidget *m_titleBarWidgetEmpty = nullptr; //!< replacing default title bar
        QWidget *m_titleBarWidgetOriginal = nullptr; //!< the original title bar
        QWidgetAction *m_marginMenuAction = nullptr; //!< menu widget(!) action for margin widget
        components::CMarginsInput *m_input = nullptr; //!< margins widget
        menus::CFontMenu *m_fontMenu = nullptr; //!< Font menu
        bool m_allowStatusBar = true;
        bool m_windowTitleWhenDocked = true;
        bool m_wasAlreadyFloating = false; //!< flag if widget was at least once in floating state
        bool m_selected = false; //!< selected when tabbed
        bool m_dockWidgetVisible = false; //!< logical visible, not to be confused with QDockWidget::isVisible()
        bool m_wasFrameless = false; //!< frameless when last floating
        bool m_alwaysOnTop = false; //!< only effective if floating
        CManagedStatusBar m_statusBar; //!< status bar when floating
        QString m_windowTitleBackup; //!< original title, even if the widget title is deleted for layout purposes
        QSize m_preferredSizeWhenFloating; //!< preferred size when floating 1st time
        QSize m_initialDockedMinimumSize; //!< minimum size before first floating
        QSize m_lastFloatingSize; //!< last floating position
        QPoint m_offsetWhenFloating; //!< initial offset to main window when floating first time
        QPoint m_lastFloatingPosition; //!< last floating position
        Qt::DockWidgetAreas m_originalAreas; //!< areas before floating

        swift::misc::CSetting<swift::gui::settings::TDockWidget> m_settings { this, &CDockWidget::settingsChanged };

        //! Top level has been chaged
        void onTopLevelChanged(bool topLevel);

        //! Context menu
        void showContextMenu(const QPoint &pos);

        //! Visibility has changed
        void onVisibilityChanged(bool visible);

        //! Change margins
        void menuChangeMargins(const QMargins &margins);

        //! Changed settings
        void settingsChanged();

        //! Dummy slot for QAction
        void dummy();

        //! Empty widget with no size
        void initTitleBarWidgets();

        //! Init status bar
        void initStatusBarAndProperties();

        //! Force a style sheet update
        void forceStyleSheetUpdate();

        //! Size and position in floating mode
        void rememberFloatingSizeAndPosition();

        //! Size and position of floating window
        void restoreFloatingSizeAndPosition();

        //! Size and position of floating window
        void restoreFloatingSizeAndPositionDeferred();

        //! This widget`s settings
        settings::CDockWidgetSettings getSettings() const;

        //! Save settings
        void saveSettings(const swift::gui::settings::CDockWidgetSettings &settings);
    };
} // namespace swift::gui

#endif // guard
