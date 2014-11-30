#include <QFile>
#include <QDir>

#include "jskitmanager.h"
#include "jskitobjects.h"

JSKitManager::JSKitManager(AppManager *apps, AppMsgManager *appmsg, QObject *parent) :
    QObject(parent), _apps(apps), _appmsg(appmsg), _engine(0)
{
    connect(_appmsg, &AppMsgManager::appStarted, this, &JSKitManager::handleAppStarted);
    connect(_appmsg, &AppMsgManager::appStopped, this, &JSKitManager::handleAppStopped);
}

JSKitManager::~JSKitManager()
{
    if (_engine) {
        stopJsApp();
    }
}

void JSKitManager::handleAppStarted(const QUuid &uuid)
{
    AppInfo info = _apps->info(uuid);
    if (!info.uuid().isNull() && info.isJSKit()) {
        logger()->debug() << "Preparing to start JSKit app" << info.uuid() << info.shortName();
        _curApp = info;
        startJsApp();
    }
}

void JSKitManager::handleAppStopped(const QUuid &uuid)
{
    if (!_curApp.uuid().isNull()) {
        if (_curApp.uuid() != uuid) {
            logger()->warn() << "Closed app with invalid UUID";
        }

        stopJsApp();
        _curApp.setUuid(QUuid()); // Clear the uuid to force invalid app
    }
}

void JSKitManager::startJsApp()
{
    if (_engine) stopJsApp();
    if (_curApp.uuid().isNull()) {
        logger()->warn() << "Attempting to start JS app with invalid UUID";
        return;
    }

    _engine = new QJSEngine(this);
    _jspebble = new JSKitPebble(this);
    _jsstorage = new JSKitLocalStorage(_curApp.uuid(), this);

    logger()->debug() << "starting JS app";

    QJSValue globalObj = _engine->globalObject();

    globalObj.setProperty("Pebble", _engine->newQObject(_jspebble));
    globalObj.setProperty("localStorage", _engine->newQObject(_jsstorage));


    QFile scriptFile(_curApp.path() + "/pebble-js-app.js");
    if (!scriptFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        logger()->warn() << "Failed to open JS file at:" << scriptFile.fileName();
        stopJsApp();
        return;
    }

    QString script = QString::fromUtf8(scriptFile.readAll());

    QJSValue result = _engine->evaluate(script, scriptFile.fileName());
    if (result.isError()) {
        logger()->warn() << "error while evaluating JSKit script:" << result.toString();
    }

    logger()->debug() << "JS script evaluated";
}

void JSKitManager::stopJsApp()
{
    if (!_engine) return; // Nothing to do!

    logger()->debug() << "stopping JS app";

    _engine->collectGarbage();

    delete _engine;
    _engine = 0;
    delete _jsstorage;
    _jsstorage = 0;
    delete _jspebble;
    _jspebble = 0;
}
