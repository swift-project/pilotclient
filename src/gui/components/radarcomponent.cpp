// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/components/radarcomponent.h"

#include <QStringBuilder>
#include <QtMath>

#include "ui_radarcomponent.h"

#include "core/context/contextnetwork.h"
#include "core/context/contextownaircraft.h"
#include "gui/guiapplication.h"
#include "gui/infoarea.h"
#include "misc/simulation/simulatedaircraft.h"

using namespace swift::misc;
using namespace swift::misc::aviation;
using namespace swift::misc::simulation;
using namespace swift::misc::geo;
using namespace swift::misc::physical_quantities;
using namespace swift::gui::views;

namespace swift::gui::components
{
    CRadarComponent::CRadarComponent(QWidget *parent)
        : QFrame(parent), ui(new Ui::CRadarComponent), m_tagFont(QApplication::font())
    {
        ui->setupUi(this);

        ui->gv_RadarView->setScene(&m_scene);

        ui->cb_RadarRange->addItem(QString::number(0.5) % u" nm", 0.5);
        for (int r = 1; r <= 9; ++r) { ui->cb_RadarRange->addItem(QString::number(r) % u" nm", r); }
        for (int r = 10; r <= 90; r += 10) { ui->cb_RadarRange->addItem(QString::number(r) % u" nm", r); }

        ui->cb_RadarRange->setCurrentText(QString::number(m_rangeNM) % u" nm");
        ui->sb_FontSize->setRange(1, 100);
        ui->sb_FontSize->setValue(QApplication::font().pointSize());

        connect(ui->gv_RadarView, &CRadarView::radarViewResized, this, &CRadarComponent::fitInView);
        connect(ui->gv_RadarView, &CRadarView::zoomEvent, this, &CRadarComponent::changeRangeInSteps);
        connect(&m_updateTimer, &QTimer::timeout, this, &CRadarComponent::refreshTargets);
        connect(&m_headingTimer, &QTimer::timeout, this, &CRadarComponent::rotateView);

        connect(ui->cb_RadarRange, qOverload<int>(&QComboBox::currentIndexChanged), this,
                &CRadarComponent::changeRangeFromUserSelection);
        connect(ui->sb_FontSize, qOverload<int>(&QSpinBox::valueChanged), this, &CRadarComponent::updateFont);
        connect(ui->cb_Callsign, &QCheckBox::toggled, this, &CRadarComponent::refreshTargets);
        connect(ui->cb_Heading, &QCheckBox::toggled, this, &CRadarComponent::refreshTargets);
        connect(ui->cb_Altitude, &QCheckBox::toggled, this, &CRadarComponent::refreshTargets);
        connect(ui->cb_GroundSpeed, &QCheckBox::toggled, this, &CRadarComponent::refreshTargets);
        connect(ui->cb_Grid, &QCheckBox::toggled, this, &CRadarComponent::toggleGrid);

        prepareScene();

        m_updateTimer.start(5000);
        m_headingTimer.start(50);
    }

    CRadarComponent::~CRadarComponent() {}

    bool CRadarComponent::setParentDockWidgetInfoArea(CDockWidgetInfoArea *parentDockableWidget)
    {
        CEnableForDockWidgetInfoArea::setParentDockWidgetInfoArea(parentDockableWidget);
        const bool c = connect(this->getParentInfoArea(), &CInfoArea::changedInfoAreaTabBarIndex, this,
                               &CRadarComponent::onInfoAreaTabBarChanged);
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
        auto *lix = new QGraphicsLineItem { QLineF(-5.0, 0.0, 5.0, 0.0), &m_center };
        lix->setFlags(QGraphicsItem::ItemIgnoresTransformations);
        lix->setPen(pen);

        auto *liy = new QGraphicsLineItem(QLineF(0.0, -5.0, 0.0, 5.0), &m_center);
        liy->setFlags(QGraphicsItem::ItemIgnoresTransformations);
        liy->setPen(pen);
    }

    void CRadarComponent::addGraticules()
    {
        QPen pen(Qt::white, 1);
        pen.setCosmetic(true);

        // Macro graticule, drawn as full line at every 10 nm
        for (int range = 10; range <= 100; range += 10)
        {
            auto *circle = new QGraphicsEllipseItem(-range, -range, 2.0 * range, 2.0 * range, &m_macroGraticule);
            circle->setPen(pen);
        }
        pen = QPen(Qt::gray, 1, Qt::DashLine);
        pen.setCosmetic(true);

        // Micro graticule, drawn as dash line at every 2.5 nm
        for (qreal range = 1; range <= 3; ++range)
        {
            auto *circle =
                new QGraphicsEllipseItem(-range * 2.5, -range * 2.5, 5.0 * range, 5.0 * range, &m_microGraticule);
            circle->setPen(pen);
        }
    }

    void CRadarComponent::addRadials()
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

    void CRadarComponent::refreshTargets()
    {
        if (!sGui || sGui->isShuttingDown()) { return; }

        qDeleteAll(m_radarTargets.childItems());

        if (sGui->getIContextNetwork() && sGui->getIContextNetwork()->isConnected())
        {
            if (isVisibleWidget())
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
    }

    void CRadarComponent::rotateView()
    {
        if (sGui->getIContextOwnAircraft())
        {
            if (isVisibleWidget())
            {
                int headingDegree = 0;
                if (!ui->cb_LockNorth->isChecked())
                {
                    headingDegree = sGui->getIContextOwnAircraft()->getOwnAircraftSituation().getHeading().valueInteger(
                        CAngleUnit::deg());
                }

                if (m_rotatenAngle != headingDegree)
                {
                    // Rotations are summed up, hence rotate back before applying the new rotation.
                    // Doing a global transformation reset will not work as it resets also zooming.
                    ui->gv_RadarView->rotate(m_rotatenAngle);
                    ui->gv_RadarView->rotate(-headingDegree);
                    m_rotatenAngle = headingDegree;
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
        ui->gv_RadarView->fitInView(-m_rangeNM, -m_rangeNM, 2.0 * m_rangeNM, 2.0 * m_rangeNM, Qt::KeepAspectRatio);
    }

    void CRadarComponent::changeRangeInSteps(bool zoomIn)
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

    void CRadarComponent::changeRangeFromUserSelection(int index)
    {
        double range = ui->cb_RadarRange->itemData(index).toDouble();
        if (!qFuzzyCompare(m_rangeNM, range))
        {
            m_rangeNM = range;
            fitInView();
        }
    }

    void CRadarComponent::updateFont(int pointSize)
    {
        m_tagFont.setPointSize(pointSize);
        this->refreshTargets();
    }

    void CRadarComponent::onInfoAreaTabBarChanged(int index)
    {
        Q_UNUSED(index)

        // ignore in those cases
        if (!this->isVisibleWidget()) return;
        if (this->isParentDockWidgetFloating()) return;
        if (!sGui->getIContextNetwork()->isConnected()) return;

        // here I know I am the selected widget, update, but keep GUI responsive (hence I use a timer)
        QPointer<CRadarComponent> myself(this);
        QTimer::singleShot(1000, this, [=] {
            if (!myself) { return; }
            myself->refreshTargets();
        });
    }

    QPointF CRadarComponent::polarPoint(double distance, double angleRadians)
    {
        angleRadians = -angleRadians; // conversion assumes angles are counterclockwise

        // standard conversion from https://en.wikipedia.org/wiki/Polar_coordinate_system
        QPointF p(distance * qCos(angleRadians), distance * qSin(angleRadians));

        // conversion yields a coordinate system
        // in which North=(1,0) and East=(-1,0)
        // but we want North=(0,-1) and East=(0,1)
        // (QGraphicsView y axis increases downwards)
        std::swap(p.rx(), p.ry());
        p.setX(-p.x());
        p.setY(-p.y());
        return p;
    }
} // namespace swift::gui::components
