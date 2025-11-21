// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_EDITORS_COORDINATEFORM_H
#define SWIFT_GUI_EDITORS_COORDINATEFORM_H

#include "gui/editors/form.h"
#include "gui/swiftguiexport.h"
#include "misc/geo/coordinategeodetic.h"
#include "misc/statusmessagelist.h"

namespace Ui
{
    class CCoordinateForm;
}
namespace swift::gui::editors
{
    /*!
     * Select / enter a geo position
     */
    class SWIFT_GUI_EXPORT CCoordinateForm : public CForm
    {
        Q_OBJECT

    public:
        //! Ctor
        explicit CCoordinateForm(QWidget *parent = nullptr);

        //! Dtor
        ~CCoordinateForm() override;

        //! Get the coordinate
        swift::misc::geo::CCoordinateGeodetic getCoordinate() const { return m_coordinate; }

        //! Set the coordinate
        bool setCoordinate(const swift::misc::geo::ICoordinateGeodetic &coordinate);

        //! \name Form class implementations
        //! @{
        void setReadOnly(bool readonly) override;
        swift::misc::CStatusMessageList validate(bool nested = false) const override;
        //! @}

        //! Set button visible
        void showSetButton(bool visible);

        //! Show elevation
        void showElevation(bool show);

    signals:
        //! Change coordinate
        void changedCoordinate();

    private:
        QScopedPointer<Ui::CCoordinateForm> ui;

        void locationEntered();
        void latEntered();
        void latCombinedEntered();
        void lngEntered();
        void lngCombinedEntered();
        void elvEntered();
        void presetOwnAircraftPosition();

        swift::misc::geo::CCoordinateGeodetic m_coordinate;
    };
} // namespace swift::gui::editors
#endif // SWIFT_GUI_EDITORS_COORDINATEFORM_H
