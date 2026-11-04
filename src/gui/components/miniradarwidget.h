// SPDX-FileCopyrightText: Copyright (C) 2026 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_MINIRADARWIDGET_H
#define SWIFT_GUI_COMPONENTS_MINIRADARWIDGET_H

#include <QFont>
#include <QFrame>
#include <QGraphicsItemGroup>
#include <QGraphicsScene>
#include <QPen>
#include <QScopedPointer>
#include <QTimer>

#include "gui/swiftguiexport.h"

namespace Ui
{
    class CRadarComponent;
}
namespace swift::gui::components
{
    /*!
     * Stark vereinfachte Radaransicht (zeigt Flugzeuge in Reichweite).
     * Im Gegensatz zu CRadarComponent komplett ohne Abhaengigkeit
     * zu CDockWidgetInfoArea / CInfoArea - dadurch direkt in jedem
     * beliebigen Fenster (auch ohne Infoarea-Infrastruktur) nutzbar.
     * Nutzt dieselbe .ui Oberflaeche wie CRadarComponent.
     * \remark benoetigt sGui mit gestartetem IContextNetwork/IContextOwnAircraft
     */
    class SWIFT_GUI_EXPORT CMiniRadarWidget : public QFrame
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CMiniRadarWidget(QWidget *parent = nullptr);

        //! Destructor
        ~CMiniRadarWidget() override;

    private:
        void prepareScene();
        void addCenter();
        void addGraticules();
        void addRadials();

        void refreshTargets();
        void rotateView();

        void toggleGrid(bool checked);

        void fitInView();
        void changeRangeInSteps(bool zoomIn);
        void changeRangeFromUserSelection(int index);
        void updateFont(int pointSize);

        static QPointF polarPoint(double distance, double angleRadians);

        QScopedPointer<Ui::CRadarComponent> ui;
        QGraphicsScene m_scene;
        QGraphicsItemGroup m_radarTargets;
        QGraphicsItemGroup m_center;
        QGraphicsItemGroup m_macroGraticule;
        QGraphicsItemGroup m_microGraticule;
        QGraphicsItemGroup m_radials;

        QPen m_radarTargetPen { Qt::green, 1 };
        qreal m_rangeNM = 10.0;
        int m_rotatenAngle = 0;
        QTimer m_updateTimer;
        QTimer m_headingTimer;
        QFont m_tagFont;
    };
} // namespace swift::gui::components

#endif // SWIFT_GUI_COMPONENTS_MINIRADARWIDGET_H