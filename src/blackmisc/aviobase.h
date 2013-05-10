/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKMISC_AVIOBASE_H
#define BLACKMISC_AVIOBASE_H

// QtGlobal is required for asserts
#include "blackmisc/basestreamstringifier.h"
#include "blackmisc/pqconstants.h"
#include <QtGlobal>

namespace BlackMisc
{
namespace Aviation
{

/*!
 * \brief Base class for avionics
 */
class CAvionicsBase : public CBaseStreamStringifier
{

private:
    QString m_name; //!< name of the unit

protected:

    /*!
     * \brief Default constructor
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
     * \param otherSystem
     * \return
     */
    bool operator ==(const CAvionicsBase &otherSystem) const
    {
        if (this == &otherSystem) return true;
        return this->m_name == otherSystem.m_name;
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
