//  SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
//  SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_SIMULATION_PARTSTATUS_H
#define SWIFT_MISC_SIMULATION_PARTSTATUS_H

#include <QString>

#include "misc/swiftmiscexport.h"

namespace swift::misc::simulation
{
    //! Status regarding parts
    class SWIFT_MISC_EXPORT CPartsStatus
    {
    public:
        //! Ctor
        CPartsStatus() = default;

        //! Ctor
        explicit CPartsStatus(bool supportsParts) : m_supportsParts(supportsParts) {}

        //! Supporting parts
        bool isSupportingParts() const { return m_supportsParts; }

        //! Set support flag
        void setSupportsParts(bool supports) { m_supportsParts = supports; }

        //! Is a reused parts object?
        //! \remark means using last value again
        bool isReusedParts() const { return m_resusedParts; }

        //! Mark as reused
        void setReusedParts(bool reused) { m_resusedParts = reused; }

        //! Same parts as last parts?
        bool isSameParts() const { return m_isSameParts; }

        //! Same parts as last parts?
        void setSameParts(bool same) { m_isSameParts = same; }

        //! Info string
        QString toQString() const;

    private:
        bool m_supportsParts = false; //!< supports parts for given callsign
        bool m_resusedParts = false; //!< reusing from last step
        bool m_isSameParts = false; //!< same as last parts?
    };
} // namespace swift::misc::simulation

#endif // SWIFT_MISC_SIMULATION_PARTSTATUS_H
