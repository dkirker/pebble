#ifndef APPMANAGER_H
#define APPMANAGER_H

#include <QObject>
#include <QHash>
#include <QUuid>
#include <QFileSystemWatcher>
#include <QLoggingCategory>
#include "appinfo.h"

class AppManager : public QObject
{
    Q_OBJECT
    QLoggingCategory l;

public:
    explicit AppManager(QObject *parent = 0);

    QStringList appPaths() const;
    QList<QUuid> appUuids() const;

    AppInfo info(const QUuid &uuid) const;
    AppInfo info(const QString &shortName) const;

    void insertAppInfo(const AppInfo &info);

public slots:
    void rescan();

signals:
    void appsChanged();

private:
    void scanApp(const QString &path);

private:
    QFileSystemWatcher *_watcher;
    QHash<QUuid, AppInfo> _apps;
    QHash<QString, QUuid> _names;
};

#endif // APPMANAGER_H
