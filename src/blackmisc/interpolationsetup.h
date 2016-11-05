/* Copyright (C) 2016
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_INTERPOLATION_SETUP_H
#define BLACKMISC_INTERPOLATION_SETUP_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/propertyindex.h"
#include "blackmisc/valueobject.h"
#include <QString>

namespace BlackMisc
{
    /*!
     * Value object for interpolator and rendering
     */
    class BLACKMISC_EXPORT CInterpolationAndRenderingSetup :
        public CValueObject<CInterpolationAndRenderingSetup>
    {
    public:
        //! Properties by index
        enum ColumnIndex
        {
            IndexInterpolatorDebugMessages = BlackMisc::CPropertyIndex::GloablIndexInterpolatorSetup,
            IndexSimulatorDebugMessages,
            IndexForceFullInterpolation
        };

        //! Constructor.
        CInterpolationAndRenderingSetup();

        //! Debugging messages
        bool showInterpolatorDebugMessages() const { return m_interpolatorDebugMessage; }

        //! Debugging messages
        void setInterpolatorDebuggingMessages(bool debug) { m_interpolatorDebugMessage = debug; }

        //! Debugging messages
        bool showSimulatorDebugMessages() const { return m_simulatorDebugMessages; }

        //! Debugging messages
        void setDriverDebuggingMessages(bool debug) { m_simulatorDebugMessages = debug; }

        //! Full interpolation
        bool forceFullInterpolation() const { return m_forceFullInterpolation; }

        //! Force full interpolation
        void setForceFullInterpolation(bool force) { m_forceFullInterpolation = force; }

        //! \copydoc BlackMisc::Mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

        //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
        CVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const;

        //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
        void setPropertyByIndex(const BlackMisc::CPropertyIndex &index, const CVariant &variant);

    private:
        bool m_interpolatorDebugMessage = false;  //! Debug messages in interpolator
        bool m_simulatorDebugMessages = false;    //! Debug messages of simulator (aka plugin)
        bool m_forceFullInterpolation = false;    //! always do a full interpolation, even if aircraft is not moving

        BLACK_METACLASS(
            CInterpolationAndRenderingSetup,
            BLACK_METAMEMBER(interpolatorDebugMessage),
            BLACK_METAMEMBER(simulatorDebugMessages),
            BLACK_METAMEMBER(forceFullInterpolation)
        );
    };
} // namespace

Q_DECLARE_METATYPE(BlackMisc::CInterpolationAndRenderingSetup)

#endif // guard
