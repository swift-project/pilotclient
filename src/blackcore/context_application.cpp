#include "blackcore/context_application.h"
#include "blackcore/context_application_impl.h"
#include "blackcore/context_application_proxy.h"
#include "blackcore/context_settings.h"
#include "blackcore/input_manager.h"
#include "blackmisc/statusmessage.h"
#include <QCoreApplication>
#include <QThread>

using namespace BlackMisc;
using namespace BlackMisc::Settings;

namespace BlackCore
{

    QtMessageHandler IContextApplication::s_oldHandler = nullptr;

    IContextApplication *IContextApplication::create(CRuntime *parent, CRuntimeConfig::ContextMode mode, CDBusServer *server, QDBusConnection &conn)
    {
        switch (mode)
        {
        case CRuntimeConfig::Local:
        case CRuntimeConfig::LocalInDbusServer:
            return (new CContextApplication(mode, parent))->registerWithDBus(server);
        case CRuntimeConfig::Remote:
            return new BlackCore::CContextApplicationProxy(BlackCore::CDBusServer::ServiceName, conn, mode, parent);
        default:
            qFatal("Always initialize an application context!");
            return nullptr;
        }
    }

    /*
     * Constructor
     */
    IContextApplication::IContextApplication(CRuntimeConfig::ContextMode mode, CRuntime *runtime) :
        CContext(mode, runtime)
    {
        if (IContextApplication::s_contexts.isEmpty())
        {
            IContextApplication::s_oldHandler = qInstallMessageHandler(IContextApplication::messageHandlerDispatch);
        }
        IContextApplication::s_contexts.append(this);

        changeSettings(IContextSettings::SettingsHotKeys);
    }

    void IContextApplication::changeSettings(uint typeValue)
    {
        auto type = static_cast<IContextSettings::SettingsType>(typeValue);
        switch (type)
        {
        case IContextSettings::SettingsHotKeys:
        {
            CSettingKeyboardHotkeyList hotkeys = getIContextSettings()->getHotkeys();
            CInputManager::getInstance()->changeHotkeySettings(hotkeys);
            break;
        }
        default:
            break;
        }
    }

    /*
     *  Dispatch message
     */
    void IContextApplication::messageHandlerDispatch(QtMsgType type, const QMessageLogContext &messageContext, const QString &message)
    {
    }

} // namespace
