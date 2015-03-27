/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef BLACKMISC_VOICECAPABILITIES_H
#define BLACKMISC_VOICECAPABILITIES_H

//! \file

#include "valueobject.h"
#include "blackmiscfreefunctions.h"

namespace BlackMisc
{
    namespace Network
    {
        /*!
         * Value object encapsulating information for voice capabilities.
         */
        class CVoiceCapabilities : public CValueObject<CVoiceCapabilities>
        {
        public:
            //! Voice capabilities
            enum VoiceCapabilities
            {
                Unknown,
                Voice,
                VoiceReceivingOnly,
                TextOnly,
            };

            //! Default constructor.
            CVoiceCapabilities() = default;

            //! Constructor by callsign
            CVoiceCapabilities(VoiceCapabilities capabilities) : m_voiceCapabilities(static_cast<int>(capabilities)) {}

            //! Constructor.
            CVoiceCapabilities(const QString &flightPlanRemarks);

            //! Get capabilities
            VoiceCapabilities getCapabilities() const { return static_cast<VoiceCapabilities>(m_voiceCapabilities); }

            //! Set capabilites
            void setCapabilities(VoiceCapabilities capabilites) { m_voiceCapabilities = static_cast<int>(capabilites); }

            //! Is capability known
            bool isUnknown() const { return m_voiceCapabilities == Unknown; }

            //! \copydoc CValueObject::toIcon()
            virtual CIcon toIcon() const override;

            //! From enum
            static const CVoiceCapabilities &fromVoiceCapabilities(VoiceCapabilities capabilities);

            //! From flight plan remarks
            static CVoiceCapabilities fromFlightPlanRemarks(const QString &remarks)
            {
                return CVoiceCapabilities(remarks);
            }

            //! All capabilities as list
            static const QList<CVoiceCapabilities> &allCapabilities();

        protected:
            //! \copydoc CValueObject::convertToQString
            virtual QString convertToQString(bool i18n = false) const override;

        private:
            BLACK_ENABLE_TUPLE_CONVERSION(CVoiceCapabilities)
            int m_voiceCapabilities = Unknown;

            //! Capabilites from flight plans remarks such as "/V/"
            void setFromFlightPlanRemarks(const QString &flightPlanRemarks);
        };
    } // namespace
} // namespace

BLACK_DECLARE_TUPLE_CONVERSION(BlackMisc::Network::CVoiceCapabilities, (o.m_voiceCapabilities))
Q_DECLARE_METATYPE(BlackMisc::Network::CVoiceCapabilities)

#endif // guard
