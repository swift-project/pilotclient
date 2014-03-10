/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKMISC_AVIOBASE_H
#define BLACKMISC_AVIOBASE_H

// QtGlobal is required for asserts
#include "blackmisc/valueobject.h"
#include "blackmisc/pqconstants.h"
#include <QtGlobal>

namespace BlackMisc
{
    namespace Aviation
    {

        //! \brief Base class for avionics
        class CAvionicsBase : public BlackMisc::CValueObject
        {
        protected:
            QString m_name; //!< name of the unit

            //! \brief Constructor
            CAvionicsBase(const QString &name) : m_name(name) {}

            //! \brief Are set values valid?
            virtual bool validValues() const
            {
                return true;
            }

            //! \brief Set name
            void setName(const QString &name)
            {
                this->m_name = name;
            }

            //! \brief operator ==
            bool operator ==(const CAvionicsBase &other) const;

            //! \brief operator !=
            bool operator !=(const CAvionicsBase &other) const
            {
                return !(other == (*this));
            }

            //! \copydoc CValueObject::getMetaTypeId
            virtual int getMetaTypeId() const override { return 0; }

            //! \copydoc CValueObject::isA
            virtual bool isA(int metaTypeId) const override { return this->CValueObject::isA(metaTypeId); }

            //! \copydoc CValueObject::compareImpl(otherBase)
            virtual int compareImpl(const CValueObject &otherBase) const override;

            //! \copydoc CValueObject::marshallToDbus()
            virtual void marshallToDbus(QDBusArgument &argument) const override;

            //! \copydoc CValueObject::unmarshallFromDbus()
            virtual void unmarshallFromDbus(const QDBusArgument &argument) override;

            //! \copydoc CValueObject::getValueHash()
            virtual uint getValueHash() const override;

        public:
            //! \brief Virtual destructor
            virtual ~CAvionicsBase() {}

            //! \brief Name
            QString getName() const
            {
                return this->m_name;
            }

        private:
            BLACK_ENABLE_TUPLE_CONVERSION(CAvionicsBase)
        };

    } // namespace
} // namespace

BLACK_DECLARE_TUPLE_CONVERSION(BlackMisc::Aviation::CAvionicsBase, (o.m_name))

#endif // guard
