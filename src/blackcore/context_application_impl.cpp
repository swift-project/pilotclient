/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "context_application_impl.h"
#include "context_runtime.h"
#include "input_manager.h"
#include "blackmisc/settingutilities.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/loghandler.h"
#include <QtMsgHandler>
#include <QFile>

using namespace BlackMisc;

namespace BlackCore
{
    CContextApplication::CContextApplication(CRuntimeConfig::ContextMode mode, CRuntime *runtime) :
        IContextApplication(mode, runtime)
    {}

    CContextApplication *CContextApplication::registerWithDBus(CDBusServer *server)
    {
        if (!server || this->m_mode != CRuntimeConfig::LocalInDbusServer) { return this; }
        server->addObject(IContextApplication::ObjectPath(), this);
        return this;
    }

    void CContextApplication::logMessage(const CStatusMessage &message, const COriginator &origin)
    {
        if (!origin.isFromSameProcess())
        {
            CLogHandler::instance()->logRemoteMessage(message);
        }
        emit this->messageLogged(message, origin);
    }

    qint64 CContextApplication::ping(qint64 token) const
    {
        return token;
    }

    void CContextApplication::notifyAboutComponentChange(uint component, uint action)
    {
        CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << QString::number(component) << QString::number(action);
        this->componentChanged(component, action);
    }

    bool CContextApplication::writeToFile(const QString &fileName, const QString &content)
    {
        CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << fileName << content.left(25);
        if (fileName.isEmpty()) { return false; }
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            QTextStream out(&file);
            out << content;
            return true;
        }
        else
        {
            return false;
        }
    }

    QString CContextApplication::readFromFile(const QString &fileName)
    {
        CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << fileName;
        QFile file(fileName);
        QString content;
        if (fileName.isEmpty()) return content;
        if (file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QTextStream in(&file);
            in >> content;
            file.close();
        }
        return content;
    }

    bool CContextApplication::removeFile(const QString &fileName)
    {
        if (fileName.isEmpty()) return false;
        return QFile::remove(fileName);
    }

    bool CContextApplication::existsFile(const QString &fileName)
    {
        if (fileName.isEmpty()) return false;
        return QFile::exists(fileName);
    }

    void CContextApplication::processHotkeyFuncEvent(const BlackMisc::Event::CEventHotkeyFunction &event)
    {
        CInputManager::getInstance()->callFunctionsBy(event.getFunction(), event.getFunctionArgument());
        CLogMessage(this, CLogCategory::contextSlot()).debug() << "Calling function from origin" << event.getEventOriginator().toQString();
    }

} // namespace
