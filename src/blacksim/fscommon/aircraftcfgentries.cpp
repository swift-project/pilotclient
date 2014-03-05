/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "aircraftcfgentries.h"
#include "blackmisc/blackmiscfreefunctions.h"

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
        bool CAircraftCfgEntries::operator ==(const CAircraftCfgEntries &otherEntry) const
        {
            if (this == &otherEntry) return true;
            return this->m_title == otherEntry.m_title;
        }

        /*
         * Operator !=
         */
        bool CAircraftCfgEntries::operator !=(const CAircraftCfgEntries &otherEntry) const
        {
            if (this == &otherEntry) return false;
            return !((*this) == otherEntry);
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
                break;
            case IndexTitle:
                return this->m_title;
                break;
            case IndexAtcType:
                return this->m_atcType;
                break;
            case IndexAtcModel:
                return this->m_atcModel;
                break;
            case IndexParkingCode:
                return this->m_atcParkingCode;
                break;
            case IndexEntryIndex:
                return this->m_index;
                break;
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
            QList<uint> hashs;
            hashs << qHash(this->m_atcModel);
            hashs << qHash(this->m_atcParkingCode);
            hashs << qHash(this->m_atcType);
            hashs << qHash(this->m_filePath);
            hashs << qHash(this->m_index);
            hashs << qHash(this->m_title);
            return BlackMisc::calculateHash(hashs, "CAircraftCfgEntries");
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
            if (this->m_index != other.m_index)
            {
                return (this->m_index > other.m_index) ? 1 : -1;
            }
            int result;
            if ((result = this->m_atcModel.compare(other.m_atcModel, Qt::CaseInsensitive))) return result;
            if ((result = this->m_atcParkingCode.compare(other.m_atcParkingCode, Qt::CaseInsensitive))) return result;
            if ((result = this->m_atcType.compare(other.m_atcType, Qt::CaseInsensitive))) return result;;
            if ((result = this->m_filePath.compare(other.m_filePath, Qt::CaseInsensitive))) return result;;
            return this->m_title.compare(other.m_title, Qt::CaseInsensitive);
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
            argument << this->m_atcModel;
            argument << this->m_atcParkingCode;
            argument << this->m_atcType;
            argument << this->m_filePath;
            argument << this->m_index;
            argument << this->m_title;
        }

        /*
         * Unmarshall from DBus
         */
        void CAircraftCfgEntries::unmarshallFromDbus(const QDBusArgument &argument)
        {
            argument >> this->m_atcModel;
            argument >> this->m_atcParkingCode;
            argument >> this->m_atcType;
            argument >> this->m_filePath;
            argument >> this->m_index;
            argument >> this->m_title;
        }
    }
} // namespace
