/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of Swift Project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_ICON_H
#define BLACKMISC_ICON_H

#include "icons.h"
#include "pqangle.h"

namespace BlackMisc
{
    /*!
     * Value object for icons.
     */
    class CIcon : public BlackMisc::CValueObject
    {

    public:
        //! Default constructor.
        CIcon() {}

        //! Constructor.
        CIcon(CIcons::IconIndexes index, const QString &descriptiveText) :
            m_index(static_cast<int>(index)), m_descriptiveText(descriptiveText) {}

        //! \copydoc CValueObject::toQVariant
        virtual QVariant toQVariant() const override { return QVariant::fromValue(*this); }

        //! Get descriptive text
        const QString &getDescriptiveText() const { return this->m_descriptiveText; }

        //! Index
        CIcons::IconIndexes getIndex() const { return static_cast<CIcons::IconIndexes>(this->m_index);}

        //! Corresponding pixmap
        QPixmap toPixmap() const;

        //! Icon set?
        bool isSet() const { return this->m_index != static_cast<int>(CIcons::NotSet);}

        //! Rotate by n degrees
        void setRotation(int degrees) { this->m_rotateDegrees = degrees; }

        //! Rotate by given degrees
        void setRotation(const BlackMisc::PhysicalQuantities::CAngle &rotate);

        //! Equal operator ==
        bool operator ==(const CIcon &other) const;

        //! Unequal operator !=
        bool operator !=(const CIcon &other) const;

        //! \copydoc CValueObject::getValueHash
        virtual uint getValueHash() const override;

        //! \copydoc CValueObject::toJson
        virtual QJsonObject toJson() const override;

        //! \copydoc CValueObject::fromJson
        void fromJson(const QJsonObject &json) override;

        //! Register metadata
        static void registerMetadata();

        //! \copydoc TupleConverter<>::jsonMembers()
        static const QStringList &jsonMembers();

        //! Implicit conversion
        operator QPixmap () const { return this->toPixmap(); }

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

    private:
        BLACK_ENABLE_TUPLE_CONVERSION(CIcon)
        int m_index = static_cast<int>(CIcons::NotSet);
        int m_rotateDegrees = 0;
        QString m_descriptiveText;
    };
} // namespace

BLACK_DECLARE_TUPLE_CONVERSION(BlackMisc::CIcon, (o.m_index, o.m_descriptiveText))
Q_DECLARE_METATYPE(BlackMisc::CIcon)

#endif // guard
