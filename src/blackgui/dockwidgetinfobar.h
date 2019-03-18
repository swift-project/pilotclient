/* Copyright (C) 2014
 * Swift Project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

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

    protected:
        //! Contribute to menu
        virtual void addToContextMenu(QMenu *contextMenu) const override;

    protected:
        //! \copydoc CDockWidget::onStyleSheetsChanged
        virtual void onStyleSheetsChanged() override;
    };
} // namespace

#endif // guard
