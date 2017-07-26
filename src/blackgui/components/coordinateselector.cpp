/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "coordinateselector.h"
#include "blackcore/webdataservices.h"
#include "blackcore/db/airportdatareader.h"
#include "blackgui/guiapplication.h"
#include "blackmisc/aviation/airport.h"
#include "ui_coordinateselector.h"

#include <QIntValidator>

using namespace BlackGui;
using namespace BlackMisc::Geo;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::PhysicalQuantities;

namespace BlackGui
{
    namespace Components
    {
        CCoordinateSelector::CCoordinateSelector(QWidget *parent) :
            QFrame(parent),
            ui(new Ui::CCoordinateSelector)
        {
            ui->setupUi(this);

            ui->lblp_LatCheck->setToolTips("ok", "wrong format");
            ui->lblp_LngCheck->setToolTips("ok", "wrong format");
            ui->lblp_AltCheck->setToolTips("ok", "wrong format");

            ui->le_LatDeg->setValidator(new QIntValidator(-90, 90, ui->le_LatDeg));
            ui->le_LatMin->setValidator(new QIntValidator(0, 60, ui->le_LatMin));
            ui->le_LatSec->setValidator(new QIntValidator(0, 60, ui->le_LatSec));
            ui->le_LatSecFrag->setValidator(new QIntValidator(0, 10E7, ui->le_LatSecFrag));

            ui->le_LngDeg->setValidator(new QIntValidator(-180, 180, ui->le_LngDeg));
            ui->le_LngMin->setValidator(new QIntValidator(0, 60, ui->le_LngMin));
            ui->le_LngSec->setValidator(new QIntValidator(0, 60, ui->le_LngSec));
            ui->le_LngSecFrag->setValidator(new QIntValidator(0, 10E7, ui->le_LngSecFrag));

            connect(ui->le_Latitude, &QLineEdit::editingFinished, this, &CCoordinateSelector::latEntered);
            connect(ui->le_Longitude, &QLineEdit::editingFinished, this, &CCoordinateSelector::lngEntered);
            connect(ui->le_Elevation, &QLineEdit::editingFinished, this, &CCoordinateSelector::elvEntered);

            connect(ui->le_LatDeg, &QLineEdit::editingFinished, this, &CCoordinateSelector::latCombinedEntered);
            connect(ui->le_LatMin, &QLineEdit::editingFinished, this, &CCoordinateSelector::latCombinedEntered);
            connect(ui->le_LatSec, &QLineEdit::editingFinished, this, &CCoordinateSelector::latCombinedEntered);
            connect(ui->le_LatSecFrag, &QLineEdit::editingFinished, this, &CCoordinateSelector::latCombinedEntered);

            connect(ui->le_LngDeg, &QLineEdit::editingFinished, this, &CCoordinateSelector::lngCombinedEntered);
            connect(ui->le_LngMin, &QLineEdit::editingFinished, this, &CCoordinateSelector::lngCombinedEntered);
            connect(ui->le_LngSec, &QLineEdit::editingFinished, this, &CCoordinateSelector::lngCombinedEntered);
            connect(ui->le_LngSecFrag, &QLineEdit::editingFinished, this, &CCoordinateSelector::lngCombinedEntered);

            connect(ui->le_Location, &QLineEdit::returnPressed, this, &CCoordinateSelector::locationEntered);

            const CCoordinateGeodetic c;
            this->setCoordinate(c);
        }

        CCoordinateSelector::~CCoordinateSelector()
        { }

        void CCoordinateSelector::setCoordinate(const ICoordinateGeodetic &coordinate)
        {
            m_coordinate = coordinate;

            const CLatitude lat = coordinate.latitude();
            const QString latWgs = lat.toWgs84();
            ui->le_Latitude->setText(latWgs);
            ui->le_Latitude->setToolTip(QString::number(lat.value(CAngleUnit::deg())));
            ui->lblp_LatCheck->setTicked(!lat.isNull());
            if (latWgs.contains('S'))
            {
                ui->rb_S->setChecked(true);
            }
            else
            {
                ui->rb_N->setChecked(true);
            }

            const CLongitude lng = coordinate.longitude();
            const QString lngWgs = lng.toWgs84();
            ui->le_Longitude->setText(lngWgs);
            ui->le_Longitude->setToolTip(QString::number(lng.value(CAngleUnit::deg())));
            ui->lblp_LngCheck->setTicked(!lng.isNull());
            if (lngWgs.contains('W'))
            {
                ui->rb_W->setChecked(true);
            }
            else
            {
                ui->rb_E->setChecked(true);
            }

            const CLatitude::DegMinSecFractionalSec latParts = lat.asSexagesimalDegMinSec(true);
            ui->le_LatDeg->setText(latParts.degAsString());
            ui->le_LatMin->setText(latParts.minAsString());
            ui->le_LatSec->setText(latParts.secAsString());
            ui->le_LatSecFrag->setText(latParts.fractionalSecAsString());

            const CLongitude::DegMinSecFractionalSec lngParts = lng.asSexagesimalDegMinSec(true);
            ui->le_LngDeg->setText(lngParts.degAsString());
            ui->le_LngMin->setText(lngParts.minAsString());
            ui->le_LngSec->setText(lngParts.secAsString());
            ui->le_LngSecFrag->setText(lngParts.fractionalSecAsString());

            ui->le_Elevation->setText(coordinate.geodeticHeightAsString());
        }

        void CCoordinateSelector::locationEntered()
        {
            const QString l = ui->le_Location->text().trimmed().toUpper();

            // location based on swift data
            if (sApp && sApp->hasWebDataServices())
            {
                // airport ICAO
                if (l.length() == 4 && sApp->getWebDataServices()->getAirportsCount() > 0)
                {
                    const CAirport airport = sApp->getWebDataServices()->getAirportForIcaoDesignator(l);
                    if (airport.hasValidDbKey())
                    {
                        this->setCoordinate(airport);
                        return;
                    }
                }
            }
        }

        void CCoordinateSelector::latEntered()
        {
            const QString ls = ui->le_Latitude->text();
            const CLatitude l = CLatitude::fromWgs84(ls);
            ui->lblp_LatCheck->setTicked(!l.isNull());
            CCoordinateGeodetic c = m_coordinate;
            c.setLatitude(l);
            this->setCoordinate(c);
        }

        void CCoordinateSelector::latCombinedEntered()
        {
            bool ok;
            int deg = ui->le_LatDeg->text().trimmed().toInt(&ok);
            if (!ok) return;

            int min = ui->le_LatMin->text().trimmed().toInt(&ok);
            if (!ok) return;

            const QString secStr = ui->le_LatSec->text().trimmed() + "." + ui->le_LatSecFrag->text().trimmed();
            double sec = secStr.toDouble(&ok);
            if (!ok) return;

            CAngle::unifySign(deg, min, sec);
            const CAngle a(deg, min, sec);
            CLatitude lat(a);
            lat.roundToEpsilon();
            CCoordinateGeodetic c = m_coordinate;
            c.setLatitude(lat);
            this->setCoordinate(c);
        }

        void CCoordinateSelector::lngEntered()
        {
            const QString ls = ui->le_Longitude->text();
            const CLongitude l = CLongitude::fromWgs84(ls);
            ui->lblp_LatCheck->setTicked(!l.isNull());
            CCoordinateGeodetic c = m_coordinate;
            c.setLongitude(l);
            this->setCoordinate(c);
        }

        void CCoordinateSelector::lngCombinedEntered()
        {
            bool ok;
            int deg = ui->le_LngDeg->text().trimmed().toInt(&ok);
            if (!ok) return;

            int min = ui->le_LngMin->text().trimmed().toInt(&ok);
            if (!ok) return;

            const QString secStr = ui->le_LngSec->text().trimmed() + "." + ui->le_LngSecFrag->text().trimmed();
            double sec = secStr.toDouble(&ok);
            if (!ok) return;

            CAngle::unifySign(deg, min, sec);
            const CAngle a(deg, min, sec);
            CLongitude lng(a);
            lng.roundToEpsilon();
            CCoordinateGeodetic c = m_coordinate;
            c.setLongitude(lng);
            this->setCoordinate(c);
        }

        void CCoordinateSelector::elvEntered()
        {
            const QString e = ui->le_Elevation->text();
            CAltitude a;
            a.parseFromString(e);
            ui->lblp_AltCheck->setTicked(!e.isNull());
            CCoordinateGeodetic c = m_coordinate;
            c.setGeodeticHeight(a);
            this->setCoordinate(c);
        }
    } // ns
} // ns
