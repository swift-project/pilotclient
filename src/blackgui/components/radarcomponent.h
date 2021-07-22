/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_RADARCOMPONENT_H
#define BLACKGUI_COMPONENTS_RADARCOMPONENT_H

#include "blackgui/enablefordockwidgetinfoarea.h"
#include "blackgui/blackguiexport.h"
#include "blackcore/actionbind.h"
#include "blackmisc/input/actionhotkeydefs.h"

#include <QGraphicsScene>
#include <QGraphicsItemGroup>
#include <QFrame>
#include <QScopedPointer>
#include <QTimer>

namespace Ui { class CRadarComponent; }
namespace BlackGui
{
    namespace Components
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

            static QPointF polarPoint(double distance, double angleRadians);

            //! Info area tab bar has changed
            void onInfoAreaTabBarChanged(int index);

            QScopedPointer<Ui::CRadarComponent> ui;
            QGraphicsScene     m_scene;
            QGraphicsItemGroup m_radarTargets;
            QGraphicsItemGroup m_center;
            QGraphicsItemGroup m_macroGraticule;
            QGraphicsItemGroup m_microGraticule;
            QGraphicsItemGroup m_radials;

            QPen m_radarTargetPen = { Qt::green, 1 };
            qreal  m_rangeNM      = 10.0;
            int    m_rotatenAngle = 0;
            QTimer m_updateTimer;
            QTimer m_headingTimer;

            BlackCore::CActionBind m_actionZoomIn  { BlackMisc::Input::radarZoomInHotkeyAction(),  BlackMisc::Input::radarZoomInHotkeyIcon(),  this, &CRadarComponent::rangeZoomIn };
            BlackCore::CActionBind m_actionZoomOut { BlackMisc::Input::radarZoomOutHotkeyAction(), BlackMisc::Input::radarZoomOutHotkeyIcon(), this, &CRadarComponent::rangeZoomOut };
            void rangeZoomIn (bool keydown) { if (keydown) { changeRangeInSteps(true); } }
            void rangeZoomOut(bool keydown) { if (keydown) { changeRangeInSteps(false); } }
        };
    } // ns
} // ns

#endif // guard
