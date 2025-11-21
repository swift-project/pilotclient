// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_EDITORS_RELATIVEAIRCRAFTPOSITION_H
#define SWIFT_GUI_EDITORS_RELATIVEAIRCRAFTPOSITION_H

#include <QFrame>
#include <QScopedPointer>

#include "gui/editors/form.h"
#include "misc/geo/coordinategeodetic.h"

namespace Ui
{
    class CRelativeAircraftPosition;
}
namespace swift::gui::editors
{
    //! Position relative to other aircraft
    class CRelativeAircraftPosition : public CForm
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CRelativeAircraftPosition(QWidget *parent = nullptr);

        //! Destructor
        ~CRelativeAircraftPosition() override;

        //! \copydoc CForm::setReadOnly
        void setReadOnly(bool readOnly) override;

        //! Set origin coordinate
        void setOriginCoordinate(const swift::misc::geo::CCoordinateGeodetic &originCoordinate)
        {
            m_originCoordinate = originCoordinate;
        }

        //! Set the distance
        void setDistance(const swift::misc::physical_quantities::CLength &distance);

        //! Relative coordinate based on entered data
        swift::misc::geo::CCoordinateGeodetic
        getRelativeCoordinate(const swift::misc::physical_quantities::CAngle &bearingOffset =
                                  swift::misc::physical_quantities::CAngle::null()) const;

        //! Display coordinate info
        void displayInfo(
            const swift::misc::geo::CCoordinateGeodetic &relPos = swift::misc::geo::CCoordinateGeodetic::null());

    private:
        QScopedPointer<Ui::CRelativeAircraftPosition> ui;
        swift::misc::geo::CCoordinateGeodetic m_originCoordinate;
    };
} // namespace swift::gui::editors

#endif // SWIFT_GUI_EDITORS_RELATIVEAIRCRAFTPOSITION_H
