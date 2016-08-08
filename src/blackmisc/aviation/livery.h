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

#include "blackmisc/aviation/airlineicaocode.h"
#include "blackmisc/blackmiscexport.h"
#include "blackmisc/db/datastore.h"
#include "blackmisc/metaclass.h"
#include "blackmisc/propertyindex.h"
#include "blackmisc/rgbcolor.h"
#include "blackmisc/statusmessagelist.h"
#include "blackmisc/valueobject.h"
#include "blackmisc/variant.h"

#include <QJsonObject>
#include <QMetaType>
#include <QString>

namespace BlackMisc
{
    namespace Aviation
    {
        //! Value object encapsulating information about an airpot.
        class BLACKMISC_EXPORT CLivery :
            public CValueObject<CLivery>,
            public BlackMisc::Db::IDatastoreObjectWithIntegerKey
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

            //! Combined code
            QString getCombinedCodePlusInfo() const;

            //! Get description.
            const QString &getDescription() const { return m_description; }

            //! Get corresponding airline name
            const QString &getAirlineName() const { return this->getAirlineIcaoCode().getName(); }

            //! Does simplified airline name contain the candidate
            bool isContainedInSimplifiedAirlineName(const QString &candidate) const;

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

            //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
            CVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const;

            //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
            void setPropertyByIndex(const BlackMisc::CPropertyIndex &index, const CVariant &variant);

            //! Compare for index
            int comparePropertyByIndex(const CPropertyIndex &index, const CLivery &compareValue) const;

            //! \copydoc BlackMisc::Mixin::String::toQString
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

            //! Standard livery marker string
            static const QString &standardLiveryMarker();

            //! Standard code for
            static QString getStandardCode(const BlackMisc::Aviation::CAirlineIcaoCode &airline);

            //! Color livery marker
            static const QString &colorLiveryMarker();

        private:
            CAirlineIcaoCode m_airline;           //!< corresponding airline, if any
            QString m_combinedCode;               //!< livery code and pseudo airline ICAO code
            QString m_description;                //!< describes the livery
            BlackMisc::CRgbColor m_colorFuselage; //! color of fuselage
            BlackMisc::CRgbColor m_colorTail;     //! color of tail
            bool m_military = false;              //! Military livery?

            BLACK_METACLASS(
                CLivery,
                BLACK_METAMEMBER(dbKey),
                BLACK_METAMEMBER(timestampMSecsSinceEpoch),
                BLACK_METAMEMBER(airline),
                BLACK_METAMEMBER(combinedCode, 0, CaseInsensitiveComparison),
                BLACK_METAMEMBER(description),
                BLACK_METAMEMBER(colorFuselage),
                BLACK_METAMEMBER(colorTail),
                BLACK_METAMEMBER(military)
            );
        };
    } // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Aviation::CLivery)

#endif // guard
