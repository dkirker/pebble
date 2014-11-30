#ifndef JSKITMANAGER_H
#define JSKITMANAGER_H

#include <QJSEngine>
#include "appmanager.h"
#include "appmsgmanager.h"

class JSKitPebble;
class JSKitLocalStorage;

class JSKitManager : public QObject
{
    Q_OBJECT
    LOG4QT_DECLARE_QCLASS_LOGGER

public:
    explicit JSKitManager(AppManager *apps, AppMsgManager *appmsg, QObject *parent = 0);
    ~JSKitManager();

signals:

public slots:

private slots:
    void handleAppStarted(const QUuid &uuid);
    void handleAppStopped(const QUuid &uuid);

private:
    void startJsApp();
    void stopJsApp();

private:
    friend class JSKitPebble;

    AppManager *_apps;
    AppMsgManager *_appmsg;
    AppInfo _curApp;
    QJSEngine *_engine;
    QPointer<JSKitPebble> _jspebble;
    QPointer<JSKitLocalStorage> _jsstorage;

};

#endif // JSKITMANAGER_H
