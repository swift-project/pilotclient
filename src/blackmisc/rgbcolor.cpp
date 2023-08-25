// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackmisc/rgbcolor.h"
#include "blackmisc/mixin/mixincompare.h"
#include "blackmisc/comparefunctions.h"
#include "blackmisc/icons.h"
#include "blackmisc/stringutils.h"

#include <QBrush>
#include <QPainter>
#include <QPixmap>
#include <QSize>
#include <QStringBuilder>
#include <QtGlobal>

using namespace BlackMisc;

BLACK_DEFINE_VALUEOBJECT_MIXINS(BlackMisc, CRgbColor)

namespace BlackMisc
{
    CRgbColor::CRgbColor(const QString &color, bool isName)
    {
        this->setByString(color, isName);
    }

    CRgbColor::CRgbColor(int r, int g, int b) : m_r(r), m_g(g), m_b(b)
    {}

    CRgbColor::CRgbColor(const QColor &color)
    {
        this->setQColor(color);
    }

    QPixmap CRgbColor::toPixmap() const
    {
        QPixmap pixmap(QSize(16, 16));
        QPainter p(&pixmap);
        p.setBackground(QBrush(this->toQColor()));
        p.setBrush(this->toQColor());
        p.drawRect(0, 0, 16, 16);
        return pixmap;
    }

    CIcons::IconIndex CRgbColor::toIcon() const
    {
        // if (this->isValid())
        //{
        //     return CIcon(toPixmap(), hex());
        // }
        // else
        {
            return CIcons::StandardIconError16;
        }
    }

    QColor CRgbColor::toQColor() const
    {
        return QColor(red(), green(), blue());
    }

    bool CRgbColor::setQColor(const QColor &color)
    {
        if (color.isValid())
        {
            m_r = color.red();
            m_g = color.green();
            m_b = color.blue();
            return true;
        }
        else
        {
            this->setInvalid();
            return false;
        }
    }

    int CRgbColor::red() const
    {
        return m_r;
    }

    double CRgbColor::normalizedRed() const
    {
        double c = red() * 1.0;
        return c / colorRange();
    }

    QString CRgbColor::redHex(int digits) const
    {
        return intToHex(m_r, digits);
    }

    int CRgbColor::green() const
    {
        return m_g;
    }

    double CRgbColor::normalizedGreen() const
    {
        double c = green() * 1.0;
        return c / colorRange();
    }

    QString CRgbColor::greenHex(int digits) const
    {
        return intToHex(m_g, digits);
    }

    int CRgbColor::blue() const
    {
        return m_b;
    }

    double CRgbColor::normalizedBlue() const
    {
        double c = blue() * 1.0;
        return c / colorRange();
    }

    QString CRgbColor::blueHex(int digits) const
    {
        return intToHex(m_b, digits);
    }

    QString CRgbColor::hex(bool withHash) const
    {
        if (!isValid()) { return {}; }
        const QString h(redHex() + greenHex() + blueHex());
        return withHash ? u'#' % h : h;
    }

    int CRgbColor::packed() const
    {
        if (!isValid()) { return 0; }
        return static_cast<int>(toQColor().rgb() & qRgba(255, 255, 255, 0));
    }

    CRgbColor CRgbColor::fromPacked(int rgb)
    {
        return { qRed(rgb), qGreen(rgb), qBlue(rgb) };
    }

    void CRgbColor::setByString(const QString &color, bool isName)
    {
        if (color.isEmpty()) { return; }
        else if (isName)
        {
            const QColor q(color);
            m_r = q.red();
            m_g = q.green();
            m_b = q.blue();
        }
        else
        {
            const QString c(color.trimmed());
            QColor q(c);
            if (this->setQColor(q)) { return; }
            if (c.startsWith("#"))
            {
                this->setInvalid();
                return;
            }
            q.setNamedColor("#" + c);
            this->setQColor(q);
        }
    }

    bool CRgbColor::isValid() const
    {
        return m_r >= 0 && m_g >= 0 && m_b >= 0;
    }

    double CRgbColor::colorDistance(const CRgbColor &color) const
    {
        if (!this->isValid() && !color.isValid()) { return 0; }
        if (!this->isValid() || !color.isValid()) { return 1; }
        if (*this == color) { return 0.0; } // avoid rounding

        // all values 0-1
        const double rd = (normalizedRed() - color.normalizedRed());
        const double bd = (normalizedBlue() - color.normalizedBlue());
        const double gd = (normalizedGreen() - color.normalizedGreen());
        return (rd * rd + bd * bd + gd * gd) / 3.0;
    }

    void CRgbColor::setInvalid()
    {
        m_r = -1;
        m_g = -1;
        m_b = -1;
    }

    QString CRgbColor::convertToQString(bool i18n) const
    {
        Q_UNUSED(i18n);
        return this->hex();
    }

    QVariant CRgbColor::propertyByIndex(BlackMisc::CPropertyIndexRef index) const
    {
        if (index.isMyself()) { return QVariant::fromValue(*this); }
        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexBlue: return QVariant::fromValue(blue());
        case IndexRed: return QVariant::fromValue(red());
        case IndexGreen: return QVariant::fromValue(green());
        case IndexWebHex: return QVariant::fromValue(hex());
        default: return CValueObject::propertyByIndex(index);
        }
    }

    void CRgbColor::setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant)
    {
        if (index.isMyself())
        {
            (*this) = variant.value<CRgbColor>();
            return;
        }
        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexBlue: m_b = variant.toInt(); break;
        case IndexRed: m_r = variant.toInt(); break;
        case IndexGreen: m_g = variant.toInt(); break;
        case IndexWebHex: this->setByString(variant.toString()); break;
        default: CValueObject::setPropertyByIndex(index, variant); break;
        }
    }

    int CRgbColor::comparePropertyByIndex(CPropertyIndexRef index, const CRgbColor &compareValue) const
    {
        if (index.isMyself()) { return this->compare(compareValue); }
        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexBlue: return Compare::compare(m_b, compareValue.m_b);
        case IndexRed: return Compare::compare(m_r, compareValue.m_r);
        case IndexGreen: return Compare::compare(m_g, compareValue.m_g);
        case IndexWebHex: return this->compare(compareValue);
        default:
            Q_ASSERT_X(false, Q_FUNC_INFO, "Missing compare");
            break;
        }
        return 0;
    }

    int CRgbColor::compare(const CRgbColor &color) const
    {
        int c = Compare::compare(m_r, color.m_r);
        if (c != 0) { return c; }
        c = Compare::compare(m_g, color.m_g);
        if (c != 0) { return c; }
        return Compare::compare(m_b, color.m_b);
    }

    double CRgbColor::colorRange() const
    {
        if (!this->isValid()) { return 255; }
        if (m_b < 256 && m_g < 256 && m_r < 256) { return 255; }
        if (m_b < 4096 && m_g < 4096 && m_r < 4096) { return 4095; }
        return 65535;
    }

    QString CRgbColor::intToHex(int h, int digits)
    {
        return BlackMisc::intToHex(h, digits);
    }

} // namespace
