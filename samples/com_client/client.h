#include "blackmisc/message_system.h"
#include "blackmisc/com_client.h"

class Client : public QObject
{
    Q_OBJECT
    
public:
    explicit Client(QObject *parent = NULL);
    ~Client();

protected slots:
    
    void onError(QAbstractSocket::SocketError,QString);
    void onClientConnected();

private:
    BlackMisc::CComClient comclient;
};
