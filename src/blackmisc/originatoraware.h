/* Copyright (C) 2015
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef BLACKMISC_ORIGINATORAWARE_H
#define BLACKMISC_ORIGINATORAWARE_H

//! \file

#include "blackmiscexport.h"
#include "originator.h"
#include <QObject>

namespace BlackMisc
{
    //! Value object encapsulating information about the originiator
    class BLACKMISC_EXPORT COriginatorAware
    {
    public:
        //! Get originator
        const COriginator &originator() const { return m_originator; }

        //! Originator with current timestamp
        COriginator getCurrentTimestampOriginator() const;

        //! My originator?
        bool isMyOriginator(const COriginator &otherOriginator) { return m_originator == otherOriginator; }

    protected:
        //! Use literal based originator name
        COriginatorAware(const QString &originatorName) : m_originator(originatorName) {}

        //! Connect with QObject providing then name
        COriginatorAware(QObject *nameProvider);

        //! Destructor
        ~COriginatorAware();

    private:
        COriginator m_originator;
        QMetaObject::Connection m_originatorConnection;

    };
} // namespace

#endif // guard
