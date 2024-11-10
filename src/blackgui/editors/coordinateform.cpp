// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "coordinateform.h"
#include "core/context/contextownaircraft.h"
#include "core/db/airportdatareader.h"
#include "core/webdataservices.h"
#include "blackgui/guiapplication.h"
#include "misc/aviation/airport.h"
#include "ui_coordinateform.h"

#include <QIntValidator>

using namespace BlackGui;
using namespace swift::misc;
using namespace swift::misc::geo;
using namespace swift::misc::aviation;
using namespace swift::misc::physical_quantities;

namespace BlackGui::Editors
{
    CCoordinateForm::CCoordinateForm(QWidget *parent) : CForm(parent),
                                                        ui(new Ui::CCoordinateForm)
    {
        ui->setupUi(this);

        ui->lblp_LatCheck->setToolTips("ok", "wrong format");
        ui->lblp_LngCheck->setToolTips("ok", "wrong format");
        ui->lblp_ElvCheck->setToolTips("ok", "wrong format");

        ui->le_LatDeg->setValidator(new QIntValidator(-90, 90, ui->le_LatDeg));
        ui->le_LatMin->setValidator(new QIntValidator(0, 60, ui->le_LatMin));
        ui->le_LatSec->setValidator(new QIntValidator(0, 60, ui->le_LatSec));
        ui->le_LatSecFrag->setValidator(new QIntValidator(0, 10E7, ui->le_LatSecFrag));

        ui->le_LngDeg->setValidator(new QIntValidator(-180, 180, ui->le_LngDeg));
        ui->le_LngMin->setValidator(new QIntValidator(0, 60, ui->le_LngMin));
        ui->le_LngSec->setValidator(new QIntValidator(0, 60, ui->le_LngSec));
        ui->le_LngSecFrag->setValidator(new QIntValidator(0, 10E7, ui->le_LngSecFrag));

        connect(ui->le_Latitude, &QLineEdit::editingFinished, this, &CCoordinateForm::latEntered);
        connect(ui->le_Longitude, &QLineEdit::editingFinished, this, &CCoordinateForm::lngEntered);
        connect(ui->le_Elevation, &QLineEdit::editingFinished, this, &CCoordinateForm::elvEntered);

        connect(ui->le_LatDeg, &QLineEdit::editingFinished, this, &CCoordinateForm::latCombinedEntered);
        connect(ui->le_LatMin, &QLineEdit::editingFinished, this, &CCoordinateForm::latCombinedEntered);
        connect(ui->le_LatSec, &QLineEdit::editingFinished, this, &CCoordinateForm::latCombinedEntered);
        connect(ui->le_LatSecFrag, &QLineEdit::editingFinished, this, &CCoordinateForm::latCombinedEntered);

        connect(ui->le_LngDeg, &QLineEdit::editingFinished, this, &CCoordinateForm::lngCombinedEntered);
        connect(ui->le_LngMin, &QLineEdit::editingFinished, this, &CCoordinateForm::lngCombinedEntered);
        connect(ui->le_LngSec, &QLineEdit::editingFinished, this, &CCoordinateForm::lngCombinedEntered);
        connect(ui->le_LngSecFrag, &QLineEdit::editingFinished, this, &CCoordinateForm::lngCombinedEntered);

        connect(ui->le_Location, &QLineEdit::returnPressed, this, &CCoordinateForm::locationEntered);

        connect(ui->pb_Set, &QPushButton::pressed, this, &CCoordinateForm::changedCoordinate);
        connect(ui->pb_OwnAircraft, &QPushButton::pressed, this, &CCoordinateForm::presetOwnAircraftPosition);

        const CCoordinateGeodetic c;
        this->setCoordinate(c);
    }

    CCoordinateForm::~CCoordinateForm()
    {}

    bool CCoordinateForm::setCoordinate(const ICoordinateGeodetic &coordinate)
    {
        if (coordinate == m_coordinate) { return false; }
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
        ui->le_LatSecFrag->setText(latParts.fractionalSecAsString(6));

        const CLongitude::DegMinSecFractionalSec lngParts = lng.asSexagesimalDegMinSec(true);
        ui->le_LngDeg->setText(lngParts.degAsString());
        ui->le_LngMin->setText(lngParts.minAsString());
        ui->le_LngSec->setText(lngParts.secAsString());
        ui->le_LngSecFrag->setText(lngParts.fractionalSecAsString(6));

        const QString elvString = coordinate.geodeticHeightAsString();
        ui->le_Elevation->setText(elvString);
        ui->lblp_ElvCheck->setTicked(!elvString.isEmpty());

        std::array<double, 3> v = coordinate.normalVectorDouble();
        ui->le_X->setText(QString::number(v[0]));
        ui->le_Y->setText(QString::number(v[1]));
        ui->le_Z->setText(QString::number(v[2]));
        ui->le_X->home(false);
        ui->le_Y->home(false);
        ui->le_Z->home(false);

        return true;
    }

    void CCoordinateForm::setReadOnly(bool readonly)
    {
        ui->le_Elevation->setReadOnly(readonly);
        ui->le_LatDeg->setReadOnly(readonly);
        ui->le_Latitude->setReadOnly(readonly);
        ui->le_LatMin->setReadOnly(readonly);
        ui->le_LatSec->setReadOnly(readonly);
        ui->le_LatSecFrag->setReadOnly(readonly);
        ui->le_LngDeg->setReadOnly(readonly);
        ui->le_LngMin->setReadOnly(readonly);
        ui->le_LngSec->setReadOnly(readonly);
        ui->le_LngSecFrag->setReadOnly(readonly);
        ui->le_Longitude->setReadOnly(readonly);

        ui->rb_E->setEnabled(!readonly);
        ui->rb_N->setEnabled(!readonly);
        ui->rb_S->setEnabled(!readonly);
        ui->rb_W->setEnabled(!readonly);

        ui->le_Location->setReadOnly(readonly);
        ui->le_Location->setVisible(!readonly); // does not make sense to show it in ro, no reverse lookup
        ui->lbl_Location->setVisible(!readonly);

        this->forceStyleSheetUpdate();
    }

    CStatusMessageList CCoordinateForm::validate(bool nested) const
    {
        // not implemented
        Q_UNUSED(nested);
        CStatusMessageList ml;
        return ml;
    }

    void CCoordinateForm::showSetButton(bool visible)
    {
        ui->pb_Set->setVisible(visible);
    }

    void CCoordinateForm::showElevation(bool show)
    {
        ui->le_Elevation->setVisible(show);
        ui->lbl_Elevation->setVisible(show);
        ui->lblp_ElvCheck->setVisible(show);
        m_coordinate.setGeodeticHeightToNull();
    }

    void CCoordinateForm::locationEntered()
    {
        const QString l = ui->le_Location->text().trimmed().simplified().toUpper();

        // location based on swift data
        if (sApp && sApp->hasWebDataServices())
        {
            CAirport airport;

            // airport ICAO
            if (l.length() == 4 && sApp->getWebDataServices()->getAirportsCount() > 0)
            {
                airport = sApp->getWebDataServices()->getAirportForIcaoDesignator(l);
                if (airport.hasValidDbKey())
                {
                    this->setCoordinate(airport);
                    return;
                }
            }

            airport = sApp->getWebDataServices()->getAirportForNameOrLocation(l);
            if (airport.hasValidDbKey())
            {
                this->setCoordinate(airport);
                return;
            }
        }
        // 33°59′42″S 150°57′06″E
        if (l.contains(' '))
        {
            QString lat, lng;
            const QStringList parts = l.split(' ');
            for (const QString &p : parts)
            {
                if (p.contains('S') || p.contains('N'))
                {
                    lat = p;
                }
                else if (p.contains('E') || p.contains('W'))
                {
                    lng = p;
                }
            }
            if (!lat.isEmpty() && !lng.isEmpty())
            {
                CCoordinateGeodetic c = m_coordinate;
                c.setLatLongFromWgs84(lat, lng);
                this->setCoordinate(c);
            }
        }
    }

    void CCoordinateForm::latEntered()
    {
        const QString ls = ui->le_Latitude->text();
        const CLatitude l = CLatitude::fromWgs84(ls);
        ui->lblp_LatCheck->setTicked(!l.isNull());
        CCoordinateGeodetic c = m_coordinate;
        c.setLatitude(l);
        this->setCoordinate(c);
    }

    void CCoordinateForm::latCombinedEntered()
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

    void CCoordinateForm::lngEntered()
    {
        const QString ls = ui->le_Longitude->text();
        const CLongitude l = CLongitude::fromWgs84(ls);
        ui->lblp_LatCheck->setTicked(!l.isNull());
        CCoordinateGeodetic c = m_coordinate;
        c.setLongitude(l);
        this->setCoordinate(c);
    }

    void CCoordinateForm::lngCombinedEntered()
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

    void CCoordinateForm::elvEntered()
    {
        const QString e = ui->le_Elevation->text().trimmed();
        CAltitude a;
        a.parseFromString(e, CPqString::SeparatorBestGuess);
        ui->lblp_ElvCheck->setTicked(!e.isNull());
        CCoordinateGeodetic c = m_coordinate;
        c.setGeodeticHeight(a);
        this->setCoordinate(c);
    }

    void CCoordinateForm::presetOwnAircraftPosition()
    {
        if (!sGui || sGui->isShuttingDown()) { return; }
        if (!sGui->getIContextOwnAircraft()) { return; }
        const CCoordinateGeodetic coordinate = sGui->getIContextOwnAircraft()->getOwnAircraft().getSituation();
        this->setCoordinate(coordinate);
    }
} // ns
