/* Copyright (C) 2014
 * Swift Project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_DOCKWIDGETINFOAREA_H
#define BLACKGUI_DOCKWIDGETINFOAREA_H

#include "blackgui/dockwidget.h"

namespace BlackGui
{
    //! Forward declarations
    class CInfoArea;
    namespace Components { class CDockWidgetInfoAreaComponent; }

    //! Specialized class for dock widgets serving as info area
    //! \sa CDockWidgetInfoBar
    //! \sa CInfoArea
    class CDockWidgetInfoArea : public CDockWidget
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CDockWidgetInfoArea(QWidget *parent = nullptr);

        //! The parent info area
        const CInfoArea *getParentInfoArea() const;

        //! The parent info area
        CInfoArea *getParentInfoArea();

        //! Is this the selected widget, means it is not floating, and the one selected
        bool isSelectedDockWidget() const;

        //! Visible widget
        bool isVisibleWidget() const;

    protected:
        //! Contribute to menu
        virtual void addToContextMenu(QMenu *contextMenu) const override;

        //! \copydoc CDockWidget::initalFloating
        virtual void initalFloating() override;

    private:
        //! Find all embedded runtime components
        QList<Components::CDockWidgetInfoAreaComponent *> findEmbeddedDockWidgetInfoAreaComponents() const;
    };

} // namespace

#endif // guard
