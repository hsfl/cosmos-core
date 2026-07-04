#include "nodedata.h"
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QRegularExpression>

// ---------------------------------------------------------------------------
// Device type tables
// ---------------------------------------------------------------------------

const QMap<int,QString> kDeviceTypeNames = {
    {0,"PLOAD"},{1,"SSEN"},{2,"IMU"},{3,"RW"},{4,"MTR"},{5,"CPU"},
    {6,"GPS"},{7,"ANT"},{8,"RXR"},{9,"TXR"},{10,"TCV"},{11,"PVSTRG"},
    {12,"BATT"},{13,"HTR"},{14,"MOTR"},{15,"TSEN"},{16,"THST"},{17,"PROP"},
    {18,"SWCH"},{19,"ROT"},{20,"STT"},{21,"MCC"},{22,"TCU"},{23,"BUS"},
    {24,"PSEN"},{25,"SUCHI"},{26,"CAM"},{27,"TELEM"},{28,"DISK"},{29,"TNC"},
    {30,"BCREG"},{31,"GYRO"},{32,"MAG"},{33,"XYZSEN"},{34,"NTELEM"},
    {35,"STELEM"},{36,"COOLER"},{37,"LENS"},{38,"BB"},{65535,"NONE"}
};

const QMap<int,QString> kDeviceSpecPrefix = {
    {0,"pload"},{1,"ssen"},{2,"imu"},{3,"rw"},{4,"mtr"},{5,"cpu"},
    {6,"gps"},{7,"ant"},{8,"rxr"},{9,"txr"},{10,"tcv"},{11,"pvstrg"},
    {12,"batt"},{13,"htr"},{14,"motr"},{16,"thst"},{17,"prop"},
    {18,"swch"},{20,"stt"},{21,"mcc"},{22,"tcu"},{23,"bus"},
    {24,"psen"},{26,"cam"},{28,"disk"},{29,"tnc"},
    {30,"bcreg"},{31,"gyro"},{32,"mag"},{33,"xyzsen"}
};

// ---------------------------------------------------------------------------
// I/O helpers
// ---------------------------------------------------------------------------

QVariantMap NodeData::loadIni(const QString& path)
{
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly)) return {};
    auto doc = QJsonDocument::fromJson(f.readAll());
    return doc.isObject() ? doc.object().toVariantMap() : QVariantMap{};
}

void NodeData::saveIni(const QString& path, const QVariantMap& data)
{
    QFile f(path);
    if (!f.open(QIODevice::WriteOnly)) return;
    f.write(QJsonDocument(QJsonObject::fromVariantMap(data))
              .toJson(QJsonDocument::Compact));
}

// ---------------------------------------------------------------------------
// Flat-namespace parsers
// ---------------------------------------------------------------------------

QVariantList NodeData::parseIndexed(const QVariantMap& flat,
                                    const QString& prefix,
                                    const QStringList& fields)
{
    if (flat.isEmpty()) return {};

    // Find max row index
    int maxIdx = -1;
    QRegularExpression anyRe(
        QString("^%1[a-z_]+_(\\d{3})(?:_\\d+)?$")
        .arg(QRegularExpression::escape(prefix)));
    for (const QString& k : flat.keys()) {
        auto m = anyRe.match(k);
        if (m.hasMatch())
            maxIdx = qMax(maxIdx, m.captured(1).toInt());
    }
    if (maxIdx < 0) return {};

    QList<QVariantMap> rows(maxIdx + 1);

    static QRegularExpression scalarTail("^(\\d{3})$");
    static QRegularExpression nestedTail("^(\\d{3})_(\\d+)$");

    for (const QString& field : fields) {
        QString fpfx = prefix + field + "_";
        for (auto it = flat.constBegin(); it != flat.constEnd(); ++it) {
            const QString& k = it.key();
            if (!k.startsWith(fpfx)) continue;
            QString tail = k.mid(fpfx.length());

            auto nm = nestedTail.match(tail);
            if (nm.hasMatch()) {
                int idx = nm.captured(1).toInt();
                int sub = nm.captured(2).toInt();
                if (idx > maxIdx) continue;
                QVariantList arr = rows[idx].value(field).toList();
                while (arr.size() <= sub) arr.append(QVariant());
                arr[sub] = it.value();
                rows[idx][field] = arr;
                continue;
            }
            auto sm = scalarTail.match(tail);
            if (sm.hasMatch()) {
                int idx = sm.captured(1).toInt();
                if (idx <= maxIdx) rows[idx][field] = it.value();
            }
        }
    }

    QVariantList out;
    out.reserve(rows.size());
    for (auto& r : rows) out.append(r);
    return out;
}

QVariantMap NodeData::serializeIndexed(const QVariantList& items,
                                        const QString& prefix)
{
    QVariantMap out;
    for (int i = 0; i < items.size(); i++) {
        const QVariantMap item = items[i].toMap();
        for (auto it = item.constBegin(); it != item.constEnd(); ++it) {
            if (it.value().type() == QVariant::List) {
                const QVariantList arr = it.value().toList();
                for (int j = 0; j < arr.size(); j++) {
                    out[QString("%1%2_%3_%4")
                        .arg(prefix, it.key(),
                             QString::number(i).rightJustified(3,'0'),
                             QString::number(j).rightJustified(3,'0'))] = arr[j];
                }
            } else {
                out[QString("%1%2_%3")
                    .arg(prefix, it.key(),
                         QString::number(i).rightJustified(3,'0'))] = it.value();
            }
        }
    }
    return out;
}

QVariantList NodeData::parseSimple(const QVariantMap& flat, const QString& prefix)
{
    QMap<int,QVariant> tmp;
    for (auto it = flat.constBegin(); it != flat.constEnd(); ++it) {
        const QString& k = it.key();
        if (!k.startsWith(prefix)) continue;
        bool ok;
        int idx = k.mid(prefix.length()).toInt(&ok);
        if (ok) tmp[idx] = it.value();
    }
    QVariantList out;
    for (const QVariant& v : tmp.values()) out.append(v);
    return out;
}

QVariantMap NodeData::serializeSimple(const QVariantList& items, const QString& prefix)
{
    QVariantMap out;
    for (int i = 0; i < items.size(); i++)
        out[prefix + QString::number(i).rightJustified(3,'0')] = items[i];
    return out;
}

QVariantList NodeData::parseTriangles(const QVariantMap& flat)
{
    QMap<int,QVariantMap> tmp;
    QRegularExpression re("^triangle_(.+)_(\\d{3})$");
    for (auto it = flat.constBegin(); it != flat.constEnd(); ++it) {
        auto m = re.match(it.key());
        if (!m.hasMatch()) continue;
        tmp[m.captured(2).toInt()][m.captured(1)] = it.value();
    }
    QVariantList out;
    for (const QVariantMap& v : tmp.values()) out.append(v);
    return out;
}

// ---------------------------------------------------------------------------
// Field name lists
// ---------------------------------------------------------------------------

static const QStringList kPieceFields = {
    "name","cidx","mass","density","emi","abs","hcap","hcon",
    "dim","area","volume","com","struc_idx"
};
static const QStringList kDevGenFields = {
    "type","name","model","didx","pidx","bidx","addr",
    "portidx","nvolt","namp","flag"
};
static const QStringList kFaceFields  = {
    "normal","com","area","triangle_cnt","triangle_idx"
};
static const QStringList kStrucFields = {
    "com","mass","volume","area","face_cnt","face_idx"
};
static const QStringList kPortFields  = {"name","type","utc"};

// ---------------------------------------------------------------------------
// Load / Save
// ---------------------------------------------------------------------------

bool NodeData::load(const QString& nodeDir)
{
    QDir d(nodeDir);
    nodeName = d.dirName();

    meta = loadIni(d.filePath("node.ini"));
    meta["node_name"] = nodeName;

    pieces = parseIndexed(loadIni(d.filePath("pieces.ini")), "piece_", kPieceFields);

    QVariantList rawDevs = parseIndexed(
        loadIni(d.filePath("devices_general.ini")), "device_all_", kDevGenFields);
    devices.clear();
    for (const QVariant& v : rawDevs) {
        QVariantMap dev = v.toMap();
        int t = dev.value("type", 65535).toInt();
        dev["type_name"] = kDeviceTypeNames.value(t, "NONE");
        devices.append(dev);
    }

    devSpecFlat = loadIni(d.filePath("devices_specific.ini"));
    vertices    = parseSimple(loadIni(d.filePath("vertices.ini")), "vertex_");
    faces       = parseIndexed(loadIni(d.filePath("faces.ini")),   "face_",  kFaceFields);
    strucs      = parseIndexed(loadIni(d.filePath("strucs.ini")),  "struc_", kStrucFields);
    triangles   = parseTriangles(loadIni(d.filePath("triangles.ini")));
    ports       = parseIndexed(loadIni(d.filePath("ports.ini")),   "port_",  kPortFields);
    return true;
}

bool NodeData::save(const QString& nodeDir) const
{
    QDir().mkpath(nodeDir);
    QDir d(nodeDir);

    QVariantMap m = meta;
    m.remove("node_name");
    m["node_piece_cnt"]    = pieces.size();
    m["node_device_cnt"]   = devices.size();
    m["node_vertex_cnt"]   = vertices.size();
    m["node_face_cnt"]     = faces.size();
    m["node_struc_cnt"]    = strucs.size();
    m["node_triangle_cnt"] = triangles.size();
    m["node_port_cnt"]     = ports.size();
    saveIni(d.filePath("node.ini"), m);

    saveIni(d.filePath("pieces.ini"), serializeIndexed(pieces, "piece_"));

    QVariantList devsSave;
    for (const QVariant& v : devices) {
        QVariantMap dev = v.toMap();
        dev.remove("type_name");
        devsSave.append(dev);
    }
    saveIni(d.filePath("devices_general.ini"), serializeIndexed(devsSave, "device_all_"));
    saveIni(d.filePath("devices_specific.ini"), devSpecFlat);
    saveIni(d.filePath("vertices.ini"),  serializeSimple(vertices, "vertex_"));
    saveIni(d.filePath("faces.ini"),     serializeIndexed(faces,   "face_"));
    saveIni(d.filePath("strucs.ini"),    serializeIndexed(strucs,  "struc_"));
    saveIni(d.filePath("triangles.ini"), serializeIndexed(triangles, "triangle_"));
    saveIni(d.filePath("ports.ini"),     serializeIndexed(ports,   "port_"));
    return true;
}

QStringList NodeData::listNodes(const QString& nodesDir)
{
    QDir dir(nodesDir);
    QStringList result;
    for (const QString& name :
         dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name))
    {
        if (QFile::exists(dir.filePath(name + "/node.ini")))
            result.append(name);
    }
    return result;
}
