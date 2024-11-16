// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/geo/coordinategeodetic.h"

#include <cmath>

#include <QStringBuilder>
#include <QtGlobal>

#include "misc/logmessage.h"
#include "misc/verify.h"

using namespace swift::misc::aviation;
using namespace swift::misc::physical_quantities;
using namespace swift::misc::math;

SWIFT_DEFINE_VALUEOBJECT_MIXINS(swift::misc::geo, CCoordinateGeodetic)

namespace swift::misc::geo
{
    ICoordinateGeodetic::~ICoordinateGeodetic() {}

    QString CCoordinateGeodetic::convertToQString(bool i18n) const
    {
        return ICoordinateGeodetic::convertToQString(i18n);
    }

    CCoordinateGeodetic CCoordinateGeodetic::fromWgs84(const QString &latitudeWgs84, const QString &longitudeWgs84,
                                                       const CAltitude &geodeticHeight)
    {
        const CLatitude lat = CLatitude::fromWgs84(latitudeWgs84);
        const CLongitude lon = CLongitude::fromWgs84(longitudeWgs84);
        return CCoordinateGeodetic(lat, lon, geodeticHeight);
    }

    const CCoordinateGeodetic &CCoordinateGeodetic::null()
    {
        static const CCoordinateGeodetic n;
        return n;
    }

    CLength calculateGreatCircleDistance(const ICoordinateGeodetic &coordinate1, const ICoordinateGeodetic &coordinate2)
    {
        if (coordinate1.isNull() || coordinate2.isNull()) { return CLength::null(); }
        // if (coordinate1.equalNormalVectorDouble(coordinate2)) { return CLength(0, CLengthUnit::defaultUnit()); }
        constexpr float earthRadiusMeters = 6371000.8f;

        const QVector3D v1 = coordinate1.normalVector();
        const QVector3D v2 = coordinate2.normalVector();
        Q_ASSERT_X(std::isfinite(v1.x()) && std::isfinite(v1.y()) && std::isfinite(v1.z()), Q_FUNC_INFO,
                   "Distance calculation: v1 non-finite argument");
        Q_ASSERT_X(std::isfinite(v2.x()) && std::isfinite(v2.y()) && std::isfinite(v2.z()), Q_FUNC_INFO,
                   "Distance calculation: v2 non-finite argument");

        const float d =
            earthRadiusMeters * std::atan2(QVector3D::crossProduct(v1, v2).length(), QVector3D::dotProduct(v1, v2));

        SWIFT_VERIFY_X(!std::isnan(d), Q_FUNC_INFO, "Distance calculation: NaN in result");
        if (std::isnan(d))
        {
            CLogMessage().debug(u"Distance calculation: NaN in result (given arguments %1 %2 %3; %4 %5 %6)")
                << static_cast<double>(v1.x()) << static_cast<double>(v1.y()) << static_cast<double>(v1.z())
                << static_cast<double>(v2.x()) << static_cast<double>(v2.y()) << static_cast<double>(v2.z());
            return CLength::null();
        }
        return { static_cast<double>(d), CLengthUnit::m() };
    }

    CAngle calculateBearing(const ICoordinateGeodetic &coordinate1, const ICoordinateGeodetic &coordinate2)
    {
        if (coordinate1.isNull() || coordinate2.isNull()) { return CAngle::null(); }
        // if (coordinate1.equalNormalVectorDouble(coordinate2)) { return CAngle(0, CAngleUnit::defaultUnit()); } //
        // null or 0?
        static const QVector3D northPole { 0, 0, 1 };
        const QVector3D c1 = QVector3D::crossProduct(coordinate1.normalVector(), coordinate2.normalVector());
        const QVector3D c2 = QVector3D::crossProduct(coordinate1.normalVector(), northPole);
        const QVector3D cross = QVector3D::crossProduct(c1, c2);
        const float sinTheta = std::copysign(cross.length(), QVector3D::dotProduct(cross, coordinate1.normalVector()));
        const float cosTheta = QVector3D::dotProduct(c1, c2);
        const float theta = std::atan2(sinTheta, cosTheta);
        return { static_cast<double>(theta), CAngleUnit::rad() };
    }

    double calculateEuclideanDistance(const ICoordinateGeodetic &coordinate1, const ICoordinateGeodetic &coordinate2)
    {
        return static_cast<double>(coordinate1.normalVector().distanceToPoint(coordinate2.normalVector()));
    }

    double calculateEuclideanDistanceSquared(const ICoordinateGeodetic &coordinate1,
                                             const ICoordinateGeodetic &coordinate2)
    {
        return static_cast<double>((coordinate1.normalVector() - coordinate2.normalVector()).lengthSquared());
    }

    bool ICoordinateGeodetic::equalNormalVectorDouble(const std::array<double, 3> &otherVector) const
    {
        static const double epsilon = std::numeric_limits<double>::epsilon();
        const std::array<double, 3> thisVector = this->normalVectorDouble();
        for (unsigned int i = 0; i < otherVector.size(); i++)
        {
            const double d = thisVector[i] - otherVector[i];
            if (qAbs(d) > epsilon) { return false; }
        }
        return true;
    }

    bool ICoordinateGeodetic::equalNormalVectorDouble(const ICoordinateGeodetic &otherCoordinate) const
    {
        return this->equalNormalVectorDouble(otherCoordinate.normalVectorDouble());
    }

    CLength ICoordinateGeodetic::calculateGreatCircleDistance(const ICoordinateGeodetic &otherCoordinate) const
    {
        return geo::calculateGreatCircleDistance((*this), otherCoordinate);
    }

    bool ICoordinateGeodetic::isWithinRange(const ICoordinateGeodetic &otherCoordinate, const CLength &range) const
    {
        if (range.isNull()) { return false; }
        const CLength distance = this->calculateGreatCircleDistance(otherCoordinate);
        if (distance.isNull()) { return false; }
        return distance <= range;
    }

    CAngle ICoordinateGeodetic::calculateBearing(const ICoordinateGeodetic &otherCoordinate) const
    {
        return geo::calculateBearing((*this), otherCoordinate);
    }

    bool ICoordinateGeodetic::canHandleIndex(CPropertyIndexRef index)
    {
        const int i = index.frontCasted<int>();
        return (i >= static_cast<int>(IndexLatitude)) && (i <= static_cast<int>(IndexNormalVector));
    }

    QVariant ICoordinateGeodetic::propertyByIndex(swift::misc::CPropertyIndexRef index) const
    {
        if (!index.isMyself())
        {
            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexLatitude: return this->latitude().propertyByIndex(index.copyFrontRemoved());
            case IndexLongitude: return this->longitude().propertyByIndex(index.copyFrontRemoved());
            case IndexLatitudeAsString: return QVariant(this->latitudeAsString());
            case IndexLongitudeAsString: return QVariant(this->longitudeAsString());
            case IndexGeodeticHeight: return this->geodeticHeight().propertyByIndex(index.copyFrontRemoved());
            case IndexGeodeticHeightAsString: return QVariant(this->geodeticHeightAsString());
            case IndexNormalVector: return QVariant::fromValue(this->normalVector());
            default: break;
            }
        }

        const QString m = QString("no property, index ").append(index.toQString());
        SWIFT_VERIFY_X(false, Q_FUNC_INFO, qUtf8Printable(m));
        return QVariant::fromValue(m);
    }

    int ICoordinateGeodetic::comparePropertyByIndex(CPropertyIndexRef index,
                                                    const ICoordinateGeodetic &compareValue) const
    {
        if (!index.isMyself())
        {
            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexLatitude:
                return this->latitude().comparePropertyByIndex(index.copyFrontRemoved(), compareValue.latitude());
            case IndexLongitude:
                return this->longitude().comparePropertyByIndex(index.copyFrontRemoved(), compareValue.longitude());
            case IndexLatitudeAsString: return this->latitudeAsString().compare(compareValue.latitudeAsString());
            case IndexLongitudeAsString: return this->longitudeAsString().compare(compareValue.longitudeAsString());
            case IndexGeodeticHeight:
                return this->geodeticHeight().comparePropertyByIndex(index.copyFrontRemoved(),
                                                                     compareValue.geodeticHeight());
            case IndexGeodeticHeightAsString:
                return this->geodeticHeightAsString().compare(compareValue.geodeticHeightAsString());
            default: break;
            }
        }

        const QString m = QString("no property, index ").append(index.toQString());
        SWIFT_VERIFY_X(false, Q_FUNC_INFO, qUtf8Printable(m));
        return 0;
    }

    QString ICoordinateGeodetic::convertToQString(bool i18n) const
    {
        const CLatitude lat = this->latitude();
        const CLongitude lng = this->longitude();
        return QStringLiteral("Geodetic: {%1/%2, %3/%4, %5}")
            .arg(lat.valueRoundedWithUnit(CAngleUnit::deg(), 6, i18n),
                 lat.valueRoundedWithUnit(CAngleUnit::rad(), 6, i18n),
                 lng.valueRoundedWithUnit(CAngleUnit::deg(), 6, i18n),
                 lng.valueRoundedWithUnit(CAngleUnit::rad(), 6, i18n),
                 this->geodeticHeight().valueRoundedWithUnit(CLengthUnit::ft(), 2, i18n));
    }

    bool ICoordinateGeodetic::isNaNVector() const
    {
        const QVector3D v = this->normalVector();
        return std::isnan(v.x()) || std::isnan(v.y()) || std::isnan(v.z());
    }

    bool ICoordinateGeodetic::isNaNVectorDouble() const
    {
        const std::array<double, 3> v = this->normalVectorDouble();
        return std::isnan(v[0]) || std::isnan(v[1]) || std::isnan(v[2]);
    }

    bool ICoordinateGeodetic::isInfVector() const
    {
        const QVector3D v = this->normalVector();
        return std::isinf(v.x()) || std::isinf(v.y()) || std::isinf(v.z());
    }

    bool ICoordinateGeodetic::isInfVectorDouble() const
    {
        const std::array<double, 3> v = this->normalVectorDouble();
        return std::isinf(v[0]) || std::isinf(v[1]) || std::isinf(v[2]);
    }

    bool ICoordinateGeodetic::isValidVectorRange() const
    {
        // inf is out of range, comparing nans is always false
        const std::array<double, 3> v = this->normalVectorDouble();
        return isValidVector(v);
    }

    bool ICoordinateGeodetic::isValidVector(const std::array<double, 3> &v)
    {
        constexpr double l = 1.00001; // because of interpolation
        return v[0] <= l && v[1] <= l && v[2] <= l && v[0] >= -l && v[1] >= -l && v[2] >= -l;
    }

    int CCoordinateGeodetic::clampVector()
    {
        int c = 0;
        // *INDENT-OFF*
        if (m_x < -1.0)
        {
            m_x = -1.0;
            c++;
        }
        else if (m_x > 1.0)
        {
            m_x = 1.0;
            c++;
        }
        if (m_y < -1.0)
        {
            m_y = -1.0;
            c++;
        }
        else if (m_y > 1.0)
        {
            m_y = 1.0;
            c++;
        }
        if (m_z < -1.0)
        {
            m_z = -1.0;
            c++;
        }
        else if (m_z > 1.0)
        {
            m_z = 1.0;
            c++;
        }
        // *INDENT-ON*
        return c;
    }

    QVariant CCoordinateGeodetic::propertyByIndex(swift::misc::CPropertyIndexRef index) const
    {
        if (index.isMyself()) { return QVariant::fromValue(*this); }
        return (ICoordinateGeodetic::canHandleIndex(index)) ? ICoordinateGeodetic::propertyByIndex(index) :
                                                              CValueObject::propertyByIndex(index);
    }

    void CCoordinateGeodetic::setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant)
    {
        if (index.isMyself())
        {
            (*this) = variant.value<CCoordinateGeodetic>();
            return;
        }
        const ICoordinateGeodetic::ColumnIndex i = index.frontCasted<ICoordinateGeodetic::ColumnIndex>();
        switch (i)
        {
        case IndexGeodeticHeight: m_geodeticHeight.setPropertyByIndex(index.copyFrontRemoved(), variant); break;
        case IndexLatitude: this->setLatitude(variant.value<CLatitude>()); break;
        case IndexLongitude: this->setLongitude(variant.value<CLongitude>()); break;
        case IndexLatitudeAsString: this->setLatitude(CLatitude::fromWgs84(variant.toString())); break;
        case IndexLongitudeAsString: this->setLongitude(CLongitude::fromWgs84(variant.toString())); break;
        case IndexGeodeticHeightAsString: m_geodeticHeight.parseFromString(variant.toString()); break;
        case IndexNormalVector: this->setNormalVector(variant.value<QVector3D>()); break;
        default: CValueObject::setPropertyByIndex(index, variant); break;
        }
    }

    int CCoordinateGeodetic::comparePropertyByIndex(CPropertyIndexRef index,
                                                    const CCoordinateGeodetic &compareValue) const
    {
        return ICoordinateGeodetic::canHandleIndex(index) ?
                   ICoordinateGeodetic::comparePropertyByIndex(index, compareValue) :
                   CValueObject::comparePropertyByIndex(index, compareValue);
    }

    CCoordinateGeodetic::CCoordinateGeodetic(const std::array<double, 3> &normalVector)
    {
        this->setNormalVector(normalVector);
    }

    CCoordinateGeodetic::CCoordinateGeodetic(const CLatitude &latitude, const CLongitude &longitude)
        : CCoordinateGeodetic(latitude, longitude, CAltitude::null())
    {
        // void
    }

    CCoordinateGeodetic::CCoordinateGeodetic(const CLatitude &latitude, const CLongitude &longitude,
                                             const CAltitude &geodeticHeight)
        : m_x(latitude.cos() * longitude.cos()), m_y(latitude.cos() * longitude.sin()), m_z(latitude.sin()),
          m_geodeticHeight(geodeticHeight)
    {}

    CCoordinateGeodetic::CCoordinateGeodetic(double latitudeDegrees, double longitudeDegrees)
        : CCoordinateGeodetic({ latitudeDegrees, CAngleUnit::deg() }, { longitudeDegrees, CAngleUnit::deg() },
                              { 0, nullptr })
    {}

    CCoordinateGeodetic::CCoordinateGeodetic(double latitudeDegrees, double longitudeDegrees, double heightFeet)
        : CCoordinateGeodetic({ latitudeDegrees, CAngleUnit::deg() }, { longitudeDegrees, CAngleUnit::deg() },
                              { heightFeet, CLengthUnit::ft() })
    {}

    CCoordinateGeodetic::CCoordinateGeodetic(const ICoordinateGeodetic &coordinate)
        : m_geodeticHeight(coordinate.geodeticHeight())
    {
        this->setNormalVector(coordinate.normalVectorDouble());
    }

    CCoordinateGeodetic CCoordinateGeodetic::calculatePosition(const CLength &distance, const CAngle &relBearing) const
    {
        if (this->isNull()) { return CCoordinateGeodetic::null(); }
        if (distance.isNull() || distance.isNegativeWithEpsilonConsidered() || relBearing.isNull())
        {
            return CCoordinateGeodetic::null();
        }
        if (distance.isZeroEpsilonConsidered()) { return *this; }

        // http://www.movable-type.co.uk/scripts/latlong.html#destPoint
        // https://stackoverflow.com/a/879531/356726
        // https://www.cosmocode.de/en/blog/gohr/2010-06/29-calculate-a-destination-coordinate-based-on-distance-and-bearing-in-php
        constexpr double earthRadiusMeters = 6371000.8;
        const double startLatRad = this->latitude().value(CAngleUnit::rad());
        const double startLngRad = this->longitude().value(CAngleUnit::rad());
        const double bearingRad = relBearing.value(CAngleUnit::rad());
        const double distRatio = distance.value(CLengthUnit::m()) / earthRadiusMeters;

        const double newLatRad =
            asin(sin(startLatRad) * cos(distRatio) + cos(startLatRad) * sin(distRatio) * cos(bearingRad));
        double newLngRad = 0;

        constexpr double epsilon = 1E-06;
        if (cos(newLatRad) == 0 || qAbs(cos(newLatRad)) < epsilon)
            newLngRad = startLngRad;
        else
        {
            // λ1 + Math.atan2(Math.sin(brng)*Math.sin(d/R)*Math.cos(φ1), Math.cos(d/R)-Math.sin(φ1)*Math.sin(φ2));
            newLngRad = startLngRad + atan2(sin(bearingRad) * sin(distRatio) * cos(startLatRad),
                                            cos(distRatio) - sin(startLatRad) * sin(newLatRad));
            newLngRad = fmod(newLngRad + 3 * M_PI, 2 * M_PI) - M_PI; // normalize +-180deg
        }

        CCoordinateGeodetic copy = *this;
        const CLatitude lat(newLatRad, CAngleUnit::rad());
        const CLongitude lng(newLngRad, CAngleUnit::rad());
        copy.setLatLong(lat, lng);
        return copy;
    }

    CLatitude CCoordinateGeodetic::latitude() const
    {
        return { std::atan2(m_z, std::hypot(m_x, m_y)), CAngleUnit::rad() };
    }

    CLongitude CCoordinateGeodetic::longitude() const
    {
        // in mathematics atan2 of 0,0 is undefined, with IEEE floating-point atan2(0,0) is either 0 or ±180°
        return { std::atan2(m_y, m_x), CAngleUnit::rad() };
    }

    QVector3D CCoordinateGeodetic::normalVector() const
    {
        return { static_cast<float>(m_x), static_cast<float>(m_y), static_cast<float>(m_z) };
    }

    std::array<double, 3> CCoordinateGeodetic::normalVectorDouble() const { return { { m_x, m_y, m_z } }; }

    void CCoordinateGeodetic::setLatitude(const CLatitude &latitude) { this->setLatLong(latitude, this->longitude()); }

    void CCoordinateGeodetic::setLatitudeFromWgs84(const QString &wgs) { this->setLatitude(CLatitude::fromWgs84(wgs)); }

    void CCoordinateGeodetic::setLongitude(const CLongitude &longitude)
    {
        this->setLatLong(this->latitude(), longitude);
    }

    void CCoordinateGeodetic::setLongitudeFromWgs84(const QString &wgs)
    {
        this->setLongitude(CLongitude::fromWgs84(wgs));
    }

    void CCoordinateGeodetic::setLatLong(const CLatitude &latitude, const CLongitude &longitude)
    {
        m_x = latitude.cos() * longitude.cos();
        m_y = latitude.cos() * longitude.sin();
        m_z = latitude.sin();
    }

    void CCoordinateGeodetic::setLatLongFromWgs84(const QString &latitude, const QString &longitude)
    {
        this->setLatitudeFromWgs84(latitude);
        this->setLongitudeFromWgs84(longitude);
    }

    void CCoordinateGeodetic::setGeodeticHeightToNull() { this->setGeodeticHeight(CAltitude::null()); }

    void CCoordinateGeodetic::setNormalVector(const std::array<double, 3> &normalVector)
    {
        Q_ASSERT_X(normalVector.size() == 3, Q_FUNC_INFO, "Wrong vector size");
        m_x = normalVector[0];
        m_y = normalVector[1];
        m_z = normalVector[2];
    }

    CCoordinateGeodetic &CCoordinateGeodetic::switchUnit(const CLengthUnit &unit)
    {
        m_geodeticHeight.switchUnit(unit);
        return *this;
    }

    CLength ICoordinateWithRelativePosition::calculcateAndUpdateRelativeDistance(const ICoordinateGeodetic &position)
    {
        m_relativeDistance = geo::calculateGreatCircleDistance(*this, position);
        return m_relativeDistance;
    }

    CLength
    ICoordinateWithRelativePosition::calculcateAndUpdateRelativeDistanceAndBearing(const ICoordinateGeodetic &position)
    {
        m_relativeDistance = geo::calculateGreatCircleDistance(*this, position);
        m_relativeBearing = geo::calculateBearing(*this, position);
        return m_relativeDistance;
    }

    QVariant ICoordinateWithRelativePosition::propertyByIndex(CPropertyIndexRef index) const
    {
        if (ICoordinateGeodetic::canHandleIndex(index)) { return ICoordinateGeodetic::propertyByIndex(index); }
        if (!index.isMyself())
        {
            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexRelativeBearing: return this->getRelativeBearing().propertyByIndex(index.copyFrontRemoved());
            case IndexRelativeDistance: return this->getRelativeDistance().propertyByIndex(index.copyFrontRemoved());
            default: break;
            }
        }
        const QString m = QString("no property, index ").append(index.toQString());
        SWIFT_VERIFY_X(false, Q_FUNC_INFO, qUtf8Printable(m));
        return QVariant::fromValue(m);
    }

    void ICoordinateWithRelativePosition::setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant)
    {
        if (ICoordinateGeodetic::canHandleIndex(index)) { return; }
        if (!index.isMyself())
        {
            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexRelativeBearing: m_relativeBearing.setPropertyByIndex(index.copyFrontRemoved(), variant); break;
            case IndexRelativeDistance: m_relativeDistance.setPropertyByIndex(index.copyFrontRemoved(), variant); break;
            default:
                const QString m = QString("no property, index ").append(index.toQString());
                SWIFT_VERIFY_X(false, Q_FUNC_INFO, qUtf8Printable(m));
                break;
            }
        }
    }

    int
    ICoordinateWithRelativePosition::comparePropertyByIndex(CPropertyIndexRef index,
                                                            const ICoordinateWithRelativePosition &compareValue) const
    {
        if (ICoordinateGeodetic::canHandleIndex(index))
        {
            return ICoordinateGeodetic::comparePropertyByIndex(index, compareValue);
        }
        if (!index.isMyself())
        {
            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexRelativeBearing:
                return m_relativeBearing.comparePropertyByIndex(index.copyFrontRemoved(),
                                                                compareValue.getRelativeBearing());
            case IndexRelativeDistance:
                return m_relativeDistance.comparePropertyByIndex(index.copyFrontRemoved(),
                                                                 compareValue.getRelativeDistance());
            default:
                const QString m = QString("no property, index ").append(index.toQString());
                Q_ASSERT_X(false, Q_FUNC_INFO, m.toLocal8Bit().constData());
                Q_UNUSED(m)
                break;
            }
        }
        return 0;
    }

    QString ICoordinateWithRelativePosition::convertToQString(bool i18n) const
    {
        return m_relativeBearing.toQString(i18n) % u' ' % m_relativeDistance.toQString(i18n) % u' ' %
               ICoordinateGeodetic::convertToQString(i18n);
    }

    ICoordinateWithRelativePosition::ICoordinateWithRelativePosition() {}

    bool ICoordinateWithRelativePosition::canHandleIndex(CPropertyIndexRef index)
    {
        if (ICoordinateGeodetic::canHandleIndex(index)) { return true; }
        const int i = index.frontCasted<int>();
        return (i >= static_cast<int>(IndexRelativeDistance)) && (i <= static_cast<int>(IndexRelativeBearing));
    }
} // namespace swift::misc::geo
