/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKSIM_FSCOMMON_AIRCRAFTCFGENTRY_H
#define BLACKSIM_FSCOMMON_AIRCRAFTCFGENTRY_H

#include "blackmisc/valueobject.h"
#include <QVariant>

namespace BlackSim
{
    namespace FsCommon
    {

        /*!
         * \brief Set of aircraft.cfg entries representing an aircraft (FSX)
         * \remarks an entry in the aircraft.cfg is title, atc type, ... This class already bundles
         *          relevant entries, hence the class is named Entries (plural)
         */
        class CAircraftCfgEntries: public BlackMisc::CValueObject
        {
        private:
            qint32 m_index; //!< current index in given config
            QString m_filePath; //!< file path of aircraft.cfg
            QString m_title; //!< Title in aircraft.cfg
            QString m_atcType; //!< ATC type
            QString m_atcModel; //!< ATC model
            QString m_atcParkingCode; //!< ATC parking code

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

        public:

            //! \brief Properties by index
            enum ColumnIndex
            {
                IndexEntryIndex = 0,
                IndexFilePath,
                IndexTitle,
                IndexAtcType,
                IndexAtcModel,
                IndexParkingCode
            };

            //! \brief Default constructor
            CAircraftCfgEntries() {}

            /*!
             * \brief Entries representing an aircraft
             * \param filePath
             * \param index
             * \param title
             * \param atcType
             * \param atcModel
             * \param atcParkingCode
             */
            CAircraftCfgEntries(const QString &filePath, qint32 index, const QString &title, const QString &atcType, const QString &atcModel, const QString &atcParkingCode);

            //! \brief Virtual destructor
            virtual ~CAircraftCfgEntries() {}

            //! \brief operator ==
            bool operator ==(const CAircraftCfgEntries &otherEntry) const;

            //! \brief operator !=
            bool operator !=(const CAircraftCfgEntries &otherEntry) const;

            //! \copydoc CValueObject::propertyByIndex
            QVariant propertyByIndex(int index) const;

            //! \brief Filepath
            QString getFilePath() const { return this->m_filePath; }

            //! \brief Title
            QString getTitle() const { return this->m_title; }

            //! \brief Index
            qint32 getIndex() const { return this->m_index; }

            //! \brief ATC model
            QString getAtcModel() const { return this->m_atcModel; }

            //! \brief ATC type
            QString getAtcType() const { return this->m_atcType; }

            //! \brief ATC parking code
            QString getAtcParkingCode() const { return this->m_atcParkingCode; }

            //! \brief Filepath
            void setFilePath(const QString &filePath) { this->m_filePath = filePath; }

            //! \brief Title
            void setTitle(const QString &title) { this->m_title = title; }

            //! \brief Index
            void setIndex(const qint32 index) { this->m_index = index; }

            //! \brief ATC model
            void setAtcModel(const QString &atcModel) { this->m_atcModel = atcModel; }

            //! \brief ATC type
            void setAtcType(const QString &atcType) { this->m_atcType = atcType; }

            //! \brief Parking code
            void setAtcParkingCode(const QString &parkingCode) { this->m_atcParkingCode = parkingCode; }

            //! \copydoc CValueObject::getValueHash()
            virtual uint getValueHash() const override;

            //! \copydoc CValueObject::toQVariant()
            virtual QVariant toQVariant() const override
            {
                return QVariant::fromValue(*this);
            }

            //! \brief Register the metatypes
            static void registerMetadata();
        };
    }
} // namespace

Q_DECLARE_METATYPE(BlackSim::FsCommon::CAircraftCfgEntries)

#endif // guard
