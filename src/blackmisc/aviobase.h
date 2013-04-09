/*  Copyright (C) 2013 VATSIM Community
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef AVIOBASE_H
#define AVIOBASE_H

// QtGlobal is required for asserts
#include <QtGlobal>
#include "blackmisc/pqconstants.h"

namespace BlackMisc
{
namespace Aviation
{

/*!
 * \brief Base class for avionics
 */
class CAvionicsBase
{
    /*!
     * Stream operator for debugging
     * \brief operator <<
     * \param debug
     * \param avionic
     * \return
     * \remarks Has to be in the header files to avoid template link errors
     */
    friend QDebug operator<<(QDebug debug, const CAvionicsBase &avionic) {
        QString v = avionic.stringForStreamingOperator();
        debug << v;
        return debug;
    }
    /*!
     * Stream operator for log messages
     * \brief operator <<
     * \param log
     * \param avionic
     * \return
     * \remarks Has to be in the header files to avoid template link errors
     */
    friend CLogMessage operator<<(CLogMessage log, const CAvionicsBase &avionic) {
        QString v = avionic.stringForStreamingOperator();
        log << v;
        return log;
    }

private:
    QString m_name; //!< name of the unit
protected:
    /*!
     * \brief Default constructor
     */
    CAvionicsBase(const QString &name) : m_name(name) {}
    /*!
     * \brief Meaningful string representation
     * \return
     */
    virtual QString stringForStreamingOperator() const = 0;
    /*!
     * \brief Are the set values valid / in range
     * \return
     */
    virtual bool validValues() {
        return true;
    }
    /*!
     * \brief Set name
     * \param name
     */
    void setName(const QString &name) {
        this->m_name = name;
    }
    /*!
     * \brief operator ==
     * \param otherSystem
     * \return
     */
    bool operator ==(const CAvionicsBase &otherSystem) const  {
        if (this == &otherSystem) return true;
        return this->m_name == otherSystem.m_name;
    }

public:
    /*!
     * \brief Virtual destructor
     */
    virtual ~CAvionicsBase() {}
    /*!
     * \brief Cast as QString
     */
    operator QString() const {
        return this->stringForStreamingOperator();
    }
    /*!
     * \brief Name
     * \return
     */
    QString getName() const {
        return this->m_name;
    }
};
} // namespace
} // namespace

#endif // AVIOBASE_H
