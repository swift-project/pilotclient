/* Copyright (C) 2019
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_FSD_DELETEATC_H
#define BLACKCORE_FSD_DELETEATC_H

#include "blackcore/fsd/messagebase.h"

namespace BlackCore::Fsd
{
    //! FSD Message Delete ATC
    class BLACKCORE_EXPORT DeleteAtc : public MessageBase
    {
    public:
        //! Constructor
        DeleteAtc(const QString &sender, const QString &cid);

        //! Message converted to tokens
        QStringList toTokens() const;

        //! Construct from tokens
        static DeleteAtc fromTokens(const QStringList &tokens);

        //! PDU identifier
        static QString pdu() { return "#DA"; }

        QString m_cid; //!< id

    private:
        DeleteAtc();
    };

    //! Equal to operator
    inline bool operator==(const DeleteAtc &lhs, const DeleteAtc &rhs)
    {
        return lhs.sender() == rhs.sender() &&
               lhs.receiver() == rhs.receiver() &&
               lhs.m_cid == rhs.m_cid;
    }

    //! Not equal to operator
    inline bool operator!=(const DeleteAtc &lhs, const DeleteAtc &rhs)
    {
        return !(lhs == rhs);
    }
}

#endif // guard
