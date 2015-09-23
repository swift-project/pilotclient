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
#include "blackmisc/rgbcolor.h"
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
                IndexAirlineIcaoCode,
                IndexCombinedCode,
                IndexColorFuselage,
                IndexColorTail,
                IndexIsMilitary
            };

            //! Default constructor.
            CLivery();

            //! Constructor
            CLivery(const QString &combinedCode, const CAirlineIcaoCode &airline, const QString &description);

            //! Constructor
            CLivery(const QString &combinedCode, const CAirlineIcaoCode &airline, const QString &description, const QString &colorFuselage, const QString &colorTail, bool isMilitary);

            //! Constructor
            CLivery(const QString &combinedCode, const CAirlineIcaoCode &airline, const QString &description, const BlackMisc::CRgbColor &colorFuselage, const BlackMisc::CRgbColor &colorTail, bool isMilitary);

            //! Constructor
            CLivery(int dbKey, const QString &combinedCode, const CAirlineIcaoCode &airline, const QString &description, const QString &colorFuselage, const QString &colorTail, bool isMilitary);

            //! Constructor
            CLivery(int dbKey, const QString &combinedCode, const CAirlineIcaoCode &airline, const QString &description, const BlackMisc::CRgbColor &colorFuselage, const BlackMisc::CRgbColor &colorTail, bool isMilitary);

            //! Corresponding airline, if any
            const CAirlineIcaoCode &getAirlineIcaoCode() const { return m_airline; }

            //! Corresponding airline designator, if any
            const QString &getAirlineIcaoCodeDesignator() const { return m_airline.getDesignator(); }

            //! Combined code
            const QString &getCombinedCode() const { return m_combinedCode; }

            //! Get description.
            const QString &getDescription() const { return m_description; }

            //! Get fuselage color.
            const BlackMisc::CRgbColor &getColorFuselage() const { return m_colorFuselage; }

            //! Get tail color.
            const BlackMisc::CRgbColor &getColorTail() const { return m_colorTail; }

            //! Military livery
            bool isMilitary() const { return m_military; }

            //! Airline ICAO code
            bool setAirlineIcaoCode(const CAirlineIcaoCode &airlineIcao);

            //! Combined code
            void setCombinedCode(const QString &code) { m_combinedCode = code.trimmed().toUpper(); }

            //! Set fuselage color
            void setColorFuselage(const BlackMisc::CRgbColor &color) { this->m_colorFuselage = color; }

            //! Set tail color
            void setColorTail(const BlackMisc::CRgbColor &color) { this->m_colorTail = color; }

            //! Fuselage color set?
            bool hasColorFuselage() const;

            //! Tail color set?
            bool hasColorTail() const;

            //! Set description
            void setDescription(const QString &description) { this->m_description = description; }

            //! Military aircraft?
            void setMilitary(bool isMilitary) { this->m_military = isMilitary; }

            //! Matches combined code
            bool matchesCombinedCode(const QString &candidate) const;

            //! \copydoc CValueObject::propertyByIndex
            CVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const;

            //! \copydoc CValueObject::setPropertyByIndex
            void setPropertyByIndex(const CVariant &variant, const BlackMisc::CPropertyIndex &index);

            //! \copydoc CValueObject::convertToQString
            QString convertToQString(bool i18n = false) const;

            //! Complete data?
            bool hasCompleteData() const;

            //! Validate data
            BlackMisc::CStatusMessageList validate() const;

            //! Airline available?
            bool hasValidAirlineDesignator() const;

            //! Livery combined code available?
            bool hasCombinedCode() const;

            //! Livery representing airline
            bool isAirlineLivery() const;

            //! Livery representing airline standard livery
            bool isAirlineStandardLivery() const;

            //! Color livery
            bool isColorLivery() const;

            //! Update missing parts
            void updateMissingParts(const CLivery &otherLivery);

            //! Object from JSON
            static CLivery fromDatabaseJson(const QJsonObject &json, const QString &prefix = QString("liv_"));

            //! Valid combined code string?
            static bool isValidCombinedCode(const QString &candidate);

            //! Standard livery marker
            static const QString &standardLiveryMarker();

            //! Color livery marker
            static const QString &colorLiveryMarker();

        private:
            BLACK_ENABLE_TUPLE_CONVERSION(CLivery)
            CAirlineIcaoCode m_airline; //!< corresponding airline, if any
            QString m_combinedCode;     //!< livery code and pseudo airline ICAO code
            QString m_description;      //!< describes the livery
            BlackMisc::CRgbColor m_colorFuselage; //! color of fuselage
            BlackMisc::CRgbColor m_colorTail;     //! color of tail
            bool m_military = false; //! Military livery?
        };
    } // namespace
} // namespace

BLACK_DECLARE_TUPLE_CONVERSION(BlackMisc::Aviation::CLivery, (
                                   attr(o.m_dbKey),
                                   attr(o.m_timestampMSecsSinceEpoch),
                                   attr(o.m_airline),
                                   attr(o.m_combinedCode, flags <CaseInsensitiveComparison> ()),
                                   attr(o.m_description),
                                   attr(o.m_colorFuselage),
                                   attr(o.m_colorTail)
                               ))

Q_DECLARE_METATYPE(BlackMisc::Aviation::CLivery)

#endif // guard
