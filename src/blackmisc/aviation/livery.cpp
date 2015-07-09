/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/aviation/livery.h"
#include "blackmisc/datastoreutility.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include "blackmisc/propertyindex.h"
#include "blackmisc/variant.h"


using namespace BlackMisc;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Geo;

namespace BlackMisc
{
    namespace Aviation
    {
        CLivery::CLivery()
        { }

        CLivery::CLivery(const QString &combinedCode, const CAirlineIcaoCode &airline, const QString &description, const QString &colorFuselage, const QString &colorTail, bool isMilitary) :
            CLivery(-1, combinedCode, airline, description, colorFuselage, colorTail, isMilitary)
        { }

        CLivery::CLivery(int dbKey, const QString &combinedCode, const CAirlineIcaoCode &airline, const QString &description, const QString &colorFuselage, const QString &colorTail, bool isMilitary) :
            m_dbKey(dbKey), m_airline(airline),
            m_combinedCode(combinedCode.trimmed().toUpper()), m_description(description.trimmed()),
            m_colorFuselage(normalizeHexColor(colorFuselage)), m_colorTail(normalizeHexColor(colorTail)),
            m_military(isMilitary)
        { }

        QString CLivery::convertToQString(bool i18n) const
        {
            QString s(i18n ? QCoreApplication::translate("Aviation", "Livery") : "Livery");
            s.append(m_combinedCode);
            if (!this->m_description.isEmpty()) { s.append(' ').append(this->m_description); }
            if (!this->m_colorFuselage.isEmpty()) { s.append(" F: ").append(this->m_colorFuselage); }
            if (!this->m_colorTail.isEmpty()) { s.append(" T: ").append(this->m_colorTail); }
            if (this->isMilitary()) { s.append(" Military");}
            return s;

            // force strings for translation in resource files
            (void)QT_TRANSLATE_NOOP("Aviation", "Livery");
        }

        bool CLivery::hasCompleteData() const
        {
            return !m_description.isEmpty() && !m_combinedCode.isEmpty();
        }

        CLivery CLivery::fromDatabaseJson(const QJsonObject &json)
        {
            QJsonArray inner = json["cell"].toArray();
            Q_ASSERT_X(!inner.isEmpty(), Q_FUNC_INFO, "Missing JSON");
            if (inner.isEmpty()) { return CLivery(); }

            int i = 0;
            int dbKey = inner.at(i++).toInt(-1);
            QString code(inner.at(i++).toString());
            QString combinedCode(inner.at(i++).toString());
            QString airlineWithId(inner.at(i++).toString());
            QString airlineName(inner.at(i++).toString());
            QString description(inner.at(i++).toString());
            QString colorFuselage(normalizeHexColor(inner.at(i++).toString()));
            QString colorTail(normalizeHexColor(inner.at(i++).toString()));
            bool military = CDatastoreUtility::dbBoolStringToBool(inner.at(i++).toString());

            int airlineId(CDatastoreUtility::extractIntegerKey(airlineWithId));
            CAirlineIcaoCode airline;
            airline.setDbKey(airlineId);
            airline.setName(airlineName);
            Q_ASSERT_X(code.length() > 0, Q_FUNC_INFO, "Missing code");
            Q_ASSERT_X(description.length() > 0, Q_FUNC_INFO, "require description");
            CLivery livery(dbKey, combinedCode, airline, description, colorFuselage, colorTail, military);
            return livery;
        }

        QString CLivery::normalizeHexColor(const QString &color)
        {
            if (color.isEmpty()) { return ""; }
            QString c = color.trimmed().replace('#', "").toUpper();
            return c;
        }

        CVariant CLivery::propertyByIndex(const BlackMisc::CPropertyIndex &index) const
        {
            if (index.isMyself()) { return CVariant::from(*this); }
            if (IDatastoreObjectWithIntegerKey::canHandleIndex(index)) { return IDatastoreObjectWithIntegerKey::propertyByIndex(index); }
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexDescription:
                return CVariant::fromValue(m_description);
            case IndexColorFuselage:
                return CVariant::fromValue(this->m_colorFuselage);
            case IndexColorTail:
                return CVariant::fromValue(this->m_colorTail);
            case IndexCombinedCode:
                return CVariant::fromValue(this->m_combinedCode);
            case IndexIsMilitary:
                return CVariant::fromValue(this->m_military);
            default:
                return CValueObject::propertyByIndex(index);
            }
        }

        void CLivery::setPropertyByIndex(const CVariant &variant, const BlackMisc::CPropertyIndex &index)
        {
            if (index.isMyself()) { (*this) = variant.to<CLivery>(); return; }
            if (IDatastoreObjectWithIntegerKey::canHandleIndex(index)) { IDatastoreObjectWithIntegerKey::setPropertyByIndex(variant, index); return; }
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexDescription:
                this->m_description = variant.toQString(false);
                break;
            case IndexColorFuselage:
                this->setColorFuselage(variant.toQString(false));
                break;
            case IndexColorTail:
                this->setColorTail(variant.toQString(false));
                break;
            case IndexCombinedCode:
                this->setCombinedCode(variant.toQString(false));
                break;
            case IndexIsMilitary:
                this->setMilitary(variant.toBool());
                break;
            default:
                CValueObject::setPropertyByIndex(variant, index);
                break;
            }
        }

    } // namespace
} // namespace
