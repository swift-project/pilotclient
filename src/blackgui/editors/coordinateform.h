// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_EDITORS_COORDINATEFORM_H
#define BLACKGUI_EDITORS_COORDINATEFORM_H

#include "blackgui/editors/form.h"
#include "blackgui/blackguiexport.h"
#include "misc/geo/coordinategeodetic.h"
#include "misc/statusmessagelist.h"

namespace Ui
{
    class CCoordinateForm;
}
namespace BlackGui::Editors
{
    /*!
     * Select / enter a geo position
     */
    class BLACKGUI_EXPORT CCoordinateForm : public CForm
    {
        Q_OBJECT

    public:
        //! Ctor
        explicit CCoordinateForm(QWidget *parent = nullptr);

        //! Dtor
        virtual ~CCoordinateForm() override;

        //! Get the coordinate
        swift::misc::geo::CCoordinateGeodetic getCoordinate() const { return m_coordinate; }

        //! Set the coordinate
        bool setCoordinate(const swift::misc::geo::ICoordinateGeodetic &coordinate);

        //! \name Form class implementations
        //! @{
        virtual void setReadOnly(bool readonly) override;
        virtual swift::misc::CStatusMessageList validate(bool nested = false) const override;
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
} // ns
#endif // guard
