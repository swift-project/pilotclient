/* Copyright (C) 2014
 * swift Project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_DOCKWIDGET_H
#define BLACKGUI_DOCKWIDGET_H

#include "components/runtimebasedcomponent.h"
#include <QDockWidget>
#include <QTabWidget>
#include <QMenu>
#include <QLabel>

namespace BlackGui
{

    //! \brief Our base class for dockable widgets containing some specialized functionality on top of QWidget.
    //! \details We currently use dockable widgets either as "info area" or "info bar" dockable widget.
    //! Info area widgets reside in an info are and represent a larger piece of information (e.g. all ATC stations, all aircrafts in range).
    //! An info bar is meant to be a small info window displaying information about status, menu state etc.
    //!
    //! \sa CInfoArea
    //! \sa CDockWidgetInfoArea
    //! \sa CDockWidgetInfoBar
    class CDockWidget : public QDockWidget
    {
        // KB TODO: Make this an interface, IDockWidget?
        Q_OBJECT

    public:
        //! Set original title bar
        void setOriginalTitleBar();

        //! Set empty title bar (empty widget as title bar)
        void setEmptyTitleBar();

        //! Set null (nullptr) title bar
        void setNullTitleBar();

        //! Margins when window is floating
        void setMarginsWhenFloating(const QMargins &margins) { this->m_marginsWhenFloating = margins; }

        //! Margins when window is floating
        void setMarginsWhenFloating(int left, int top, int right, int bottom) { this->m_marginsWhenFloating = QMargins(left, top, right, bottom); }

        //! Margins when widget is floating
        void setMarginsWhenDocked(const QMargins &margins) { this->m_marginsWhenDocked = margins; }

        //! Margins when widget is floating
        void setMarginsWhenDocked(int left, int top, int right, int bottom) { this->m_marginsWhenDocked = QMargins(left, top, right, bottom); }

        //! Window title backup
        const QString &windowTitleBackup() const { return this->m_windowTitleBackup; }

        //! Window title when window is docked
        bool showTitleWhenDocked() const { return this->m_windowTitleWhenDocked; }

        //! Selected when tabbed
        bool isSelected() const { return this->m_selected; }

        //! Show the window title when docked
        void showTitleWhenDocked(bool show);

        //! Reset first time floating, marked as never floated before
        void resetWasAlreadyFLoating() { this->m_wasAlreadyFloating = false; }

        //! Was widget already floating?
        bool wasAlreadyFloating() const { return this->m_wasAlreadyFloating; }

        //! Size when floating first time
        void setPreferredSizeWhenFloating(const QSize &size) { this->m_preferredSizeWhenFloating = size; }

        //! Position offset when floating first time
        void setOffsetWhenFloating(const QPoint &point) { this->m_offsetWhenFloating = point; }

    public slots:
        //! Toggle floating
        void toggleFloating();

        //! Set title and internally keep a backup
        void setWindowTitle(const QString &title);

    signals:
        //! Top level has changed for given widget
        void widgetTopLevelChanged(CDockWidget *, bool topLevel);

    protected:
        //! Constructor
        explicit CDockWidget(QWidget *parent = nullptr);

        //! Override close event
        virtual void closeEvent(QCloseEvent *event) override;

        //! Paint event
        virtual void paintEvent(QPaintEvent *event) override;

        //! Hide event
        void hideEvent(QHideEvent *event) override;

        //! Show event
        void showEvent(QShowEvent *event) override;

        //! Contribute to menu
        virtual void addToContextMenu(QMenu *contextMenu) const;

        //! Widget is initialized by being a floating window for a shot period.
        //! \details Place where - when overidden - post initializations can take place.
        //! The GUI is already initialized, so all widget data are available.
        virtual void initalFloating();

    protected slots:
        //! Style sheet has changed
        virtual void onStyleSheetsChanged();

    private slots:
        //! Top level has been chaged
        virtual void ps_onTopLevelChanged(bool topLevel);

        //! Context menu
        virtual void ps_showContextMenu(const QPoint &pos);

    private:
        QWidget *m_emptyTitleBar    = nullptr; //!< replacing default title bar
        QWidget *m_titleBarOriginal = nullptr; //!< the original title bar
        QMargins m_marginsWhenFloating;        //!< Offsets when window is floating
        QMargins m_marginsWhenDocked;          //!< Offsets when window is floating
        QString  m_windowTitleBackup;          //!< original title, even if the widget title is deleted for layout purposes
        QSize m_preferredSizeWhenFloating;     //!< preferred size men floating 1st time
        QPoint m_offsetWhenFloating;           //!< initial offset to main window when floating first time
        bool m_windowTitleWhenDocked = true;
        bool m_wasAlreadyFloating    = false;
        bool m_selected              = false;  //!< selected when tabbed


        //! Empty widget with no size
        void initTitleBarWidgets();

        //! Find all embedded runtime components
        QList<QWidget *> findEmbeddedRuntimeComponents() const;

    };

} // namespace

#endif // guard
