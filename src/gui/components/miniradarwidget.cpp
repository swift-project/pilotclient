// SPDX-FileCopyrightText: Copyright (C) 2026 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "miniradarwidget.h"

#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
#include <QGraphicsTextItem>
#include <QSpinBox>
#include <QStringBuilder>
#include <QtMath>

#include "ui_radarcomponent.h"

#include "core/context/contextnetwork.h"
#include "core/context/contextownaircraft.h"
#include "gui/guiapplication.h"
#include "gui/views/radarview.h"
#include "misc/pq/angle.h"
#include "misc/pq/length.h"
#include "misc/pq/speed.h"
#include "misc/simulation/simulatedaircraftlist.h"

using namespace swift::core::context;
using namespace swift::misc::aviation;
using namespace swift::misc::physical_quantities;
using namespace swift::misc::simulation;
using namespace swift::gui::views;

namespace swift::gui::components
{
    CMiniRadarWidget::CMiniRadarWidget(QWidget *parent)
        : QFrame(parent), ui(new Ui::CRadarComponent), m_tagFont(QApplication::font())
    {
        Q_ASSERT_X(sGui, Q_FUNC_INFO, "Need sGui");
        ui->setupUi(this);

        ui->gv_RadarView->setScene(&m_scene);

        ui->cb_RadarRange->addItem(QString::number(0.5) % u" nm", 0.5);
        for (int r = 1; r <= 9; ++r) { ui->cb_RadarRange->addItem(QString::number(r) % u" nm", r); }
        for (int r = 10; r <= 90; r += 10) { ui->cb_RadarRange->addItem(QString::number(r) % u" nm", r); }

        ui->cb_RadarRange->setCurrentText(QString::number(m_rangeNM) % u" nm");
        ui->sb_FontSize->setRange(1, 100);
        ui->sb_FontSize->setValue(QApplication::font().pointSize());

        connect(ui->gv_RadarView, &CRadarView::radarViewResized, this, &CMiniRadarWidget::fitInView);
        connect(ui->gv_RadarView, &CRadarView::zoomEvent, this, &CMiniRadarWidget::changeRangeInSteps);
        connect(&m_updateTimer, &QTimer::timeout, this, &CMiniRadarWidget::refreshTargets);
        connect(&m_headingTimer, &QTimer::timeout, this, &CMiniRadarWidget::rotateView);

        connect(ui->cb_RadarRange, qOverload<int>(&QComboBox::currentIndexChanged), this,
                &CMiniRadarWidget::changeRangeFromUserSelection);
        connect(ui->sb_FontSize, qOverload<int>(&QSpinBox::valueChanged), this, &CMiniRadarWidget::updateFont);
        connect(ui->cb_Callsign, &QCheckBox::toggled, this, &CMiniRadarWidget::refreshTargets);
        connect(ui->cb_Heading, &QCheckBox::toggled, this, &CMiniRadarWidget::refreshTargets);
        connect(ui->cb_Altitude, &QCheckBox::toggled, this, &CMiniRadarWidget::refreshTargets);
        connect(ui->cb_GroundSpeed, &QCheckBox::toggled, this, &CMiniRadarWidget::refreshTargets);
        connect(ui->cb_Grid, &QCheckBox::toggled, this, &CMiniRadarWidget::toggleGrid);

        prepareScene();

        // keine Abhaengigkeit zu CDockWidgetInfoArea/CInfoArea:
        // kein isVisibleWidget() Check noetig, da wir immer direkt zeichnen
        m_updateTimer.start(5000);
        m_headingTimer.start(50);
    }

    CMiniRadarWidget::~CMiniRadarWidget() = default;

    void CMiniRadarWidget::prepareScene()
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

    void CMiniRadarWidget::addCenter()
    {
        QPen pen(Qt::white, 1);
        pen.setCosmetic(true);
        auto *lix = new QGraphicsLineItem { QLineF(-5.0, 0.0, 5.0, 0.0), &m_center };
        lix->setFlags(QGraphicsItem::ItemIgnoresTransformations);
        lix->setPen(pen);

        auto *liy = new QGraphicsLineItem(QLineF(0.0, -5.0, 0.0, 5.0), &m_center);
        liy->setFlags(QGraphicsItem::ItemIgnoresTransformations);
        liy->setPen(pen);
    }

    void CMiniRadarWidget::addGraticules()
    {
        QPen pen(Qt::white, 1);
        pen.setCosmetic(true);

        for (int range = 10; range <= 100; range += 10)
        {
            auto *circle = new QGraphicsEllipseItem(-range, -range, 2.0 * range, 2.0 * range, &m_macroGraticule);
            circle->setPen(pen);
        }
        pen = QPen(Qt::gray, 1, Qt::DashLine);
        pen.setCosmetic(true);

        for (qreal range = 1; range <= 3; ++range)
        {
            auto *circle =
                new QGraphicsEllipseItem(-range * 2.5, -range * 2.5, 5.0 * range, 5.0 * range, &m_microGraticule);
            circle->setPen(pen);
        }
    }

    void CMiniRadarWidget::addRadials()
    {
        QPen pen(Qt::gray, 1, Qt::DashDotDotLine);
        pen.setCosmetic(true);

        for (int angle = 0; angle < 360; angle += 30)
        {
            const QLineF line({ 0.0, 0.0 }, polarPoint(1000.0, qDegreesToRadians(static_cast<qreal>(angle))));
            auto *li = new QGraphicsLineItem(line, &m_radials);
            li->setFlags(QGraphicsItem::ItemIgnoresTransformations);
            li->setPen(pen);
        }
    }

    void CMiniRadarWidget::refreshTargets()
    {
        if (!sGui || sGui->isShuttingDown()) { return; }

        qDeleteAll(m_radarTargets.childItems());

        if (sGui->getIContextNetwork() && sGui->getIContextNetwork()->isConnected())
        {
            const CSimulatedAircraftList aircraft = sGui->getIContextNetwork()->getAircraftInRange();
            for (const CSimulatedAircraft &sa : aircraft)
            {
                const double distanceNM = sa.getRelativeDistance().value(CLengthUnit::NM());
                const double bearingRad = sa.getRelativeBearing().value(CAngleUnit::rad());
                const int groundSpeedKts = sa.getGroundSpeed().valueInteger(CSpeedUnit::kts());

                QPointF position(polarPoint(distanceNM, bearingRad));

                auto *dot = new QGraphicsEllipseItem(-2.0, -2.0, 4.0, 4.0, &m_radarTargets);
                dot->setPos(position);
                dot->setPen(m_radarTargetPen);
                dot->setBrush(m_radarTargetPen.color());
                dot->setFlags(QGraphicsItem::ItemIgnoresTransformations);

                auto *tag = new QGraphicsTextItem(&m_radarTargets);
                QString tagText;
                if (ui->cb_Callsign->isChecked()) { tagText += sa.getCallsignAsString() % u"\n"; }
                if (ui->cb_Altitude->isChecked())
                {
                    int flightLeveL = sa.getAltitude().valueInteger(CLengthUnit::ft()) / 100;
                    tagText += u"FL" % QStringLiteral("%1").arg(flightLeveL, 3, 10, QChar('0'));
                }
                if (ui->cb_GroundSpeed->isChecked())
                {
                    if (!tagText.isEmpty()) tagText += QStringLiteral(" ");
                    tagText += QString::number(groundSpeedKts) % u" kt";
                }

                tag->setPlainText(tagText);
                tag->setFont(m_tagFont);
                tag->setPos(position);
                tag->setDefaultTextColor(Qt::green);
                tag->setFlags(QGraphicsItem::ItemIgnoresTransformations);

                if (ui->cb_Heading->isChecked() && groundSpeedKts > 3.0)
                {
                    const double headingRad = sa.getHeading().value(CAngleUnit::rad());
                    QPen pen(Qt::green, 1);
                    pen.setCosmetic(true);
                    auto *li =
                        new QGraphicsLineItem(QLineF({ 0.0, 0.0 }, polarPoint(5.0, headingRad)), &m_radarTargets);
                    li->setPos(position);
                    li->setPen(pen);
                }
            }
        }
    }

    void CMiniRadarWidget::rotateView()
    {
        if (!sGui || sGui->isShuttingDown()) { return; }
        if (sGui->getIContextOwnAircraft())
        {
            int headingDegree = 0;
            if (!ui->cb_LockNorth->isChecked())
            {
                headingDegree = sGui->getIContextOwnAircraft()->getOwnAircraftSituation().getHeading().valueInteger(
                    CAngleUnit::deg());
            }

            if (m_rotatenAngle != headingDegree)
            {
                ui->gv_RadarView->rotate(m_rotatenAngle);
                ui->gv_RadarView->rotate(-headingDegree);
                m_rotatenAngle = headingDegree;
            }
        }
    }

    void CMiniRadarWidget::toggleGrid(bool checked)
    {
        m_macroGraticule.setVisible(checked);
        m_microGraticule.setVisible(checked);
        m_radials.setVisible(checked);
    }

    void CMiniRadarWidget::fitInView()
    {
        ui->gv_RadarView->fitInView(-m_rangeNM, -m_rangeNM, 2.0 * m_rangeNM, 2.0 * m_rangeNM, Qt::KeepAspectRatio);
    }

    void CMiniRadarWidget::changeRangeInSteps(bool zoomIn)
    {
        qreal direction = zoomIn ? 1.0 : -1.0;
        double factor = 10.0;
        if (m_rangeNM < 10.0 || (qFuzzyCompare(m_rangeNM, 10.0) && zoomIn)) { factor = 1.0; }
        if (m_rangeNM < 1.0 || (qFuzzyCompare(m_rangeNM, 1.0) && zoomIn)) { factor = 0.5; }

        m_rangeNM = m_rangeNM - direction * factor;
        m_rangeNM = qMin(90.0, qMax(0.5, m_rangeNM));
        ui->cb_RadarRange->setCurrentText(QString::number(m_rangeNM) % u" nm");
        fitInView();
    }

    void CMiniRadarWidget::changeRangeFromUserSelection(int index)
    {
        double range = ui->cb_RadarRange->itemData(index).toDouble();
        if (!qFuzzyCompare(m_rangeNM, range))
        {
            m_rangeNM = range;
            fitInView();
        }
    }

    void CMiniRadarWidget::updateFont(int pointSize)
    {
        m_tagFont.setPointSize(pointSize);
        this->refreshTargets();
    }

    QPointF CMiniRadarWidget::polarPoint(double distance, double angleRadians)
    {
        return { distance * qSin(angleRadians), -distance * qCos(angleRadians) };
    }
} // namespace swift::gui::components