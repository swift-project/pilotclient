/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_RADARVIEW_H
#define BLACKGUI_RADARVIEW_H

#include "blackgui/blackguiexport.h"
#include <QGraphicsView>

namespace BlackGui::Views
{
    //! Radar view
    class BLACKGUI_EXPORT CRadarView : public QGraphicsView
    {
        Q_OBJECT

    public:
        //! Constructor
        CRadarView(QWidget *parent = nullptr);

    signals:
        //! Signal emitted when the view is resized
        void radarViewResized();

        //! Signal emitted when the user zoomed in our out
        void zoomEvent(bool zoomIn);

    protected:
        //! \copydoc QWidget::resizeEvent
        virtual void resizeEvent(QResizeEvent *event) override;

        //! \copydoc QWidget::wheelEvent
        virtual void wheelEvent(QWheelEvent *event) override;
    };
} // ns

#endif // guard
