// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

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
