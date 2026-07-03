#pragma once
#include <QList>
#include <QMap>
#include <QString>
#include <QStringList>
#include <QVariant>
#include <QVariantList>
#include <QVariantMap>

// Device type number → name (e.g. 3 → "RW")
extern const QMap<int,QString> kDeviceTypeNames;
// Device type number → devices_specific.ini prefix (e.g. 3 → "rw")
extern const QMap<int,QString> kDeviceSpecPrefix;

class NodeData {
public:
    QString      nodeName;
    QVariantMap  meta;        // node.ini flat keys
    QVariantList pieces;      // list of QVariantMap
    QVariantList devices;     // list of QVariantMap (includes "type_name" key)
    QVariantMap  devSpecFlat; // raw devices_specific.ini (kept intact on save)
    QVariantList vertices;    // list of QVariantList [x,y,z]
    QVariantList faces;       // list of QVariantMap
    QVariantList strucs;      // list of QVariantMap
    QVariantList triangles;   // list of QVariantMap (display-only)
    QVariantList ports;       // list of QVariantMap

    bool load(const QString& nodeDir);
    bool save(const QString& nodeDir) const;

    static QStringList listNodes(const QString& nodesDir);

private:
    static QVariantMap  loadIni (const QString& path);
    static void         saveIni (const QString& path, const QVariantMap& data);

    static QVariantList parseIndexed (const QVariantMap& flat,
                                      const QString& prefix,
                                      const QStringList& fields);
    static QVariantMap  serializeIndexed (const QVariantList& items,
                                          const QString& prefix);
    static QVariantList parseSimple      (const QVariantMap& flat,
                                          const QString& prefix);
    static QVariantMap  serializeSimple  (const QVariantList& items,
                                          const QString& prefix);
    static QVariantList parseTriangles   (const QVariantMap& flat);
};
