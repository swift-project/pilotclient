// SPDX-FileCopyrightText: Copyright (C) 2014 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_DOCKWIDGETINFOBAR_H
#define BLACKGUI_DOCKWIDGETINFOBAR_H

#include "blackgui/blackguiexport.h"
#include "blackgui/dockwidget.h"

#include <QObject>

class QMenu;

namespace BlackGui
{
    //! Class for dock widgets in the info area, containing some specialized functionality
    //! \sa CDockWidgetInfoArea
    //! \sa CInfoArea
    class BLACKGUI_EXPORT CDockWidgetInfoBar : public CDockWidget
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CDockWidgetInfoBar(QWidget *parent = nullptr);

        //! Force an reload
        void reloadStyleSheet() { this->onStyleSheetsChanged(); }

        //! \copydoc CDockWidget::initialFloating
        //! \remark special case to allow init
        virtual void initialFloating() override { CDockWidget::initialFloating(); }

    protected:
        //! Contribute to menu
        virtual void addToContextMenu(QMenu *contextMenu) const override;

    protected:
        //! \copydoc CDockWidget::onStyleSheetsChanged
        virtual void onStyleSheetsChanged() override;
    };
} // namespace

#endif // guard
