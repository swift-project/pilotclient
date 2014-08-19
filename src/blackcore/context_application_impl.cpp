/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "context_application_impl.h"
#include "context_runtime.h"
#include "input_manager.h"
#include "blackmisc/settingutilities.h"
#include <QtMsgHandler>
#include <QFile>

using namespace BlackMisc;

namespace BlackCore
{
    /*
     * Init this context
     */
    CContextApplication::CContextApplication(CRuntimeConfig::ContextMode mode, CRuntime *runtime) :
        IContextApplication(mode, runtime)
    {}

    /*
     * Ping, is DBus alive?
     */
    qint64 CContextApplication::ping(qint64 token) const
    {
        return token;
    }

    /*
     * Status message
     */
    void CContextApplication::sendStatusMessage(const CStatusMessage &message)
    {
        emit this->statusMessage(message);
    }

    /*
     * Status messages
     */
    void CContextApplication::sendStatusMessages(const CStatusMessageList &messages)
    {
        emit this->statusMessages(messages);
    }

    /*
     * Component has changed
     */
    void CContextApplication::notifyAboutComponentChange(uint component, uint action)
    {
        this->getRuntime()->logSlot(c_logContext, Q_FUNC_INFO, { QString::number(component), QString::number(action) });
        this->componentChanged(component, action);
    }

    /*
     * String to file
     */
    bool CContextApplication::writeToFile(const QString &fileName, const QString &content)
    {
        this->getRuntime()->logSlot(c_logContext, Q_FUNC_INFO, { fileName, content.left(25) });
        if (fileName.isEmpty()) return false;
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            QTextStream out(&file);
            out << content;
            return true;
        }
        else return false;
    }

    /*
     * File to string
     */
    QString CContextApplication::readFromFile(const QString &fileName)
    {
        this->getRuntime()->logSlot(c_logContext, Q_FUNC_INFO, fileName);
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

    /*
     * Remove file
     */
    bool CContextApplication::removeFile(const QString &fileName)
    {
        if (fileName.isEmpty()) return false;
        return QFile::remove(fileName);
    }

    /*
     * Check file
     */
    bool CContextApplication::existsFile(const QString &fileName)
    {
        if (fileName.isEmpty()) return false;
        return QFile::exists(fileName);
    }

    void CContextApplication::processHotkeyFuncEvent(const BlackMisc::Event::CEventHotkeyFunction &event)
    {
        CInputManager::getInstance()->callFunctionsBy(event.getFunction(), event.getFunctionArgument());
        qDebug() << "Calling function from origin" << event.getEventOriginator().toQString();
    }

} // namespace
