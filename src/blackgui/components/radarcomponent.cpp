/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "ui_radarcomponent.h"
#include "blackgui/guiapplication.h"
#include "blackgui/infoarea.h"
#include "blackgui/components/radarcomponent.h"
#include "blackcore/context/contextnetwork.h"
#include "blackmisc/simulation/simulatedaircraft.h"

#include <QtMath>
#include <QStringBuilder>

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Geo;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackGui::Views;

namespace BlackGui
{
    namespace Components
    {
        CRadarComponent::CRadarComponent(QWidget *parent) :
            QFrame(parent),
            ui(new Ui::CRadarComponent)
        {
            ui->setupUi(this);

            ui->gv_RadarView->setScene(&m_scene);

            ui->cb_RadarRange->addItem(QString::number(0.5) + " nm", 0.5);
            for (qreal r = 1.0; r < 10; r += 1)
            {
                ui->cb_RadarRange->addItem(QString::number(r) + " nm", r);
            }
            for (qreal r = 10; r < 91; r += 10)
            {
                ui->cb_RadarRange->addItem(QString::number(r) + " nm", r);
            }

            ui->cb_RadarRange->setCurrentText(QString::number(m_range) + " nm");

            connect(ui->gv_RadarView, &CRadarView::radarViewResized, this, &CRadarComponent::fitInView);
            connect(ui->gv_RadarView, &CRadarView::zoomEvent, this, &CRadarComponent::changeRangeInSteps);
            connect(&m_updateTimer, &QTimer::timeout, this, &CRadarComponent::refreshTargets);

            connect(ui->cb_RadarRange, qOverload<int>(&QComboBox::currentIndexChanged), this, &CRadarComponent::changeRangeFromUserSelection);
            connect(ui->cb_Callsign, &QCheckBox::toggled, this, &CRadarComponent::refreshTargets);
            connect(ui->cb_Heading, &QCheckBox::toggled, this, &CRadarComponent::refreshTargets);
            connect(ui->cb_Altitude, &QCheckBox::toggled, this, &CRadarComponent::refreshTargets);
            connect(ui->cb_GroundSpeed, &QCheckBox::toggled, this, &CRadarComponent::refreshTargets);
            connect(ui->cb_Grid, &QCheckBox::toggled, this, &CRadarComponent::toggleGrid);

            prepareScene();

            m_updateTimer.start(5000);
        }

        CRadarComponent::~CRadarComponent()
        { }

        bool CRadarComponent::setParentDockWidgetInfoArea(CDockWidgetInfoArea *parentDockableWidget)
        {
            CEnableForDockWidgetInfoArea::setParentDockWidgetInfoArea(parentDockableWidget);
            const bool c = connect(this->getParentInfoArea(), &CInfoArea::changedInfoAreaTabBarIndex, this, &CRadarComponent::onInfoAreaTabBarChanged);
            Q_ASSERT_X(c, Q_FUNC_INFO, "failed connect");
            Q_ASSERT_X(parentDockableWidget, Q_FUNC_INFO, "missing parent");
            return c && parentDockableWidget;
        }

        void CRadarComponent::prepareScene()
        {
            m_scene.addItem(&m_center);
            m_scene.addItem(&m_macroGraticule);
            m_scene.addItem(&m_microGraticule);
            m_scene.addItem(&m_radials);
            m_scene.addItem(&m_radarTargets);
            m_radarTargetPen.setCosmetic(true);
            addCenter();
            addGraticules();
            addRadials();
        }

        void CRadarComponent::addCenter()
        {
            QPen pen(Qt::white, 1);
            pen.setCosmetic(true);
            QGraphicsLineItem *li = new QGraphicsLineItem{QLineF(-5.0, 0.0, 5.0, 0.0), &m_center};
            li->setFlags(QGraphicsItem::ItemIgnoresTransformations);
            li->setPen(pen);

            li = new QGraphicsLineItem(QLineF(0.0, -5.0, 0.0, 5.0), &m_center);
            li->setFlags(QGraphicsItem::ItemIgnoresTransformations);
            li->setPen(pen);
        }

        void CRadarComponent::addGraticules()
        {
            QPen pen(Qt::white, 1);
            pen.setCosmetic(true);

            // Macro graticule, drawn as full line at every 10 nm
            for (qreal range = 10.0; range < 101.0; range += 10.0)
            {
                qreal x = -range;
                qreal y = -range;
                qreal width = 2.0 * range;
                qreal height = 2.0 * range;
                QGraphicsEllipseItem *circle = new QGraphicsEllipseItem(x, y, width, height, &m_macroGraticule);
                circle->setPen(pen);
            }
            pen = QPen(Qt::gray, 1, Qt::DashLine);
            pen.setCosmetic(true);

            // Micro graticule, drawn as dash line at every 2.5 nm
            for (qreal range = 2.5; range < 9.9; range += 2.5)
            {
                qreal x = -range;
                qreal y = -range;
                qreal width = 2.0 * range;
                qreal height = 2.0 * range;
                QGraphicsEllipseItem *circle = new QGraphicsEllipseItem(x, y, width, height, &m_microGraticule);
                circle->setPen(pen);
            }
        }

        void CRadarComponent::addRadials()
        {
            QPen pen(Qt::gray, 1, Qt::DashDotDotLine);
            pen.setCosmetic(true);

            for (qreal angle = 0.0; angle < 359.0; angle += 30.0)
            {
                const QLineF line(0.0, 0.0, 100.0 * qCos(qDegreesToRadians(angle)), -100.0 * qSin(qDegreesToRadians(angle)));
                QGraphicsLineItem *li = new QGraphicsLineItem(line, &m_radials);
                li->setPen(pen);
            }
        }

        void CRadarComponent::refreshTargets()
        {
            if (!sGui || sGui->isShuttingDown()) { return; }

            qDeleteAll(m_radarTargets.childItems());

            if (sGui->getIContextNetwork()->isConnected())
            {
                if (isVisibleWidget())
                {
                    const CSimulatedAircraftList aircraft = sGui->getIContextNetwork()->getAircraftInRange();
                    for (const CSimulatedAircraft &sa : aircraft)
                    {
                        double distanceNM = sa.getRelativeDistance().value(CLengthUnit::NM());
                        double bearingRad = sa.getRelativeBearing().value(CAngleUnit::rad());
                        int groundSpeedKts = sa.getGroundSpeed().valueInteger(CSpeedUnit::kts());

                        QPointF position(distanceNM * qSin(bearingRad), -distanceNM * qCos(bearingRad));

                        QGraphicsEllipseItem *dot = new QGraphicsEllipseItem(-2.0, -2.0, 4.0, 4.0, &m_radarTargets);
                        dot->setPos(position);
                        dot->setPen(m_radarTargetPen);
                        dot->setBrush(m_radarTargetPen.color());
                        dot->setFlags(QGraphicsItem::ItemIgnoresTransformations);

                        QGraphicsTextItem *tag = new QGraphicsTextItem(&m_radarTargets);
                        QString tagText;
                        if (ui->cb_Callsign->isChecked())
                        {
                            tagText += sa.getCallsignAsString() % QStringLiteral("\n");
                        }
                        if (ui->cb_Altitude->isChecked())
                        {
                            int flightLeveL = sa.getAltitude().valueInteger(CLengthUnit::ft()) / 100;
                            tagText += "FL" % QString("%1").arg(flightLeveL, 3, 10, QChar('0'));
                        }
                        if (ui->cb_GroundSpeed->isChecked())
                        {
                            if (! tagText.isEmpty()) tagText += QStringLiteral(" ");

                            tagText += QString::number(groundSpeedKts) % " kt";
                        }

                        tag->setPlainText(tagText);
                        tag->setPos(position);
                        tag->setDefaultTextColor(Qt::green);
                        tag->setFlags(QGraphicsItem::ItemIgnoresTransformations);

                        if (ui->cb_Heading->isChecked() && groundSpeedKts > 3.0)
                        {
                            double headingRad = sa.getHeading().value(CAngleUnit::rad());
                            QPen pen(Qt::green, 1);
                            pen.setCosmetic(true);
                            QGraphicsLineItem *li = new QGraphicsLineItem(QLineF(0.0, 0.0, 20.0 * qSin(headingRad), -20.0 * qCos(headingRad)), dot);
                            li->setFlags(QGraphicsItem::ItemIgnoresTransformations);
                            li->setPen(pen);
                        }
                    }
                }
            }
        }

        void CRadarComponent::toggleGrid(bool checked)
        {
            m_macroGraticule.setVisible(checked);
            m_microGraticule.setVisible(checked);
            m_radials.setVisible(checked);
        }

        void CRadarComponent::fitInView()
        {
            ui->gv_RadarView->fitInView(-m_range, -m_range, 2.0 * m_range, 2.0 * m_range, Qt::KeepAspectRatio);
        }

        void CRadarComponent::changeRangeInSteps(bool zoomIn)
        {
            qreal direction = zoomIn ? 1.0 : -1.0;
            double factor = 10.0;
            if (m_range < 10.0 || (qFuzzyCompare(m_range, 10.0) && zoomIn))
            {
                factor = 1.0;
            }

            if (m_range < 1.0 || (qFuzzyCompare(m_range, 1.0) && zoomIn))
            {
                factor = 0.5;
            }

            m_range = m_range - direction * factor;
            m_range = qMin(90.0, qMax(0.5, m_range));
            ui->cb_RadarRange->setCurrentText(QString::number(m_range) + " nm");
            fitInView();
        }

        void CRadarComponent::changeRangeFromUserSelection(int index)
        {
            double range = ui->cb_RadarRange->itemData(index).toDouble();
            if (! qFuzzyCompare(m_range, range))
            {
                m_range = range;
                fitInView();
            }
        }

        void CRadarComponent::onInfoAreaTabBarChanged(int index)
        {
            Q_UNUSED(index);

            // ignore in those cases
            if (!this->isVisibleWidget()) return;
            if (this->isParentDockWidgetFloating()) return;
            if (!sGui->getIContextNetwork()->isConnected()) return;

            // here I know I am the selected widget, update, but keep GUI responsive (hence I use a timer)
            QPointer<CRadarComponent> myself(this);
            QTimer::singleShot(1000, this, [ = ]
            {
                if (!myself) { return; }
                myself->refreshTargets();
            });

        }

    }
} // namespace
