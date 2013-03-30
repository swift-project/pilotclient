#include "blackmisc/pqunits.h"

namespace BlackMisc {

/**
 * Convert to SI
 */
double CTemperatureUnit::convertToSiConversionUnit(double value) const
{
    double v = value + this->m_conversionOffsetToSi;
    v *= this->getConversionFactorToSI();
    return v;
}

/**
 * Convert from SI
 */
double CTemperatureUnit::convertFromSiConversionUnit(double value) const
{
    double v = value / this->getConversionFactorToSI();
    v -= this->m_conversionOffsetToSi;
    return v;
}

/**
 * Convert from SI
 */
double CAngleUnit::convertFromSiConversionUnit(double value) const
{
    double v;
    // still a design flaw since I have to distinguish as per type
    // but an own converter per object was really too much
    if ((*this) == CAngleUnit::sexagesimalDeg()) {
        value = value * 180 / M_PI; // degree
        v = floor(value);
        double c = value - v;
        double mr = c * 60.0;
        double m = floor(mr); // minutes
        double s = (mr-m) * 60; // seconds + rest fraction
        v = (v + (m/100) + (s/10000));
    } else {
        v = CMeasurementUnit::convertFromSiConversionUnit(value);
    }
    return v;
}

/**
 * Convert to SI
 */
double CAngleUnit::convertToSiConversionUnit(double value) const
{
    // still a design flaw since I have to distinguish as per type
    // but an own converter per object was really too much
    double v;
    if ((*this) == CAngleUnit::sexagesimalDeg()) {
        double v = floor(value); // degrees
        double c = value - v;
        c = c * 100.0;
        double m = floor(c);
        c = c - m;
        m /= 60.0; // minutes back to decimals
        double s = c / 36.0; // seconds back to decimals
        v = v + m + s;
        return v / 180.0 * M_PI;
    } else {
        v = CMeasurementUnit::convertToSiConversionUnit(value);
    }
    return v;
}

/**
 * Rounded to QString
 */
QString CAngleUnit::toQStringRounded(double value, int digits) const
{
    QString s;
    if ((*this) == CAngleUnit::sexagesimalDeg()) {
        double de = floor(value);
        double mi = floor((value-de)*100.0);
        double se = floor((value-de-mi/100.0)*1000000) / 100.0;
        QString ses = QLocale::system().toString(se, 'f', 2);
        s = QString::number(de).append(this->getUnitName()).append(QString::number(mi))
                .append("'").append(ses).append("\"");
    } else {
        s = CMeasurementUnit::toQStringRounded(value, digits);
    }
    return s;
}

}
