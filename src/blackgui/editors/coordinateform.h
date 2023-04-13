/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_EDITORS_COORDINATEFORM_H
#define BLACKGUI_EDITORS_COORDINATEFORM_H

#include "blackgui/editors/form.h"
#include "blackgui/blackguiexport.h"
#include "blackmisc/geo/coordinategeodetic.h"
#include "blackmisc/statusmessagelist.h"

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
        BlackMisc::Geo::CCoordinateGeodetic getCoordinate() const { return m_coordinate; }

        //! Set the coordinate
        bool setCoordinate(const BlackMisc::Geo::ICoordinateGeodetic &coordinate);

        //! \name Form class implementations
        //! @{
        virtual void setReadOnly(bool readonly) override;
        virtual BlackMisc::CStatusMessageList validate(bool nested = false) const override;
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

        BlackMisc::Geo::CCoordinateGeodetic m_coordinate;
    };
} // ns
#endif // guard
