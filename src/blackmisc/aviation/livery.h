/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_AVIATION_LIVERY_H
#define BLACKMISC_AVIATION_LIVERY_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/aviation/airlineicaocode.h"
#include "blackmisc/geo/coordinategeodetic.h"
#include "blackmisc/propertyindex.h"
#include "blackmisc/datastore.h"

namespace BlackMisc
{
    namespace Aviation
    {
        //! Value object encapsulating information about an airpot.
        class BLACKMISC_EXPORT CLivery :
            public CValueObject<CLivery>,
            public BlackMisc::IDatastoreObjectWithIntegerKey
        {
        public:
            //! Properties by index
            enum ColumnIndex
            {
                IndexDescription = BlackMisc::CPropertyIndex::GlobalIndexCLivery,
                IndexCombinedCode,
                IndexColorFuselage,
                IndexColorTail,
                IndexIsMilitary
            };

            //! Default constructor.
            CLivery();

            //! Constructor
            CLivery(const QString &combinedCode, const CAirlineIcaoCode &airline, const QString &description, const QString &colorFuselage, const QString &colorTail, bool isMilitary);

            //! Constructor
            CLivery(int dbKey, const QString &combinedCode, const CAirlineIcaoCode &airline, const QString &description, const QString &colorFuselage, const QString &colorTail, bool isMilitary);

            //! Corresponding airline, if any
            const CAirlineIcaoCode &getAirlineIcao() const { return m_airline; }

            //! Combined code
            const QString &getCombinedCode() const { return m_combinedCode; }

            //! Get description.
            const QString &getDescription() const { return m_description; }

            //! Get fuselage color.
            const QString &getColorFuselage() const { return m_colorFuselage; }

            //! Get tail color.
            const QString &getColorTails() const { return m_colorTail; }

            //! Military livery
            bool isMilitary() const { return m_military; }

            //! Airline ICAO code
            void setAirlineIcao(const CAirlineIcaoCode &airlineIcao) { m_airline = airlineIcao; }

            //! Combined code
            void setCombinedCode(const QString &code) { m_combinedCode = code.trimmed().toUpper(); }

            //! Set fuselage color
            void setColorFuselage(const QString &color) { this->m_colorFuselage = normalizeHexColor(color); }

            //! Set tail color
            void setColorTail(const QString &color) { this->m_colorTail = normalizeHexColor(color); }

            //! Military aircraft?
            void setMilitary(bool isMilitary) { this->m_military = isMilitary; }

            //! \copydoc CValueObject::propertyByIndex
            CVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const;

            //! \copydoc CValueObject::setPropertyByIndex
            void setPropertyByIndex(const CVariant &variant, const BlackMisc::CPropertyIndex &index);

            //! \copydoc CValueObject::convertToQString
            QString convertToQString(bool i18n = false) const;

            //! Complete data?
            bool hasCompleteData() const;

            //! Object from JSON
            static CLivery fromDatabaseJson(const QJsonObject &json);

        private:
            BLACK_ENABLE_TUPLE_CONVERSION(CLivery)
            int m_dbKey = -1;           //!< optional DB key
            CAirlineIcaoCode m_airline; //!< corresponding airline, if any
            QString m_combinedCode;     //!< livery code and pseudo airline ICAO code
            QString m_description;
            QString m_colorFuselage;
            QString m_colorTail;
            bool m_military = false; //! Military livery?

            static QString normalizeHexColor(const QString &color);
        };
    } // namespace
} // namespace

BLACK_DECLARE_TUPLE_CONVERSION(BlackMisc::Aviation::CLivery, (
                                   attr(o.m_dbKey),
                                   attr(o.m_airline),
                                   attr(o.m_combinedCode, flags <CaseInsensitiveComparison> ()),
                                   attr(o.m_description),
                                   attr(o.m_colorFuselage, flags <CaseInsensitiveComparison> ()),
                                   attr(o.m_colorTail, flags <CaseInsensitiveComparison> ())
                               ))

Q_DECLARE_METATYPE(BlackMisc::Aviation::CLivery)

#endif // guard
