// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_EDITORS_RELATIVEAIRCRAFTPOSITION_H
#define BLACKGUI_EDITORS_RELATIVEAIRCRAFTPOSITION_H

#include <QFrame>
#include <QScopedPointer>

#include "misc/geo/coordinategeodetic.h"
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
        void setOriginCoordinate(const swift::misc::geo::CCoordinateGeodetic &originCoordinate) { m_originCoordinate = originCoordinate; }

        //! Set the distance
        void setDistance(const swift::misc::physical_quantities::CLength &distance);

        //! Relative coordinate based on entered data
        swift::misc::geo::CCoordinateGeodetic getRelativeCoordinate(const swift::misc::physical_quantities::CAngle &bearingOffset = swift::misc::physical_quantities::CAngle::null()) const;

        //! Display coordinate info
        void displayInfo(const swift::misc::geo::CCoordinateGeodetic &relPos = swift::misc::geo::CCoordinateGeodetic::null());

    private:
        QScopedPointer<Ui::CRelativeAircraftPosition> ui;
        swift::misc::geo::CCoordinateGeodetic m_originCoordinate;
    };
} // ns

#endif // guard
