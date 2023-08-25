// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_NETWORK_URLLIST_H
#define BLACKMISC_NETWORK_URLLIST_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/collection.h"
#include "blackmisc/network/url.h"
#include "blackmisc/sequence.h"

#include <QMetaType>
#include <QString>
#include <QStringList>

BLACK_DECLARE_SEQUENCE_MIXINS(BlackMisc::Network, CUrl, CUrlList)

namespace BlackMisc::Network
{
    //! Value object encapsulating a list of URLs.
    class BLACKMISC_EXPORT CUrlList :
        public CSequence<CUrl>,
        public BlackMisc::Mixin::MetaType<CUrlList>
    {
    public:
        BLACKMISC_DECLARE_USING_MIXIN_METATYPE(CUrlList)
        using CSequence::CSequence;

        //! Default constructor.
        CUrlList();

        //! Construct from a base class object.
        CUrlList(const CSequence<CUrl> &other);

        //! By list of URLs
        explicit CUrlList(const QStringList &listOfUrls, bool removeDuplicates = true);

        //! Random location for distributed load
        CUrl getRandomUrl() const;

        //! Random location for distributed load, tested
        CUrl getRandomWorkingUrl(int maxTrials = 2, int timeoutMs = -1) const;

        //! Random location for distributed load
        CUrl getRandomWithout(const CUrl &exclude) const;

        //! Random location for distributed load
        CUrl getRandomWithout(const CUrlList &exclude) const;

        //! Append path to all URLs
        CUrlList withAppendedPath(const QString &path) const;

        //! Find by host
        CUrlList findByHost(const QString &host, Qt::CaseSensitivity cs = Qt::CaseInsensitive) const;

        //! To formatted String
        QString convertToQString(const QString &separator, bool i18n = false) const;

        //! URLs without duplicates
        CUrlList getWithoutDuplicates() const;

        //! Remove duplicated URL and return number of removed elements
        int removeDuplicates();
    };

    //! URL list with fail support
    class BLACKMISC_EXPORT CFailoverUrlList : public CUrlList
    {
    public:
        //! Default constructor.
        CFailoverUrlList(int maxTrials = 2);

        //! By list of URLs
        explicit CFailoverUrlList(const QStringList &listOfUrls, int maxTrials = 2);

        //! From url list
        CFailoverUrlList(const CUrlList &urlIst, int maxTrials = 2);

        //! All failed URLs
        const CUrlList &getFailedUrls() const { return m_failedUrls; }

        //! Size of failed URLs
        int getFailedUrlsSize() const { return m_failedUrls.size(); }

        //! Get without the failed URLs
        CUrlList getWithoutFailed() const;

        //! Failed URL
        bool addFailedUrl(const CUrl &failedUrl);

        //! Failed URLs
        bool addFailedUrls(const CUrlList &failedUrls);

        //! Failed host
        bool addFailedHost(const CUrl &failedUrl);

        //! Failed host
        bool addFailedHost(const QString &host, Qt::CaseSensitivity cs = Qt::CaseInsensitive);

        //! More URLs to try
        bool hasMoreUrlsToTry() const;

        //! Number of URLs which can be used for a retry
        int numberOfStillValidUrls() const;

        //! Next utl from this list
        CUrl obtainNextUrl(bool randomStart = false);

        //! Round robin with random start point
        CUrl obtainNextUrlWithout(bool randomStart = false, const CUrlList &exclude = CUrlList()) const;

        //! Next working URL, test if it can be connected
        CUrl obtainNextWorkingUrl(bool random = false, int connectTimeoutMs = -1);

        //! Get the error messages
        const QStringList &getErrorMessages() const { return m_errorMsgs; }

        //! Reset failed URL, allows to set an optional new number of max.trials
        void reset(int maxTrials = -1);

    private:
        int m_currentIndexDistributedLoad = -1; //!< index for random access
        int m_maxTrials = 2; //!< number of max trials
        CUrlList m_failedUrls; //!< failed tested URLs
        QStringList m_errorMsgs; //!< error messages while testing;
    };

} // namespace

Q_DECLARE_METATYPE(BlackMisc::Network::CUrlList)
Q_DECLARE_METATYPE(BlackMisc::Network::CFailoverUrlList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Network::CUrl>)

#endif // guard
