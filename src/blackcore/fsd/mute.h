// SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKCORE_FSD_MUTE_H
#define BLACKCORE_FSD_MUTE_H

#include "blackcore/fsd/messagebase.h"

namespace BlackCore::Fsd
{
    //! Mute the user for AFV
    class BLACKCORE_EXPORT Mute : public MessageBase
    {
    public:
        //! Constructor
        Mute(const QString &sender, const QString &receiver, bool mute);

        //! Message converted to tokens
        QStringList toTokens() const;

        //! Construct from tokens
        static Mute fromTokens(const QStringList &tokens);

        //! PDU identifier
        static QString pdu() { return "#MU"; }

        bool m_mute = false; //!< Flag whether the user should be muted/unmuted

    private:
        Mute() = default;
    };

    //! Equal to operator
    inline bool operator==(const Mute &lhs, const Mute &rhs)
    {
        return lhs.sender() == rhs.sender() &&
               lhs.receiver() == rhs.receiver() &&
               lhs.m_mute == rhs.m_mute;
    }

    //! Not equal to operator
    inline bool operator!=(const Mute &lhs, const Mute &rhs)
    {
        return !(lhs == rhs);
    }
}

#endif // guard
