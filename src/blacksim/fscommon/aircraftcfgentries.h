/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKSIM_FSCOMMON_AIRCRAFTCFGENTRY_H
#define BLACKSIM_FSCOMMON_AIRCRAFTCFGENTRY_H

#include "blackmisc/propertyindex.h"
#include <QVariant>

namespace BlackSim
{
    namespace FsCommon
    {

        /*!
         * Set of aircraft.cfg entries representing an aircraft (FSX)
         * \remarks an entry in the aircraft.cfg is title, atc type, ... This class already bundles
         *          relevant entries, hence the class is named Entries (plural)
         */
        class CAircraftCfgEntries: public BlackMisc::CValueObject
        {
        public:

            //! Properties by index
            enum ColumnIndex
            {
                IndexEntryIndex = BlackMisc::CPropertyIndex::GlobalIndexCAircraftModel,
                IndexFilePath,
                IndexTitle,
                IndexAtcType,
                IndexAtcModel,
                IndexParkingCode
            };

            //! Default constructor
            CAircraftCfgEntries() {}

            /*!
             * Entries representing an aircraft
             * \param filePath
             * \param index
             * \param title
             * \param atcType
             * \param atcModel
             * \param atcParkingCode
             */
            CAircraftCfgEntries(const QString &filePath, qint32 index, const QString &title, const QString &atcType, const QString &atcModel, const QString &atcParkingCode);

            //! Virtual destructor
            virtual ~CAircraftCfgEntries() {}

            //! operator ==
            bool operator ==(const CAircraftCfgEntries &other) const;

            //! operator !=
            bool operator !=(const CAircraftCfgEntries &other) const;

            //! Filepath
            QString getFilePath() const { return this->m_filePath; }

            //! Title
            QString getTitle() const { return this->m_title; }

            //! Index
            qint32 getIndex() const { return this->m_index; }

            //! ATC model
            QString getAtcModel() const { return this->m_atcModel; }

            //! ATC type
            QString getAtcType() const { return this->m_atcType; }

            //! ATC parking code
            QString getAtcParkingCode() const { return this->m_atcParkingCode; }

            //! Filepath
            void setFilePath(const QString &filePath) { this->m_filePath = filePath; }

            //! Title
            void setTitle(const QString &title) { this->m_title = title; }

            //! Index
            void setIndex(const qint32 index) { this->m_index = index; }

            //! ATC model
            void setAtcModel(const QString &atcModel) { this->m_atcModel = atcModel; }

            //! ATC type
            void setAtcType(const QString &atcType) { this->m_atcType = atcType; }

            //! Parking code
            void setAtcParkingCode(const QString &parkingCode) { this->m_atcParkingCode = parkingCode; }

            //! \copydoc CValueObject::getValueHash()
            virtual uint getValueHash() const override;

            //! \copydoc CValueObject::toQVariant
            virtual QVariant toQVariant() const override { return QVariant::fromValue(*this); }

            //! \copydoc CValueObject::fromQVariant
            virtual void fromQVariant(const QVariant &variant) override { BlackMisc::setFromQVariant(this, variant); }

            //! \copydoc CValueObject::toJson
            virtual QJsonObject toJson() const override;

            //! \copydoc CValueObject::fromJson
            virtual void fromJson(const QJsonObject &json) override;

            //! \copydoc CValueObject::propertyByIndex
            virtual QVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const override;

            //! \copydoc CValueObject::setPropertyByIndex
            virtual void setPropertyByIndex(const QVariant &variant, const BlackMisc::CPropertyIndex &index) override;

            //! Register the metatypes
            static void registerMetadata();

            //! Members
            static const QStringList &jsonMembers();

        protected:
            //! \copydoc CValueObject::convertToQString
            virtual QString convertToQString(bool i18n = false) const override;

            //! \copydoc CValueObject::marshallToDbus
            virtual void marshallToDbus(QDBusArgument &) const override;

            //! \copydoc CValueObject::unmarshallFromDbus
            virtual void unmarshallFromDbus(const QDBusArgument &) override;

            //! \copydoc CValueObject::compareImpl
            int compareImpl(const CValueObject &otherBase) const override;

            //! \copydoc CValueObject::getMetaTypeId()
            int getMetaTypeId() const override;

        private:
            BLACK_ENABLE_TUPLE_CONVERSION(CAircraftCfgEntries)
            qint32 m_index;     //!< current index in given config
            QString m_filePath; //!< file path of aircraft.cfg
            QString m_title;    //!< Title in aircraft.cfg
            QString m_atcType;  //!< ATC type
            QString m_atcModel; //!< ATC model
            QString m_atcParkingCode; //!< ATC parking code

        };
    }
} // namespace

BLACK_DECLARE_TUPLE_CONVERSION(BlackSim::FsCommon::CAircraftCfgEntries, (o.m_index, o.m_filePath, o.m_title, o.m_atcType, o.m_atcModel, o.m_atcParkingCode))
Q_DECLARE_METATYPE(BlackSim::FsCommon::CAircraftCfgEntries)

#endif // guard
