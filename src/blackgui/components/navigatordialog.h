// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_COMPONENTS_NAVIGATORDIALOG_H
#define BLACKGUI_COMPONENTS_NAVIGATORDIALOG_H

#include "blackgui/blackguiexport.h"
#include "blackgui/enableforframelesswindow.h"
#include "blackgui/settings/navigatorsettings.h"

#include <QDialog>
#include <QObject>
#include <QScopedPointer>
#include <QWidgetAction>
#include <QTimer>
#include <QGridLayout>
#include <QTimer>
#include <QPoint>

class QEvent;
class QMenu;
class QMouseEvent;
class QMainWindow;

namespace Ui
{
    class CNavigatorDialog;
}
namespace BlackGui::Components
{
    class CMarginsInput;

    /*!
     * Navigator dialog
     */
    class BLACKGUI_EXPORT CNavigatorDialog :
        public QDialog,
        public CEnableForFramelessWindow
    {
        Q_OBJECT

    public:
        //! Constructor
        CNavigatorDialog(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CNavigatorDialog() override;

        //! Navigator
        void buildNavigator(int columns);

        //! Called when dialog is closed
        virtual void reject() override;

        //! Toggle frameless mode
        void toggleFrameless();

        //! Visibility visibility
        void showNavigator(bool visible);

        //! Toggle visibility
        void toggleNavigatorVisibility();

        //! Restore from settings
        void restoreFromSettings();

        //! Save to settings
        void saveToSettings();

        //! The main window
        void setMainWindow(QMainWindow *window) { m_mainWindow = window; }

    signals:
        //! Navigator closed
        void navigatorClosed();

    protected:
        //! \name Base class events
        //! @{

        //! \copydoc QDialog::mouseMoveEvent
        virtual void mouseMoveEvent(QMouseEvent *event) override;

        //! \copydoc QDialog::mousePressEvent
        virtual void mousePressEvent(QMouseEvent *event) override;

        //! \copydoc QDialog::mouseReleaseEvent
        virtual void mouseReleaseEvent(QMouseEvent *event) override;

        //! \copydoc QDialog::changeEvent
        virtual void changeEvent(QEvent *evt) override;

        //! \copydoc CEnableForFramelessWindow::windowFlagsChanged
        virtual void windowFlagsChanged() override;

        //! \copydoc QDialog::paintEvent
        virtual void paintEvent(QPaintEvent *event) override;

        //! \copydoc QDialog::enterEvent
        virtual void enterEvent(QEnterEvent *event) override;
        //! @}

    private:
        //! Margins context menu
        void menuChangeMargins(const QMargins &margins);

        //! Change the layout
        void changeLayout();

        //! Dummy slot
        void dummyFunction();

        //! Context menu
        void showContextMenu(const QPoint &pos);

        //! Style sheet has changed
        void onStyleSheetsChanged();

        //! Changed settigs
        void onSettingsChanged();

        //! Insert own actions
        void insertOwnActions();

        //! Contribute to menu
        void addToContextMenu(QMenu *contextMenu) const;

        //! How many columns for given rows
        int columnsForRows(int rows);

        //! Get my own grid layout
        QGridLayout *myGridLayout() const;

        //! Adjust navigator size
        void adjustNavigatorSize(QGridLayout *layout = nullptr);

        //! On watchdog
        void onWatchdog();

        bool m_firstBuild = true;
        bool m_originalQuitOnLastWindow = false;
        int m_currentColumns = 1;
        QScopedPointer<Ui::CNavigatorDialog> ui;
        QMainWindow *m_mainWindow = nullptr;
        QWidgetAction *m_marginMenuAction = nullptr; //!< menu widget(!) action for margin widget
        CMarginsInput *m_input = nullptr; //!< margins widget
        QTimer m_watchdog; //!< navigator watchdog
        swift::misc::CSetting<BlackGui::Settings::TNavigator> m_settings { this, &CNavigatorDialog::onSettingsChanged };
    };
} // ns

#endif // guard
