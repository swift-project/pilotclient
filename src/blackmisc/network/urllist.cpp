/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/network/urllist.h"
#include "blackmisc/math/mathutils.h"

using namespace BlackMisc::Math;

namespace BlackMisc
{
    namespace Network
    {
        CUrlList::CUrlList() { }

        CUrlList::CUrlList(const QStringList &listOfUrls)
        {
            for (const QString &url : listOfUrls)
            {
                this->push_back(CUrl(url));
            }
        }

        CUrlList::CUrlList(const CSequence<CUrl> &other) :
            CSequence<CUrl>(other)
        { }

        CUrl CUrlList::getRandomUrl() const
        {
            if (this->isEmpty()) { return CUrl(); }
            if (this->size() == 1) { return this->front();}
            int i = CMathUtils::randomInteger(0, this->size() - 1);
            return (*this)[i];
        }

        CUrl CUrlList::getRandomWithout(const CUrlList &exclude) const
        {
            CUrlList copy(*this);
            copy.removeIfIn(exclude);
            if (copy.isEmpty()) { return CUrl(); }
            return copy.getRandomUrl();
        }

        CUrl CUrlList::getNextUrl(bool randomStart) const
        {
            if (this->isEmpty()) { return CUrl(); }
            if (this->size() == 1) { return this->front();}
            if (m_currentIndexDistributedLoad < 0)
            {
                // random start point
                m_currentIndexDistributedLoad =  randomStart ?
                                                 CMathUtils::randomInteger(0, this->size() - 1) :
                                                 0;
            }
            else
            {
                m_currentIndexDistributedLoad++;
                if (m_currentIndexDistributedLoad >= this->size())
                {
                    m_currentIndexDistributedLoad = 0;
                }
            }
            return (*this)[m_currentIndexDistributedLoad];
        }

        CUrl CUrlList::getNextUrlWithout(const CUrlList &exclude, bool randomStart) const
        {
            CUrlList copy(*this);
            copy.removeIfIn(exclude);
            if (copy.isEmpty()) { return CUrl(); }
            return copy.getNextUrl(randomStart);
        }

        CUrlList CUrlList::appendPath(const QString &path) const
        {
            if (path.isEmpty() || this->isEmpty()) { return (*this); }
            CUrlList urls;
            for (const CUrl &url : (*this))
            {
                urls.push_back(url.withAppendedPath(path));
            }
            return urls;
        }

    } // namespace
} // namespace
