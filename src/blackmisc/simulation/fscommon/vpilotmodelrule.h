/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SIMULATION_FSCOMMON_VPILOTMODELRULE_H
#define BLACKMISC_SIMULATION_FSCOMMON_VPILOTMODELRULE_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/metaclass.h"
#include "blackmisc/propertyindex.h"
#include "blackmisc/simulation/aircraftmodel.h"
#include "blackmisc/timestampbased.h"
#include "blackmisc/valueobject.h"
#include "blackmisc/variant.h"

#include <QMetaType>
#include <QString>
#include <QtGlobal>

namespace BlackMisc
{
    namespace Simulation
    {
        namespace FsCommon
        {
            //! Value object encapsulating information of software distributor.
            //! \deprecated vPilot handling will be most likely removed in the future
            class BLACKMISC_EXPORT CVPilotModelRule :
                public CValueObject<CVPilotModelRule>,
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
                QVariant propertyByIndex(BlackMisc::CPropertyIndexRef index) const;

                //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
                void setPropertyByIndex(BlackMisc::CPropertyIndexRef index, const QVariant &variant);

                //! \copydoc BlackMisc::Mixin::String::toQString
                QString convertToQString(bool i18n = false) const;

                //! Convert into aircraft model
                CAircraftModel toAircraftModel() const;

            private:
                QString m_modelName;           //!< model name
                QString m_folder;              //!< folder
                QString m_typeCode;            //!< type code, aka aircraft ICAO
                QString m_callsignPrefix;      //!< callsign prefix, aka airline ICAO
                qint64  m_updatedMsSinceEpoch; //!< updated when

                BLACK_METACLASS(
                    CVPilotModelRule,
                    BLACK_METAMEMBER(typeCode, 0, CaseInsensitiveComparison),
                    BLACK_METAMEMBER(callsignPrefix, 0, CaseInsensitiveComparison),
                    BLACK_METAMEMBER(modelName, 0, CaseInsensitiveComparison),
                    BLACK_METAMEMBER(folder, 0, CaseInsensitiveComparison),
                    BLACK_METAMEMBER(updatedMsSinceEpoch)
                );
            };

        } // namespace
    } // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Simulation::FsCommon::CVPilotModelRule)

#endif // guard
