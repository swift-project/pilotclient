/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_PROPERTYINDEX_H
#define BLACKMISC_PROPERTYINDEX_H

#include "valueobject.h"
#include "blackmiscfreefunctions.h"
#include <initializer_list>

namespace BlackMisc
{

    /*!
     * Property index. The index can be nested, that's why it is a sequence
     * (e.g. PropertyIndexPilot, PropertyIndexRealname).
     */
    class CPropertyIndex : public CValueObject
    {
        // In the first trial I have used CSequence<int> as base class
        // This has created too much circular dependencies of the headers
        // CIndexVariantMap is used in CValueObject, CPropertyIndex in CIndexVariantMap

    public:
        //! Global index, make sure the indexes are unqiue (for using them in class hierarchy)
        enum GlobalIndex
        {
            GlobalIndexCValueObject        =    0,
            GlobalIndexCPhysicalQuantity   =  100,
            GlobalIndexCStatusMessage      =  200,
            GlobalIndexCCallsign           = 1000,
            GlobalIndexCAircraftIcao       = 1100,
            GlobalIndexCAircraft           = 1200,
            GlobalIndexCAtcStation         = 1300,
            GlobalIndexCAirport            = 1400,
            GlobalIndexCModulator          = 2000,
            GlobalIndexICoordinateGeodetic = 3000,
            GlobalIndexCCoordinateGeodetic = 3100,
            GlobalIndexCClient             = 4000,
            GlobalIndexCUser               = 4100,
            GlobalIndexCAircraftModel      = 4200,
            GlobalIndexCVoiceRoom          = 5000,
            GlobalIndexCAircraftMapping    = 6000,
            GlobalIndexCAircraftCfgEntries = 6100
        };

        //! Default constructor.
        CPropertyIndex();

        //! Non nested index
        CPropertyIndex(int singleProperty);

        //! Initializer list constructor
        CPropertyIndex(std::initializer_list<int> il);

        //! Construct from a base class object.
        CPropertyIndex(const QList<int> &indexes);

		//! From string
        CPropertyIndex(const QString &indexes);

        //! Copy with first element removed
        CPropertyIndex copyFrontRemoved() const;

        //! Is nested index?
        bool isNested() const;

        //! Myself index, used with nesting
        bool isMyself() const { return this->m_indexes.isEmpty(); }

        //! Empty?
        bool isEmpty() const { return this->m_indexes.isEmpty(); }

        //! First element casted to given type, usually then PropertIndex enum
        template<class CastType> CastType frontCasted() const
        {
            Q_ASSERT(!this->m_indexes.isEmpty());
            int f = this->m_indexes.isEmpty() ? 0 : this->m_indexes.first();
            return static_cast<CastType>(f);
        }

        //! Equal operator ==
        bool operator ==(const CPropertyIndex &other) const;

        //! Unequal operator !=
        bool operator !=(const CPropertyIndex &other) const;

        //! \copydoc CValueObject::CPropertyIndexlueHash()
        virtual uint getValueHash() const override;

        //! \copydoc CValueObject::toQVariant
        virtual QVariant toQVariant() const override { return QVariant::fromValue(*this); }

        //! \copydoc CValueObject::convertFromQVariant
        virtual void convertFromQVariant(const QVariant &variant) override { BlackMisc::setFromQVariant(this, variant); }

        //! \copydoc CValueObject::toJson
        virtual QJsonObject toJson() const override;

        //! \copydoc CValueObject::convertFromJson
        virtual void convertFromJson(const QJsonObject &json) override;

        //! Register metadata
        static void registerMetadata();

        //! JSON member names
        static const QStringList &jsonMembers();

    protected:
        //! \copydoc CValueObject::convertToQString
        virtual QString convertToQString(bool i18n = false) const override;

        //! \copydoc CValueObject::getMetaTypeId
        virtual int getMetaTypeId() const override;

        //! \copydoc CValueObject::isA
        virtual bool isA(int metaTypeId) const override;

        //! \copydoc CValueObject::compareImpl
        virtual int compareImpl(const CValueObject &other) const override;

        //! \copydoc CValueObject::marshallToDbus
        virtual void marshallToDbus(QDBusArgument &argument) const override;

        //! \copydoc CValueObject::unmarshallFromDbus
        virtual void unmarshallFromDbus(const QDBusArgument &argument) override;

        //! \copydoc CValueObject::parseFromString
        virtual void parseFromString(const QString &indexes) override;

    private:
        BLACK_ENABLE_TUPLE_CONVERSION(CPropertyIndex)
        QList<int> m_indexes;
        QString m_indexString; //! I use a little trick here, the string is used with the tupel system, as it provides all operators, hash ..

        //! Convert list to string
        void listToString();

        //! String to list
        void stringToList();

    };
} //namespace

BLACK_DECLARE_TUPLE_CONVERSION(BlackMisc::CPropertyIndex, (o.m_indexString))
Q_DECLARE_METATYPE(BlackMisc::CPropertyIndex)

#endif //guard
