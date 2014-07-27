/* Copyright (C) 2014
 * Swift Project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of Swift Project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef BLACKGUI_DOCKWIDGETINFOAREA_H
#define BLACKGUI_DOCKWIDGETINFOAREA_H

//! \file

#include "blackgui/dockwidget.h"

namespace BlackGui
{

    //! Class for dock widgets in the info area, containing some specialized functionality
    class CDockWidgetInfoArea : public CDockWidget
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CDockWidgetInfoArea(QWidget *parent = nullptr);

    protected:
        //! Contribute to menu
        virtual void addToContextMenu(QMenu *contextMenu) const override;

    };

} // namespace

#endif // guard
