/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_AUDIO_VOICESETUP_H
#define BLACKMISC_AUDIO_VOICESETUP_H

#include "blackmisc/statusmessagelist.h"
#include "blackmisc/valueobject.h"
#include "blackmisc/metaclass.h"
#include "blackmisc/propertyindex.h"
#include "blackmisc/blackmiscexport.h"

#include <QMetaType>
#include <QString>

namespace BlackMisc
{
    namespace Audio
    {
        //! Value object for a voice setup
        class BLACKMISC_EXPORT CVoiceSetup : public CValueObject<CVoiceSetup>
        {
        public:
            //! Properties by index
            enum ColumnIndex
            {
                IndexVatsimVoiceUdpPort = CPropertyIndex::GlobalIndexCVoiceSetup,
            };

            //! Default constructor.
            CVoiceSetup() {}

            //! Setup with values
            CVoiceSetup(int vatsimUdpPort);

            //! The voice UDP port
            void setVatsimUdpVoicePort(int port) { m_vatismVoiceUdpPort = port;}

            //! VATSIM UDP voice port
            int getVatsimUdpVoicePort() const { return m_vatismVoiceUdpPort; }

            //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
            CVariant propertyByIndex(const CPropertyIndex &index) const;

            //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
            void setPropertyByIndex(const CPropertyIndex &index, const CVariant &variant);

            //! \copydoc BlackMisc::Mixin::Index::comparePropertyByIndex
            int comparePropertyByIndex(const CPropertyIndex &index, const CVoiceSetup &compareValue) const;

            //! \copydoc BlackMisc::Mixin::String::toQString()
            QString convertToQString(bool i18n = false) const;

            //! Validate
            CStatusMessageList validate() const;

            //! Standard FSD setup for official VATSIM servers
            static const CVoiceSetup &vatsimStandard();

        private:
            int m_vatismVoiceUdpPort = 3292;

            BLACK_METACLASS(
                CVoiceSetup,
                BLACK_METAMEMBER(vatismVoiceUdpPort)
            );
        };
    } // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Audio::CVoiceSetup)

#endif // guard
