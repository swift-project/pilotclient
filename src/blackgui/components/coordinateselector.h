/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_COORDINATESELECTOR_H
#define BLACKGUI_COMPONENTS_COORDINATESELECTOR_H

#include "blackmisc/geo/coordinategeodetic.h"
#include "blackgui/blackguiexport.h"
#include <QFrame>

namespace Ui { class CCoordinateSelector; }
namespace BlackGui
{
    namespace Components
    {
        /**
         * Select / enter a geo position
         */
        class BLACKGUI_EXPORT CCoordinateSelector : public QFrame
        {
            Q_OBJECT

        public:
            //! Ctor
            explicit CCoordinateSelector(QWidget *parent = nullptr);

            //! Dtor
            virtual ~CCoordinateSelector();

            //! Get the coordinate
            BlackMisc::Geo::CCoordinateGeodetic getCoordinate() const { return m_coordinate; }

            //! Set the coordinate
            void setCoordinate(const BlackMisc::Geo::ICoordinateGeodetic &coordinate);

        private:
            QScopedPointer<Ui::CCoordinateSelector> ui;

            void locationEntered();
            void latEntered();
            void latCombinedEntered();
            void lngEntered();
            void lngCombinedEntered();
            void elvEntered();

            BlackMisc::Geo::CCoordinateGeodetic m_coordinate;
        };
    } // ns
} // ns
#endif // guard
