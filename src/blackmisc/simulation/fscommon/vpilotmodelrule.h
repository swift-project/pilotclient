/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SIMULATION_FSCOMMON_VPILOTMODELRULE_H
#define BLACKMISC_SIMULATION_FSCOMMON_VPILOTMODELRULE_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/simulation/aircraftmodel.h"
#include "blackmisc/valueobject.h"
#include "blackmisc/datastore.h"
#include "blackmisc/timestampbased.h"
#include <QJsonObject>

namespace BlackMisc
{
    namespace Simulation
    {
        namespace FsCommon
        {
            //! Value object encapsulating information of software distributor.
            class BLACKMISC_EXPORT CVPilotModelRule :
                public BlackMisc::CValueObject<CVPilotModelRule>,
                public ITimestampBased
            {
            public:
                //! Property indexes
                enum ColumnIndex
                {
                    IndexModelName = CPropertyIndex::GlobalIndexCVPilotModelRule,
                    IndexFolder,
                    IndexTypeCode,
                    IndexCallsignPrefix
                };

                //! Default constructor
                CVPilotModelRule();

                //! Default constructor.
                CVPilotModelRule(const QString &modelName, const QString &folder,  const QString &typeCode, const QString &callsignPrefix, qint64 updated);

                //! Get model name
                const QString &getModelName() const { return this->m_modelName;}

                //! Get folder
                const QString &getFolder() const { return this->m_folder;}

                //! Distributor derived from folder (hardcoded)
                const QString getDistributor() const;

                //! Get type code
                const QString &getTypeCode() const { return this->m_typeCode;}

                //! Get callsign prefix
                const QString &getCallsignPrefix() const { return this->m_callsignPrefix;}

                //! Model name
                void setModelName(const QString &name) { this->m_modelName = name.trimmed().toUpper(); }

                //! Type code
                void setTypeCode(const QString &code) { this->m_typeCode = code.trimmed().toUpper(); }

                //! Folder
                void setFolder(const QString &folder) { this->m_folder = folder.trimmed().toUpper(); }

                //! Callsign prefix
                void setCallsignPrefix(const QString &callsign) { this->m_callsignPrefix = callsign.trimmed().toUpper(); }

                //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
                CVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const;

                //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
                void setPropertyByIndex(const CVariant &variant, const BlackMisc::CPropertyIndex &index);

                //! \copydoc BlackMisc::Mixin::String::toQString
                QString convertToQString(bool i18n = false) const;

                //! Convert into aircraft model
                CAircraftModel toAircraftModel() const;

            private:
                BLACK_ENABLE_TUPLE_CONVERSION(CVPilotModelRule)
                QString m_modelName;           //!< model name
                QString m_folder;              //!< folder
                QString m_typeCode;            //!< type code, aka aircraft ICAO
                QString m_callsignPrefix;      //!< callsign prefix, aka airline ICAO
                qint64  m_updatedMsSinceEpoch; //!< updated when
            };

        } // namespace
    } // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Simulation::FsCommon::CVPilotModelRule)
BLACK_DECLARE_TUPLE_CONVERSION(BlackMisc::Simulation::FsCommon::CVPilotModelRule, (
                                   attr(o.m_typeCode, flags <CaseInsensitiveComparison> ()),
                                   attr(o.m_callsignPrefix, flags <CaseInsensitiveComparison> ()),
                                   attr(o.m_modelName, flags <CaseInsensitiveComparison> ()),
                                   attr(o.m_folder, flags <CaseInsensitiveComparison> ()),
                                   attr(o.m_updatedMsSinceEpoch)
                               ))
#endif // guard
