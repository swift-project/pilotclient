/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKMISC_AVIOBASE_H
#define BLACKMISC_AVIOBASE_H

// QtGlobal is required for asserts
#include "blackmisc/streamable.h"
#include "blackmisc/pqconstants.h"
#include <QtGlobal>

namespace BlackMisc
{
    namespace Aviation
    {

class CAvionicsBase : public BlackMisc::CStreamable
        /*!
         * \brief Base class for avionics
         */
        {
        protected:
            QString m_name; //!< name of the unit

            /*!
             * \brief Constructor
             */
            CAvionicsBase(const QString &name) : m_name(name) {}

            /*!
             * \brief Are the set values valid / in range
             * \return
             */
            virtual bool validValues()
            {
                return true;
            }

            /*!
             * \brief Set name
             * \param name
             */
            void setName(const QString &name)
            {
                this->m_name = name;
            }

            /*!
             * \brief operator ==
             * \param other
             * \return
             */
            bool operator ==(const CAvionicsBase &other) const
            {
                if (this == &other) return true;
                return this->m_name == other.m_name;
            }

            /*!
             * \brief Stream to DBus <<
             * \param argument
             */
            virtual void marshallToDbus(QDBusArgument &argument) const
            {
                argument << this->m_name;
            }

            /*!
             * \brief Stream from DBus >>
             * \param argument
             */
            virtual void unmarshallFromDbus(const QDBusArgument &argument)
            {
                argument >> this->m_name;
            }

        public:
            /*!
             * \brief Virtual destructor
             */
            virtual ~CAvionicsBase() {}

            /*!
             * \brief Name
             * \return
             */
            QString getName() const
            {
                return this->m_name;
            }
        };

    } // namespace
} // namespace

#endif // guard
