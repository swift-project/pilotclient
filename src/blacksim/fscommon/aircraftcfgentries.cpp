/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "aircraftcfgentries.h"
#include "blackmisc/blackmiscfreefunctions.h"

using namespace BlackMisc;

namespace BlackSim
{
    namespace FsCommon
    {

        /*
         * Constructor
         */
        CAircraftCfgEntries::CAircraftCfgEntries(const QString &filePath, qint32 index, const QString &title, const QString &atcType, const QString &atcModel, const QString &atcParkingCode) :
            m_index(index), m_filePath(filePath), m_title(title), m_atcType(atcType),
            m_atcModel(atcModel), m_atcParkingCode(atcParkingCode)
        {
            // void
        }

        /*
         * Operator ==
         */
        bool CAircraftCfgEntries::operator ==(const CAircraftCfgEntries &other) const
        {
            if (this == &other) return true;
            return TupleConverter<CAircraftCfgEntries>::toTuple(*this) == TupleConverter<CAircraftCfgEntries>::toTuple(other);
        }

        /*
         * Operator !=
         */
        bool CAircraftCfgEntries::operator !=(const CAircraftCfgEntries &other) const
        {
            if (this == &other) return false;
            return !((*this) == other);
        }

        /*
         * String representation
         */
        QString CAircraftCfgEntries::convertToQString(bool) const
        {
            QString s = "{%1, %2, %3, %4, %5, %6}";
            s = s.arg(this->m_filePath).arg(this->m_index).arg(this->m_title)
                .arg(this->m_atcModel).arg(this->m_atcType).arg(this->m_atcParkingCode);
            return s;
        }

        /*
         * Get particular column
         */
        QVariant CAircraftCfgEntries::propertyByIndex(int index) const
        {
            switch (index)
            {
            case IndexFilePath:
                return this->m_filePath;
            case IndexTitle:
                return this->m_title;
            case IndexAtcType:
                return this->m_atcType;
            case IndexAtcModel:
                return this->m_atcModel;
            case IndexParkingCode:
                return this->m_atcParkingCode;
            case IndexEntryIndex:
                return this->m_index;
            default:
                break;
            }

            Q_ASSERT_X(false, "CAircraftCfgEntries", "index unknown");
            QString m = QString("no property, index ").append(QString::number(index));
            return QVariant::fromValue(m);
        }

        /*
         * Hash
         */
        uint CAircraftCfgEntries::getValueHash() const
        {
            return qHash(TupleConverter<CAircraftCfgEntries>::toTuple(*this));
        }

        /*
         *  Register metadata
         */
        void CAircraftCfgEntries::registerMetadata()
        {
            qRegisterMetaType<CAircraftCfgEntries>();
            qDBusRegisterMetaType<CAircraftCfgEntries>();
        }

        /*
         * Compare
         */
        int CAircraftCfgEntries::compareImpl(const CValueObject &otherBase) const
        {
            const auto &other = static_cast<const CAircraftCfgEntries &>(otherBase);
            return compare(TupleConverter<CAircraftCfgEntries>::toTuple(*this), TupleConverter<CAircraftCfgEntries>::toTuple(other));
        }

        /*
         * Metatype
         */
        int CAircraftCfgEntries::getMetaTypeId() const
        {
            return qMetaTypeId<CAircraftCfgEntries>();
        }

        /*
         * Marshall to DBus
         */
        void CAircraftCfgEntries::marshallToDbus(QDBusArgument &argument) const
        {
            argument << TupleConverter<CAircraftCfgEntries>::toTuple(*this);
        }

        /*
         * Unmarshall from DBus
         */
        void CAircraftCfgEntries::unmarshallFromDbus(const QDBusArgument &argument)
        {
            argument >> TupleConverter<CAircraftCfgEntries>::toTuple(*this);
        }

        /*
         * To JSON
         */
        QJsonObject CAircraftCfgEntries::toJson() const
        {
            return BlackMisc::serializeJson(CAircraftCfgEntries::jsonMembers(), TupleConverter<CAircraftCfgEntries>::toTuple(*this));
        }

        /*
         * To JSON
         */
        void CAircraftCfgEntries::fromJson(const QJsonObject &json)
        {
            BlackMisc::deserializeJson(json, CAircraftCfgEntries::jsonMembers(), TupleConverter<CAircraftCfgEntries>::toTuple(*this));
        }

        /*
         * Members
         */
        const QStringList &CAircraftCfgEntries::jsonMembers()
        {
            return TupleConverter<CAircraftCfgEntries>::jsonMembers();
        }

    }
} // namespace
