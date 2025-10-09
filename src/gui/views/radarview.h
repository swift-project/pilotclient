// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_RADARVIEW_H
#define SWIFT_GUI_RADARVIEW_H

#include <QGraphicsView>

#include "gui/swiftguiexport.h"

namespace swift::gui::views
{
    //! Radar view
    class SWIFT_GUI_EXPORT CRadarView : public QGraphicsView
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
        void resizeEvent(QResizeEvent *event) override;

        //! \copydoc QWidget::wheelEvent
        void wheelEvent(QWheelEvent *event) override;
    };
} // namespace swift::gui::views

#endif // SWIFT_GUI_RADARVIEW_H
