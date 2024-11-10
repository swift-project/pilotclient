// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_COMPONENTS_RADARCOMPONENT_H
#define BLACKGUI_COMPONENTS_RADARCOMPONENT_H

#include "blackgui/enablefordockwidgetinfoarea.h"
#include "blackgui/blackguiexport.h"
#include "blackcore/actionbind.h"
#include "misc/input/actionhotkeydefs.h"

#include <QGraphicsScene>
#include <QGraphicsItemGroup>
#include <QFrame>
#include <QScopedPointer>
#include <QTimer>

namespace Ui
{
    class CRadarComponent;
}
namespace BlackGui::Components
{
    //! GUI displaying a radar like view with aircrafts nearby
    class BLACKGUI_EXPORT CRadarComponent :
        public QFrame,
        public CEnableForDockWidgetInfoArea
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CRadarComponent(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CRadarComponent() override;

        //! \copydoc CEnableForDockWidgetInfoArea::setParentDockWidgetInfoArea
        virtual bool setParentDockWidgetInfoArea(BlackGui::CDockWidgetInfoArea *parentDockableWidget) override;

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

        //! Info area tab bar has changed
        void onInfoAreaTabBarChanged(int index);

        QScopedPointer<Ui::CRadarComponent> ui;
        QGraphicsScene m_scene;
        QGraphicsItemGroup m_radarTargets;
        QGraphicsItemGroup m_center;
        QGraphicsItemGroup m_macroGraticule;
        QGraphicsItemGroup m_microGraticule;
        QGraphicsItemGroup m_radials;

        QPen m_radarTargetPen = { Qt::green, 1 };
        qreal m_rangeNM = 10.0;
        int m_rotatenAngle = 0;
        QTimer m_updateTimer;
        QTimer m_headingTimer;

        QFont m_tagFont;

        BlackCore::CActionBind m_actionZoomIn { swift::misc::input::radarZoomInHotkeyAction(), swift::misc::input::radarZoomInHotkeyIcon(), this, &CRadarComponent::rangeZoomIn };
        BlackCore::CActionBind m_actionZoomOut { swift::misc::input::radarZoomOutHotkeyAction(), swift::misc::input::radarZoomOutHotkeyIcon(), this, &CRadarComponent::rangeZoomOut };
        void rangeZoomIn(bool keydown)
        {
            if (keydown) { changeRangeInSteps(true); }
        }
        void rangeZoomOut(bool keydown)
        {
            if (keydown) { changeRangeInSteps(false); }
        }
    };
} // ns

#endif // guard
