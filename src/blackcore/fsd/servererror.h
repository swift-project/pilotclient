/* Copyright (C) 2019
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_FSD_SERVERERROR_H
#define BLACKCORE_FSD_SERVERERROR_H

#include "blackcore/fsd/messagebase.h"
#include "blackcore/fsd/enums.h"

namespace BlackCore::Fsd
{
    //! FSD Message Server Error
    class BLACKCORE_EXPORT ServerError : public MessageBase
    {
    public:
        //! Constructor
        ServerError(const QString &sender, const QString &receiver, ServerErrorCode errorCode, const QString &causingParameter, const QString &description);

        //! Fatal?
        bool isFatalError() const;

        //! Message converted to tokens
        QStringList toTokens() const;

        //! Construct from tokens
        static ServerError fromTokens(const QStringList &tokens);

        //! PDU identifier
        static QString pdu() { return "$ER"; }

        //! Getter
        //! @{
        const QString &getCausingParameter() const { static const QString n("no details");     return m_causingParameter.isEmpty() ? n : m_causingParameter; }
        const QString &getDescription()      const { static const QString n("no description"); return m_description.isEmpty()      ? n : m_description; }
        //! @}

        //! Properties
        //! @{
        ServerErrorCode m_errorNumber {};
        QString m_causingParameter;
        QString m_description;
        //! @}

    private:
        //! Ctor
        ServerError();
    };

    //! Equal to operator
    inline bool operator==(const ServerError &lhs, const ServerError &rhs)
    {
        return  lhs.sender() == rhs.sender() &&
                lhs.receiver() == rhs.receiver() &&
                lhs.m_errorNumber == rhs.m_errorNumber &&
                lhs.m_causingParameter == rhs.m_causingParameter &&
                lhs.m_description == rhs.m_description;
    }

    //! Not equal to operator
    inline bool operator!=(const ServerError &lhs, const ServerError &rhs)
    {
        return !(lhs == rhs);
    }
}

#endif // guard
