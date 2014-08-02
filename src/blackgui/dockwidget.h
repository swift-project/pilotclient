/* Copyright (C) 2014
 * Swift Project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of Swift Project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_DOCKWIDGET_H
#define BLACKGUI_DOCKWIDGET_H

#include <QDockWidget>
#include <QTabWidget>
#include <QMenu>
#include <QLabel>

namespace BlackGui
{

    //! Our base class for dock widgets, containing some specialized functionality
    class CDockWidget : public QDockWidget
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CDockWidget(QWidget *parent = nullptr);

        //! Set original title bar
        void setOriginalTitleBar();

        //! Set empty title bar
        void setEmptyTitleBar();

        //! Set null title bar
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

        //! Show the window title when docked
        void showTitleWhenDocked(bool show);



    public slots:
        //! Toggle floating
        void toggleFloating();

        //! Set title and internally keep a backup
        void setWindowTitle(const QString &title);

    signals:
        //! Top level has changed for given widget
        void widgetTopLevelChanged(CDockWidget *, bool topLevel);

    protected:
        //! Override close event
        virtual void closeEvent(QCloseEvent *event) override;

        //! Contribute to menu
        virtual void addToContextMenu(QMenu *contextMenu) const;

    private slots:
        //! Top level has been chaged
        virtual void ps_onTopLevelChanged(bool topLevel);

        //! Context menu
        void ps_showContextMenu(const QPoint &pos);

    private:
        QWidget *m_emptyTitleBar = nullptr; //!< replacing default title bar
        QWidget *m_titleBarOriginal = nullptr; //!< the original title bar
        QMargins m_marginsWhenFloating; //!< Offsets when window is floating
        QMargins m_marginsWhenDocked;   //!< Offsets when window is floating
        QString m_windowTitleBackup;    //!< original title, even if the widget title is deleted for layout purposes
        bool m_windowTitleWhenDocked = true;

        //! Empty widget with no size
        void initTitleBarWidgets();

    };

} // namespace

#endif // guard
