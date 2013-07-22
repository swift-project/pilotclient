#ifndef FSDCLIENT_H
#define FSDCLIENT_H

#include <QObject>

class CFsdClient : public QObject
{
    Q_OBJECT
public:
    CFsdClient(QObject *parent = 0);

    void connectTo(const QString &host);
    
signals:
    
public slots:
    
};

#endif // FSDCLIENT_H
