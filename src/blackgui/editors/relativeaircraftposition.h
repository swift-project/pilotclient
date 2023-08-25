// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_EDITORS_RELATIVEAIRCRAFTPOSITION_H
#define BLACKGUI_EDITORS_RELATIVEAIRCRAFTPOSITION_H

#include <QFrame>
#include <QScopedPointer>

#include "blackmisc/geo/coordinategeodetic.h"
#include "blackgui/editors/form.h"

namespace Ui
{
    class CRelativeAircraftPosition;
}
namespace BlackGui::Editors
{
    //! Position relative to other aircraft
    class CRelativeAircraftPosition : public CForm
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CRelativeAircraftPosition(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CRelativeAircraftPosition() override;

        //! \copydoc CForm::setReadOnly
        virtual void setReadOnly(bool readOnly) override;

        //! Set origin coordinate
        void setOriginCoordinate(const BlackMisc::Geo::CCoordinateGeodetic &originCoordinate) { m_originCoordinate = originCoordinate; }

        //! Set the distance
        void setDistance(const BlackMisc::PhysicalQuantities::CLength &distance);

        //! Relative coordinate based on entered data
        BlackMisc::Geo::CCoordinateGeodetic getRelativeCoordinate(const BlackMisc::PhysicalQuantities::CAngle &bearingOffset = BlackMisc::PhysicalQuantities::CAngle::null()) const;

        //! Display coordinate info
        void displayInfo(const BlackMisc::Geo::CCoordinateGeodetic &relPos = BlackMisc::Geo::CCoordinateGeodetic::null());

    private:
        QScopedPointer<Ui::CRelativeAircraftPosition> ui;
        BlackMisc::Geo::CCoordinateGeodetic m_originCoordinate;
    };
} // ns

#endif // guard
