// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "relativeaircraftposition.h"
#include "ui_relativeaircraftposition.h"

using namespace swift::misc::geo;
using namespace swift::misc::physical_quantities;

namespace BlackGui::Editors
{
    CRelativeAircraftPosition::CRelativeAircraftPosition(QWidget *parent) : CForm(parent),
                                                                            ui(new Ui::CRelativeAircraftPosition)
    {
        ui->setupUi(this);
    }

    CRelativeAircraftPosition::~CRelativeAircraftPosition()
    {}

    void CRelativeAircraftPosition::setReadOnly(bool readOnly)
    {
        ui->sb_Distance->setReadOnly(readOnly);
        ui->sb_RelativeBearing->setReadOnly(readOnly);
    }

    void CRelativeAircraftPosition::setDistance(const CLength &distance)
    {
        const int distMeters = distance.valueInteger(CLengthUnit::m());
        ui->sb_Distance->setValue(distMeters);
    }

    CCoordinateGeodetic CRelativeAircraftPosition::getRelativeCoordinate(const CAngle &bearingOffset) const
    {
        if (m_originCoordinate.isNull()) { return CCoordinateGeodetic::null(); }
        const CLength distance(ui->sb_Distance->value(), CLengthUnit::m());
        CAngle relBearing(ui->sb_RelativeBearing->value(), CAngleUnit::deg());
        if (!bearingOffset.isNull()) { relBearing += bearingOffset; }
        return m_originCoordinate.calculatePosition(distance, relBearing);
    }

    void CRelativeAircraftPosition::displayInfo(const CCoordinateGeodetic &relPos)
    {
        const CCoordinateGeodetic p = relPos.isNull() ? this->getRelativeCoordinate() : relPos;
        ui->le_Info->setText(
            QStringLiteral("%1 / %2").arg(m_originCoordinate.toQString(true), p.toQString(true)));
    }
} // ns
