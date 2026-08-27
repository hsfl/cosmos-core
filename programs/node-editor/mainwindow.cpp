#include "mainwindow.h"

#include <QAction>
#include <QHBoxLayout>
#include <QJsonDocument>
#include <QHeaderView>
#include <QInputDialog>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QScrollArea>
#include <QSplitter>
#include <QTabWidget>
#include <QTableWidget>
#include <QToolBar>
#include <QVBoxLayout>
#include <cmath>

// ============================================================================
// Static helpers
// ============================================================================

static QString fmtNum(double v)
{
    if (v == 0.0) return "0";
    if (std::abs(v) >= 1e-4 && std::abs(v) < 1e7)
        return QString::number(v, 'g', 6);
    return QString::number(v, 'e', 4);
}

static QString variantToStr(const QVariant& v)
{
    if (v.type() == QVariant::List) {
        QStringList parts;
        for (const QVariant& sv : v.toList())
            parts << fmtNum(sv.toDouble());
        return "[" + parts.join(", ") + "]";
    }
    if (v.type() == QVariant::Double || v.canConvert<double>()) {
        bool ok;
        double d = v.toDouble(&ok);
        if (ok && v.type() != QVariant::String) return fmtNum(d);
    }
    return v.toString();
}

static QVariantList parseVec3(QLineEdit* x, QLineEdit* y, QLineEdit* z)
{
    return { x->text().toDouble(), y->text().toDouble(), z->text().toDouble() };
}

static QVariantList parseIntList(const QString& s)
{
    QVariantList out;
    for (const QString& t : s.split(',', Qt::SkipEmptyParts)) {
        bool ok;
        int v = t.trimmed().toInt(&ok);
        if (ok) out.append(v);
    }
    return out;
}

static QString intListToStr(const QVariantList& lst)
{
    QStringList parts;
    for (const QVariant& v : lst) parts << v.toString();
    return parts.join(", ");
}

static const QString kDark = R"(
QWidget          { background:#1a1d23; color:#cdd3de; font-size:12px; }
QMainWindow      { background:#12141a; }
QToolBar         { background:#12141a; border-bottom:1px solid #2d3140; spacing:6px; padding:4px; }
QTabWidget::pane { border:1px solid #2d3140; background:#1a1d23; }
QTabBar::tab     { background:#1a1d23; color:#7a8599; padding:6px 18px;
                   border:1px solid #2d3140; border-bottom:none;
                   border-radius:4px 4px 0 0; }
QTabBar::tab:selected { background:#1e2130; color:#cdd3de; }
QTabBar::tab:hover:!selected { background:#1e2130; }
QTableWidget     { background:#1e2130; gridline-color:#22263a;
                   selection-background-color:#1e2a40; alternate-background-color:#1c1f2a; }
QTableWidget::item { padding:3px 6px; }
QTableWidget::item:selected { background:#1e2a40; color:#cdd3de; }
QHeaderView::section { background:#1e2130; color:#7eb8f7; font-weight:bold;
                        padding:5px 8px; border:none; border-right:1px solid #2d3140;
                        border-bottom:2px solid #2d3140; }
QLineEdit        { background:#1e2130; border:1px solid #3a3f55;
                   border-radius:3px; padding:3px 7px; color:#cdd3de; }
QLineEdit:focus  { border-color:#3b6fd4; }
QComboBox        { background:#1e2130; border:1px solid #3a3f55;
                   border-radius:3px; padding:3px 7px; color:#cdd3de; }
QComboBox::drop-down { border:none; }
QComboBox QAbstractItemView { background:#1e2130; color:#cdd3de;
                               selection-background-color:#1e2a40; }
QPushButton      { background:#3b6fd4; color:white; border:none;
                   border-radius:4px; padding:5px 14px; font-weight:500; }
QPushButton:hover { background:#4f80e1; }
QPushButton[role="danger"] { background:#8b2c2c; }
QPushButton[role="danger"]:hover { background:#a83333; }
QPushButton[role="secondary"] { background:#2a2f44; color:#cdd3de; }
QPushButton[role="secondary"]:hover { background:#353b55; }
QScrollBar:vertical   { background:#12141a; width:7px; }
QScrollBar::handle:vertical { background:#3a3f55; border-radius:3px; min-height:20px; }
QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height:0; }
QScrollBar:horizontal { background:#12141a; height:7px; }
QScrollBar::handle:horizontal { background:#3a3f55; border-radius:3px; }
QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal { width:0; }
QSplitter::handle  { background:#2d3140; }
QGroupBox          { border:1px solid #2d3140; border-radius:4px; margin-top:8px;
                     color:#7eb8f7; font-weight:600; }
QGroupBox::title   { subcontrol-origin:margin; left:8px; padding:0 4px; }
QLabel[role="header"] { color:#7eb8f7; font-weight:600; font-size:11px;
                         text-transform:uppercase; border-bottom:1px solid #2d3140;
                         padding-bottom:3px; margin-top:6px; }
QLabel[role="status"] { color:#7a8599; font-size:11px; }
QLabel[role="statusErr"] { color:#e07070; font-size:11px; }
)";

// ============================================================================
// Constructor / setupUi
// ============================================================================

MainWindow::MainWindow(const QString& nodesDir, QWidget* parent)
    : QMainWindow(parent), m_nodesDir(nodesDir)
{
    setStyleSheet(kDark);
    setWindowTitle("COSMOS Node Editor");
    resize(1400, 750);
    setupUi();
    refreshNodeList();
}

void MainWindow::setupUi()
{
    // ---- Toolbar ----
    auto* tb = addToolBar("Main");
    tb->setMovable(false);

    auto* lbl = new QLabel("  Node: ");
    lbl->setStyleSheet("color:#7a8599;");
    tb->addWidget(lbl);

    m_nodeSelector = new QComboBox;
    m_nodeSelector->setFixedWidth(180);
    m_nodeSelector->addItem("— select —");
    tb->addWidget(m_nodeSelector);
    connect(m_nodeSelector, &QComboBox::currentTextChanged,
            this, &MainWindow::onNodeSelected);

    tb->addSeparator();

    auto* btnNew = new QPushButton("New Node");
    btnNew->setProperty("role","secondary");
    tb->addWidget(btnNew);
    connect(btnNew, &QPushButton::clicked, this, &MainWindow::onNewNode);

    auto* btnSave = new QPushButton("Save");
    tb->addWidget(btnSave);
    connect(btnSave, &QPushButton::clicked, this, &MainWindow::onSave);

    tb->addSeparator();
    m_statusLabel = new QLabel;
    m_statusLabel->setProperty("role","status");
    tb->addWidget(m_statusLabel);

    // ---- Central tabs ----
    m_tabs = new QTabWidget;
    m_tabs->addTab(makeNodeTab(),     "Node");
    m_tabs->addTab(makePiecesTab(),   "Pieces");
    m_tabs->addTab(makeDevicesTab(),  "Devices");
    m_tabs->addTab(makeGeometryTab(), "Geometry");
    m_tabs->addTab(makePortsTab(),    "Ports");
    setCentralWidget(m_tabs);
}

// ============================================================================
// Form builder helpers
// ============================================================================

QScrollArea* MainWindow::makeFormScroll(QWidget* content)
{
    auto* sa = new QScrollArea;
    sa->setWidgetResizable(true);
    sa->setFrameShape(QFrame::NoFrame);
    sa->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    sa->setWidget(content);
    return sa;
}

QWidget* MainWindow::makeSectionHeader(const QString& title)
{
    auto* l = new QLabel(title.toUpper());
    l->setProperty("role","header");
    return l;
}

QLineEdit* MainWindow::addRow(QFormLayout* fl, const QString& label)
{
    auto* e = new QLineEdit;
    fl->addRow(label + ":", e);
    return e;
}

QWidget* MainWindow::makeVec3Row(QFormLayout* fl, const QString& label,
                                  QLineEdit** x, QLineEdit** y, QLineEdit** z)
{
    auto* w = new QWidget;
    auto* h = new QHBoxLayout(w);
    h->setContentsMargins(0,0,0,0); h->setSpacing(4);
    *x = new QLineEdit; (*x)->setPlaceholderText("x");
    *y = new QLineEdit; (*y)->setPlaceholderText("y");
    *z = new QLineEdit; (*z)->setPlaceholderText("z");
    h->addWidget(*x); h->addWidget(*y); h->addWidget(*z);
    fl->addRow(label + ":", w);
    return w;
}

// ============================================================================
// Tab builders
// ============================================================================

QWidget* MainWindow::makeNodeTab()
{
    auto* w = new QWidget;
    auto* v = new QVBoxLayout(w);
    v->setContentsMargins(20,16,20,16);

    auto* fl = new QFormLayout;
    fl->setLabelAlignment(Qt::AlignRight);
    fl->setSpacing(8);
    m_nodeName = addRow(fl, "Name");
    m_nodeType = addRow(fl, "Type");
    v->addLayout(fl);
    v->addSpacing(12);

    auto* btn = new QPushButton("Apply");
    btn->setFixedWidth(100);
    connect(btn, &QPushButton::clicked, this, [this]{
        if (!m_loaded) return;
        m_data.meta["node_name"] = m_nodeName->text().trimmed();
        m_data.meta["node_type"] = m_nodeType->text().toInt();
        setStatus("Node meta updated");
    });
    v->addWidget(btn, 0, Qt::AlignLeft);
    v->addStretch();
    return w;
}

// ---- helper: button bar at bottom of a form panel ----
static QWidget* makeButtonBar(const char* applySlot, const char* addSlot,
                               const char* delSlot, QObject* recv)
{
    auto* w = new QWidget;
    auto* h = new QHBoxLayout(w);
    h->setContentsMargins(0,8,0,4); h->setSpacing(8);

    auto* apply = new QPushButton("Apply");
    auto* add   = new QPushButton("+ Add");
    add->setProperty("role","secondary");
    auto* del   = new QPushButton("Delete");
    del->setProperty("role","danger");

    QObject::connect(apply, SIGNAL(clicked()), recv, applySlot);
    QObject::connect(add,   SIGNAL(clicked()), recv, addSlot);
    QObject::connect(del,   SIGNAL(clicked()), recv, delSlot);

    h->addWidget(apply); h->addWidget(add); h->addWidget(del);
    h->addStretch();
    return w;
}

QWidget* MainWindow::makePiecesTab()
{
    auto* split = new QSplitter(Qt::Horizontal);

    // ---- Table ----
    m_piecesTable = new QTableWidget(0, 10);
    m_piecesTable->setHorizontalHeaderLabels({
        "#","Name","Mass","Area","Volume","Density","Emi","Abs","CIdx","SIdx"});
    m_piecesTable->horizontalHeader()->setStretchLastSection(true);
    m_piecesTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_piecesTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_piecesTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_piecesTable->setAlternatingRowColors(true);
    m_piecesTable->verticalHeader()->setVisible(false);
    connect(m_piecesTable, &QTableWidget::cellClicked,
            this, &MainWindow::onPieceSelected);
    split->addWidget(m_piecesTable);

    // ---- Form ----
    auto* formW = new QWidget;
    formW->setMinimumWidth(340);
    auto* vl = new QVBoxLayout(formW);
    vl->setContentsMargins(12,8,12,8); vl->setSpacing(4);

    auto* fl1 = new QFormLayout; fl1->setSpacing(6); fl1->setLabelAlignment(Qt::AlignRight);
    vl->addWidget(makeSectionHeader("Identity")); vl->addLayout(fl1);
    m_pName    = addRow(fl1, "Name");
    m_pCidx    = addRow(fl1, "Comp idx");
    m_pStrucIdx= addRow(fl1, "Struc idx");

    auto* fl2 = new QFormLayout; fl2->setSpacing(6); fl2->setLabelAlignment(Qt::AlignRight);
    vl->addWidget(makeSectionHeader("Physical")); vl->addLayout(fl2);
    m_pMass    = addRow(fl2, "Mass (kg)");
    m_pDensity = addRow(fl2, "Density (kg/m³)");
    m_pArea    = addRow(fl2, "Area (m²)");
    m_pVolume  = addRow(fl2, "Volume (m³)");
    m_pDim     = addRow(fl2, "Dim");
    makeVec3Row(fl2, "CoM (m)", &m_pComX, &m_pComY, &m_pComZ);

    auto* fl3 = new QFormLayout; fl3->setSpacing(6); fl3->setLabelAlignment(Qt::AlignRight);
    vl->addWidget(makeSectionHeader("Thermal / Optical")); vl->addLayout(fl3);
    m_pEmi  = addRow(fl3, "Emissivity");
    m_pAbs  = addRow(fl3, "Absorptivity");
    m_pHcap = addRow(fl3, "Heat cap (J/kg·K)");
    m_pHcon = addRow(fl3, "Heat con (W/m·K)");

    vl->addWidget(makeButtonBar(SLOT(onApplyPiece()), SLOT(onAddPiece()),
                                SLOT(onDeletePiece()), this));
    vl->addStretch();
    split->addWidget(makeFormScroll(formW));
    split->setStretchFactor(0,3); split->setStretchFactor(1,2);
    return split;
}

QWidget* MainWindow::makeDevicesTab()
{
    auto* split = new QSplitter(Qt::Horizontal);

    m_devicesTable = new QTableWidget(0, 8);
    m_devicesTable->setHorizontalHeaderLabels({
        "#","Name","Type","PIdx","BIdx","NomV","NomA","PortIdx"});
    m_devicesTable->horizontalHeader()->setStretchLastSection(true);
    m_devicesTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_devicesTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_devicesTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_devicesTable->setAlternatingRowColors(true);
    m_devicesTable->verticalHeader()->setVisible(false);
    connect(m_devicesTable, &QTableWidget::cellClicked,
            this, &MainWindow::onDeviceSelected);
    split->addWidget(m_devicesTable);

    auto* formW = new QWidget;
    formW->setMinimumWidth(340);
    auto* vl = new QVBoxLayout(formW);
    vl->setContentsMargins(12,8,12,8); vl->setSpacing(4);

    auto* fl1 = new QFormLayout; fl1->setSpacing(6); fl1->setLabelAlignment(Qt::AlignRight);
    vl->addWidget(makeSectionHeader("Identity")); vl->addLayout(fl1);
    m_dName = addRow(fl1, "Name");
    m_dType = new QComboBox;
    for (auto it = kDeviceTypeNames.constBegin(); it != kDeviceTypeNames.constEnd(); ++it)
        m_dType->addItem(QString("%1 (%2)").arg(it.value()).arg(it.key()), it.key());
    fl1->addRow("Type:", m_dType);
    connect(m_dType, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onDeviceTypeChanged);
    m_dModel = addRow(fl1, "Model");
    m_dDidx  = addRow(fl1, "Type index");

    auto* fl2 = new QFormLayout; fl2->setSpacing(6); fl2->setLabelAlignment(Qt::AlignRight);
    vl->addWidget(makeSectionHeader("Connections")); vl->addLayout(fl2);
    m_dPidx   = addRow(fl2, "Piece idx");
    m_dBidx   = addRow(fl2, "Bus idx");
    m_dAddr   = addRow(fl2, "Address");
    m_dPortidx= addRow(fl2, "Port idx");

    auto* fl3 = new QFormLayout; fl3->setSpacing(6); fl3->setLabelAlignment(Qt::AlignRight);
    vl->addWidget(makeSectionHeader("Electrical")); vl->addLayout(fl3);
    m_dNvolt = addRow(fl3, "Nom voltage (V)");
    m_dNamp  = addRow(fl3, "Nom current (A)");
    m_dFlag  = addRow(fl3, "Flags");

    // Device-specific group (populated dynamically)
    m_devSpecGroup = new QGroupBox("Device-specific");
    m_devSpecGroup->setVisible(false);
    m_devSpecLayout = new QFormLayout(m_devSpecGroup);
    m_devSpecLayout->setSpacing(5);
    m_devSpecLayout->setLabelAlignment(Qt::AlignRight);
    vl->addWidget(m_devSpecGroup);

    vl->addWidget(makeButtonBar(SLOT(onApplyDevice()), SLOT(onAddDevice()),
                                SLOT(onDeleteDevice()), this));
    vl->addStretch();
    split->addWidget(makeFormScroll(formW));
    split->setStretchFactor(0,3); split->setStretchFactor(1,2);
    return split;
}

QWidget* MainWindow::makeGeometryTab()
{
    auto* geoTabs = new QTabWidget;

    // ---- Vertices ----
    {
        auto* split = new QSplitter(Qt::Horizontal);
        m_verticesTable = new QTableWidget(0, 4);
        m_verticesTable->setHorizontalHeaderLabels({"#","X (m)","Y (m)","Z (m)"});
        m_verticesTable->horizontalHeader()->setStretchLastSection(true);
        m_verticesTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        m_verticesTable->setSelectionMode(QAbstractItemView::SingleSelection);
        m_verticesTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        m_verticesTable->setAlternatingRowColors(true);
        m_verticesTable->verticalHeader()->setVisible(false);
        connect(m_verticesTable, &QTableWidget::cellClicked,
                this, &MainWindow::onVertexSelected);
        split->addWidget(m_verticesTable);

        auto* formW = new QWidget; formW->setMinimumWidth(260);
        auto* vl = new QVBoxLayout(formW);
        vl->setContentsMargins(12,8,12,8); vl->setSpacing(4);
        auto* fl = new QFormLayout; fl->setSpacing(6); fl->setLabelAlignment(Qt::AlignRight);
        vl->addWidget(makeSectionHeader("Position")); vl->addLayout(fl);
        m_vX = addRow(fl, "X (m)");
        m_vY = addRow(fl, "Y (m)");
        m_vZ = addRow(fl, "Z (m)");
        vl->addWidget(makeButtonBar(SLOT(onApplyVertex()), SLOT(onAddVertex()),
                                    SLOT(onDeleteVertex()), this));
        vl->addStretch();
        split->addWidget(makeFormScroll(formW));
        split->setStretchFactor(0,3); split->setStretchFactor(1,2);
        geoTabs->addTab(split, "Vertices");
    }

    // ---- Faces ----
    {
        auto* split = new QSplitter(Qt::Horizontal);
        m_facesTable = new QTableWidget(0, 6);
        m_facesTable->setHorizontalHeaderLabels({
            "#","Area","Com X","Com Y","Com Z","Triangles"});
        m_facesTable->horizontalHeader()->setStretchLastSection(true);
        m_facesTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        m_facesTable->setSelectionMode(QAbstractItemView::SingleSelection);
        m_facesTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        m_facesTable->setAlternatingRowColors(true);
        m_facesTable->verticalHeader()->setVisible(false);
        connect(m_facesTable, &QTableWidget::cellClicked,
                this, &MainWindow::onFaceSelected);
        split->addWidget(m_facesTable);

        auto* formW = new QWidget; formW->setMinimumWidth(300);
        auto* vl = new QVBoxLayout(formW);
        vl->setContentsMargins(12,8,12,8); vl->setSpacing(4);

        auto* fl1 = new QFormLayout; fl1->setSpacing(6); fl1->setLabelAlignment(Qt::AlignRight);
        vl->addWidget(makeSectionHeader("Geometry")); vl->addLayout(fl1);
        m_fArea = addRow(fl1, "Area (m²)");
        makeVec3Row(fl1, "Normal", &m_fNX, &m_fNY, &m_fNZ);
        makeVec3Row(fl1, "CoM (m)", &m_fCX, &m_fCY, &m_fCZ);

        auto* fl2 = new QFormLayout; fl2->setSpacing(6); fl2->setLabelAlignment(Qt::AlignRight);
        vl->addWidget(makeSectionHeader("Triangle Indices")); vl->addLayout(fl2);
        m_fTris = addRow(fl2, "Indices");

        vl->addWidget(makeButtonBar(SLOT(onApplyFace()), SLOT(onAddFace()),
                                    SLOT(onDeleteFace()), this));
        vl->addStretch();
        split->addWidget(makeFormScroll(formW));
        split->setStretchFactor(0,3); split->setStretchFactor(1,2);
        geoTabs->addTab(split, "Faces");
    }

    // ---- Strucs ----
    {
        auto* split = new QSplitter(Qt::Horizontal);
        m_strucsTable = new QTableWidget(0, 5);
        m_strucsTable->setHorizontalHeaderLabels({
            "#","Mass","Volume","Area","Faces"});
        m_strucsTable->horizontalHeader()->setStretchLastSection(true);
        m_strucsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        m_strucsTable->setSelectionMode(QAbstractItemView::SingleSelection);
        m_strucsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        m_strucsTable->setAlternatingRowColors(true);
        m_strucsTable->verticalHeader()->setVisible(false);
        connect(m_strucsTable, &QTableWidget::cellClicked,
                this, &MainWindow::onStrucSelected);
        split->addWidget(m_strucsTable);

        auto* formW = new QWidget; formW->setMinimumWidth(300);
        auto* vl = new QVBoxLayout(formW);
        vl->setContentsMargins(12,8,12,8); vl->setSpacing(4);

        auto* fl1 = new QFormLayout; fl1->setSpacing(6); fl1->setLabelAlignment(Qt::AlignRight);
        vl->addWidget(makeSectionHeader("Physical")); vl->addLayout(fl1);
        m_sMass = addRow(fl1, "Mass (kg)");
        m_sVol  = addRow(fl1, "Volume (m³)");
        m_sArea = addRow(fl1, "Area (m²)");
        makeVec3Row(fl1, "CoM (m)", &m_sCX, &m_sCY, &m_sCZ);

        auto* fl2 = new QFormLayout; fl2->setSpacing(6); fl2->setLabelAlignment(Qt::AlignRight);
        vl->addWidget(makeSectionHeader("Face Indices")); vl->addLayout(fl2);
        m_sFaces = addRow(fl2, "Indices");

        vl->addWidget(makeButtonBar(SLOT(onApplyStruc()), SLOT(onAddStruc()),
                                    SLOT(onDeleteStruc()), this));
        vl->addStretch();
        split->addWidget(makeFormScroll(formW));
        split->setStretchFactor(0,3); split->setStretchFactor(1,2);
        geoTabs->addTab(split, "Strucs");
    }

    // ---- Triangles (read-only) ----
    {
        m_trianglesTable = new QTableWidget(0, 1);
        m_trianglesTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        m_trianglesTable->setAlternatingRowColors(true);
        m_trianglesTable->verticalHeader()->setVisible(false);
        geoTabs->addTab(m_trianglesTable, "Triangles");
    }

    return geoTabs;
}

QWidget* MainWindow::makePortsTab()
{
    auto* split = new QSplitter(Qt::Horizontal);

    m_portsTable = new QTableWidget(0, 4);
    m_portsTable->setHorizontalHeaderLabels({"#","Name","Type","UTC"});
    m_portsTable->horizontalHeader()->setStretchLastSection(true);
    m_portsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_portsTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_portsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_portsTable->setAlternatingRowColors(true);
    m_portsTable->verticalHeader()->setVisible(false);
    connect(m_portsTable, &QTableWidget::cellClicked,
            this, &MainWindow::onPortSelected);
    split->addWidget(m_portsTable);

    auto* formW = new QWidget; formW->setMinimumWidth(260);
    auto* vl = new QVBoxLayout(formW);
    vl->setContentsMargins(12,8,12,8); vl->setSpacing(4);
    auto* fl = new QFormLayout; fl->setSpacing(6); fl->setLabelAlignment(Qt::AlignRight);
    vl->addWidget(makeSectionHeader("Port")); vl->addLayout(fl);
    m_ptName = addRow(fl, "Name");
    m_ptType = addRow(fl, "Type");
    m_ptUtc  = addRow(fl, "UTC");
    vl->addWidget(makeButtonBar(SLOT(onApplyPort()), SLOT(onAddPort()),
                                SLOT(onDeletePort()), this));
    vl->addStretch();
    split->addWidget(makeFormScroll(formW));
    split->setStretchFactor(0,3); split->setStretchFactor(1,2);
    return split;
}

// ============================================================================
// Node list / load / save
// ============================================================================

void MainWindow::refreshNodeList()
{
    QStringList nodes = NodeData::listNodes(m_nodesDir);
    {
        QSignalBlocker blocker(m_nodeSelector);
        m_nodeSelector->clear();
        m_nodeSelector->addItem("— select —");
        for (const QString& n : nodes) m_nodeSelector->addItem(n);
    }
}

void MainWindow::onNodeSelected(const QString& name)
{
    if (name.isEmpty() || name.startsWith("—")) return;
    loadNode(m_nodesDir + "/" + name);
}

void MainWindow::onNewNode()
{
    bool ok;
    QString name = QInputDialog::getText(
        this, "New Node", "Node name:", QLineEdit::Normal, "", &ok);
    if (!ok || name.trimmed().isEmpty()) return;
    m_data = NodeData();
    m_data.nodeName = name.trimmed();
    m_data.meta["node_name"] = m_data.nodeName;
    m_data.meta["node_type"] = 0;
    m_loaded = true;
    m_selPiece = m_selDevice = m_selVertex =
    m_selFace  = m_selStruc = m_selPort = -1;
    populatePiecesTable(); populateDevicesTable();
    populateVerticesTable(); populateFacesTable();
    populateStrucsTable(); populatePortsTable();
    m_nodeName->setText(name.trimmed());
    m_nodeType->setText("0");
    setStatus("New node: " + name + " (unsaved)");
}

void MainWindow::onSave()
{
    if (!m_loaded) { setStatus("No node loaded", true); return; }
    QString dir = m_nodesDir + "/" + m_data.nodeName;
    if (m_data.save(dir)) {
        refreshNodeList();
        // Re-select in combobox
        QSignalBlocker b(m_nodeSelector);
        int idx = m_nodeSelector->findText(m_data.nodeName);
        if (idx >= 0) m_nodeSelector->setCurrentIndex(idx);
        setStatus("Saved: " + dir);
    } else {
        setStatus("Save failed", true);
    }
}

void MainWindow::loadNode(const QString& nodeDir)
{
    setStatus("Loading...");
    if (!m_data.load(nodeDir)) { setStatus("Failed to load " + nodeDir, true); return; }
    m_loaded = true;
    m_selPiece = m_selDevice = m_selVertex =
    m_selFace  = m_selStruc = m_selPort = -1;

    m_nodeName->setText(m_data.nodeName);
    m_nodeType->setText(m_data.meta.value("node_type","0").toString());

    populatePiecesTable();
    populateDevicesTable();
    populateVerticesTable();
    populateFacesTable();
    populateStrucsTable();
    populatePortsTable();
    setStatus("Loaded: " + m_data.nodeName +
              QString("  (%1 pieces, %2 devices)")
              .arg(m_data.pieces.size()).arg(m_data.devices.size()));
}

void MainWindow::setStatus(const QString& msg, bool error)
{
    m_statusLabel->setText(msg);
    m_statusLabel->setProperty("role", error ? "statusErr" : "status");
    m_statusLabel->style()->unpolish(m_statusLabel);
    m_statusLabel->style()->polish(m_statusLabel);
}

// ============================================================================
// Table helpers
// ============================================================================

void MainWindow::setTableItem(QTableWidget* t, int row, int col,
                               const QString& text)
{
    auto* item = t->item(row, col);
    if (!item) { item = new QTableWidgetItem; t->setItem(row, col, item); }
    item->setText(text);
    item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
}

// ============================================================================
// Pieces
// ============================================================================

void MainWindow::populatePiecesTable()
{
    auto* t = m_piecesTable;
    t->setRowCount(m_data.pieces.size());
    for (int i = 0; i < m_data.pieces.size(); i++) {
        QVariantMap p = m_data.pieces[i].toMap();
        setTableItem(t,i,0,QString::number(i));
        setTableItem(t,i,1,p.value("name").toString());
        setTableItem(t,i,2,fmtNum(p.value("mass").toDouble()));
        setTableItem(t,i,3,fmtNum(p.value("area").toDouble()));
        setTableItem(t,i,4,fmtNum(p.value("volume").toDouble()));
        setTableItem(t,i,5,fmtNum(p.value("density").toDouble()));
        setTableItem(t,i,6,fmtNum(p.value("emi").toDouble()));
        setTableItem(t,i,7,fmtNum(p.value("abs").toDouble()));
        int cidx = p.value("cidx",65535).toInt();
        setTableItem(t,i,8, cidx == 65535 ? "—" : QString::number(cidx));
        setTableItem(t,i,9,p.value("struc_idx","0").toString());
    }
    t->resizeColumnToContents(0);
}

void MainWindow::onPieceSelected(int row, int)
{
    m_selPiece = row;
    if (row < 0 || row >= m_data.pieces.size()) return;
    QVariantMap p = m_data.pieces[row].toMap();
    m_pName->setText(p.value("name").toString());
    m_pCidx->setText(p.value("cidx",65535).toString());
    m_pStrucIdx->setText(p.value("struc_idx",0).toString());
    m_pMass->setText(fmtNum(p.value("mass").toDouble()));
    m_pDensity->setText(fmtNum(p.value("density",2700).toDouble()));
    m_pArea->setText(fmtNum(p.value("area").toDouble()));
    m_pVolume->setText(fmtNum(p.value("volume").toDouble()));
    m_pDim->setText(p.value("dim",0).toString());
    QVariantList com = p.value("com").toList();
    m_pComX->setText(fmtNum(com.value(0).toDouble()));
    m_pComY->setText(fmtNum(com.value(1).toDouble()));
    m_pComZ->setText(fmtNum(com.value(2).toDouble()));
    m_pEmi->setText(fmtNum(p.value("emi",1).toDouble()));
    m_pAbs->setText(fmtNum(p.value("abs",1).toDouble()));
    m_pHcap->setText(fmtNum(p.value("hcap",0.9).toDouble()));
    m_pHcon->setText(fmtNum(p.value("hcon",205).toDouble()));
}

void MainWindow::applyPieceForm(int idx)
{
    QVariantMap p;
    p["name"]     = m_pName->text();
    p["cidx"]     = m_pCidx->text().toInt();
    p["struc_idx"]= m_pStrucIdx->text().toInt();
    p["mass"]     = m_pMass->text().toDouble();
    p["density"]  = m_pDensity->text().toDouble();
    p["area"]     = m_pArea->text().toDouble();
    p["volume"]   = m_pVolume->text().toDouble();
    p["dim"]      = m_pDim->text().toInt();
    p["com"]      = parseVec3(m_pComX, m_pComY, m_pComZ);
    p["emi"]      = m_pEmi->text().toDouble();
    p["abs"]      = m_pAbs->text().toDouble();
    p["hcap"]     = m_pHcap->text().toDouble();
    p["hcon"]     = m_pHcon->text().toDouble();
    m_data.pieces[idx] = p;
    populatePiecesTable();
    m_piecesTable->selectRow(idx);
}

void MainWindow::onApplyPiece()
{
    if (m_selPiece < 0 || m_selPiece >= m_data.pieces.size()) return;
    applyPieceForm(m_selPiece);
    setStatus("Piece updated");
}

void MainWindow::onAddPiece()
{
    if (!m_loaded) return;
    QVariantMap p;
    int n = m_data.pieces.size();
    p["name"]     = QString("piece%1").arg(n, 3, 10, QChar('0'));
    p["cidx"]     = 65535;
    p["mass"]     = 0.001; p["density"] = 2700.0;
    p["emi"]      = 1.0;   p["abs"]     = 1.0;
    p["hcap"]     = 0.9;   p["hcon"]    = 205.0;
    p["dim"]      = 0;     p["area"]    = 0.0;
    p["volume"]   = 0.0;   p["struc_idx"] = 0;
    p["com"]      = QVariantList{0.0, 0.0, 0.0};
    m_data.pieces.append(p);
    populatePiecesTable();
    int row = m_data.pieces.size() - 1;
    m_piecesTable->selectRow(row);
    onPieceSelected(row, 0);
    setStatus("Piece added");
}

void MainWindow::onDeletePiece()
{
    if (m_selPiece < 0 || m_selPiece >= m_data.pieces.size()) return;
    auto r = QMessageBox::question(this, "Delete Piece",
        QString("Delete piece %1?").arg(m_selPiece));
    if (r != QMessageBox::Yes) return;
    m_data.pieces.removeAt(m_selPiece);
    m_selPiece = -1;
    populatePiecesTable();
    setStatus("Piece deleted");
}

// ============================================================================
// Devices
// ============================================================================

void MainWindow::populateDevicesTable()
{
    // device-type color map
    static const QMap<QString,QString> typeColor = {
        {"RW","#2e4a7a"},{"MTR","#2e4a7a"},{"GYRO","#2e4a7a"},{"IMU","#2e4a7a"},
        {"STT","#2e4a7a"},{"SSEN","#2e4a7a"},{"XYZSEN","#2e4a7a"},{"MAG","#2e4a7a"},
        {"PVSTRG","#2a5c35"},{"BATT","#2a5c35"},
        {"CPU","#5c3a2a"},{"DISK","#5c3a2a"},
        {"TXR","#4a2e6a"},{"RXR","#4a2e6a"},{"TCV","#4a2e6a"},{"ANT","#4a2e6a"},
        {"THST","#6a4a00"},{"PROP","#6a4a00"},
        {"CAM","#4a3a00"},{"TSEN","#2a4a5c"},
    };

    auto* t = m_devicesTable;
    t->setRowCount(m_data.devices.size());
    for (int i = 0; i < m_data.devices.size(); i++) {
        QVariantMap d = m_data.devices[i].toMap();
        QString tname = d.value("type_name","?").toString();
        setTableItem(t,i,0,QString::number(i));
        setTableItem(t,i,1,d.value("name").toString());

        auto* typeItem = new QTableWidgetItem(tname);
        typeItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        if (typeColor.contains(tname))
            typeItem->setBackground(QColor(typeColor[tname]));
        typeItem->setForeground(QColor("#a0c8ff"));
        t->setItem(i,2,typeItem);

        setTableItem(t,i,3,d.value("pidx","").toString());
        setTableItem(t,i,4,d.value("bidx","0").toString());
        setTableItem(t,i,5,fmtNum(d.value("nvolt").toDouble()));
        setTableItem(t,i,6,fmtNum(d.value("namp").toDouble()));
        int ptidx = d.value("portidx",65535).toInt();
        setTableItem(t,i,7, ptidx == 65535 ? "—" : QString::number(ptidx));
    }
    t->resizeColumnToContents(0);
    t->resizeColumnToContents(2);
}

void MainWindow::onDeviceSelected(int row, int)
{
    m_selDevice = row;
    if (row < 0 || row >= m_data.devices.size()) return;
    QVariantMap d = m_data.devices[row].toMap();

    m_dName->setText(d.value("name").toString());

    int type = d.value("type",15).toInt();
    for (int i = 0; i < m_dType->count(); i++) {
        if (m_dType->itemData(i).toInt() == type) {
            QSignalBlocker b(m_dType);
            m_dType->setCurrentIndex(i);
            break;
        }
    }
    m_dModel->setText(d.value("model","0").toString());
    m_dDidx->setText(d.value("didx","0").toString());
    m_dPidx->setText(d.value("pidx","0").toString());
    m_dBidx->setText(d.value("bidx","0").toString());
    m_dAddr->setText(d.value("addr","0").toString());
    m_dPortidx->setText(d.value("portidx",65535).toString());
    m_dNvolt->setText(fmtNum(d.value("nvolt").toDouble()));
    m_dNamp->setText(fmtNum(d.value("namp").toDouble()));
    m_dFlag->setText(d.value("flag","0").toString());

    updateDeviceSpecGroup(row);
}

void MainWindow::onDeviceTypeChanged(int)
{
    if (m_selDevice >= 0) updateDeviceSpecGroup(m_selDevice);
}

void MainWindow::updateDeviceSpecGroup(int deviceIdx)
{
    // Clear existing spec rows
    while (m_devSpecLayout->rowCount() > 0)
        m_devSpecLayout->removeRow(0);

    if (deviceIdx < 0 || deviceIdx >= m_data.devices.size()) {
        m_devSpecGroup->setVisible(false);
        return;
    }

    int typeNum  = m_dType->currentData().toInt();
    QString pfx  = kDeviceSpecPrefix.value(typeNum);
    int didx     = m_dDidx->text().toInt();

    if (pfx.isEmpty() || m_data.devSpecFlat.isEmpty()) {
        m_devSpecGroup->setVisible(false);
        return;
    }

    // Collect keys for device_<pfx>_<field>_<didx_3digit>[_sub]
    QString keyPfx = QString("device_%1_").arg(pfx);
    QString idxStr = QString::number(didx).rightJustified(3,'0');

    QRegularExpression re(
        QString("^device_%1_(.+?)_%2(?:_(\\d+))?$")
        .arg(QRegularExpression::escape(pfx), idxStr));

    QMap<QString, QVariant> fields;
    for (auto it = m_data.devSpecFlat.constBegin();
         it != m_data.devSpecFlat.constEnd(); ++it)
    {
        auto m = re.match(it.key());
        if (!m.hasMatch()) continue;
        QString field  = m.captured(1);
        QString subStr = m.captured(2);
        if (subStr.isEmpty()) {
            fields[field] = it.value();
        } else {
            QVariantList arr = fields.value(field).toList();
            int sub = subStr.toInt();
            while (arr.size() <= sub) arr.append(QVariant());
            arr[sub] = it.value();
            fields[field] = arr;
        }
    }

    if (fields.isEmpty()) {
        m_devSpecGroup->setVisible(false);
        return;
    }

    m_devSpecGroup->setTitle(
        QString("Device-specific (%1, idx %2)").arg(pfx.toUpper()).arg(didx));

    for (auto it = fields.constBegin(); it != fields.constEnd(); ++it) {
        auto* edit = new QLineEdit(variantToStr(it.value()));
        edit->setObjectName("spec_" + it.key()); // used when applying
        m_devSpecLayout->addRow(it.key() + ":", edit);
    }

    m_devSpecGroup->setVisible(true);
}

void MainWindow::applyDeviceForm(int idx)
{
    QVariantMap d;
    int typeNum = m_dType->currentData().toInt();
    d["type"]    = typeNum;
    d["type_name"] = kDeviceTypeNames.value(typeNum, "NONE");
    d["name"]    = m_dName->text();
    d["model"]   = m_dModel->text().toInt();
    d["didx"]    = m_dDidx->text().toInt();
    d["pidx"]    = m_dPidx->text().toInt();
    d["bidx"]    = m_dBidx->text().toInt();
    d["addr"]    = m_dAddr->text().toInt();
    d["portidx"] = m_dPortidx->text().toInt();
    d["nvolt"]   = m_dNvolt->text().toDouble();
    d["namp"]    = m_dNamp->text().toDouble();
    d["flag"]    = m_dFlag->text().toInt();
    m_data.devices[idx] = d;

    // Write back device-specific fields into devSpecFlat
    if (m_devSpecGroup->isVisible()) {
        QString pfx    = kDeviceSpecPrefix.value(typeNum);
        int didx       = d["didx"].toInt();
        QString idxStr = QString::number(didx).rightJustified(3,'0');
        for (int r = 0; r < m_devSpecLayout->rowCount(); r++) {
            auto* edit = qobject_cast<QLineEdit*>(
                m_devSpecLayout->itemAt(r, QFormLayout::FieldRole)->widget());
            if (!edit) continue;
            QString field = edit->objectName().mid(5); // strip "spec_"
            QString key   = QString("device_%1_%2_%3").arg(pfx, field, idxStr);
            // Try to parse as JSON value
            QString txt = edit->text();
            if (txt.startsWith('[') || txt.startsWith('{')) {
                auto doc = QJsonDocument::fromJson(txt.toUtf8());
                if (!doc.isNull())
                    m_data.devSpecFlat[key] = doc.toVariant();
                else
                    m_data.devSpecFlat[key] = txt;
            } else {
                bool ok;
                double dv = txt.toDouble(&ok);
                m_data.devSpecFlat[key] = ok ? QVariant(dv) : QVariant(txt);
            }
        }
    }

    populateDevicesTable();
    m_devicesTable->selectRow(idx);
}

void MainWindow::onApplyDevice()
{
    if (m_selDevice < 0 || m_selDevice >= m_data.devices.size()) return;
    applyDeviceForm(m_selDevice);
    setStatus("Device updated");
}

void MainWindow::onAddDevice()
{
    if (!m_loaded) return;
    QVariantMap d;
    int n = m_data.devices.size();
    d["type"]     = 15;   // TSEN
    d["type_name"]= "TSEN";
    d["name"]     = QString("device%1").arg(n, 3, 10, QChar('0'));
    d["model"]    = 0; d["didx"] = 0; d["pidx"] = 0;
    d["bidx"]     = 0; d["addr"] = 0; d["portidx"] = 65535;
    d["nvolt"]    = 0.0; d["namp"] = 0.0; d["flag"] = 0;
    m_data.devices.append(d);
    populateDevicesTable();
    int row = m_data.devices.size() - 1;
    m_devicesTable->selectRow(row);
    onDeviceSelected(row, 0);
    setStatus("Device added");
}

void MainWindow::onDeleteDevice()
{
    if (m_selDevice < 0 || m_selDevice >= m_data.devices.size()) return;
    auto r = QMessageBox::question(this,"Delete Device",
        QString("Delete device %1?").arg(m_selDevice));
    if (r != QMessageBox::Yes) return;
    m_data.devices.removeAt(m_selDevice);
    m_selDevice = -1;
    m_devSpecGroup->setVisible(false);
    populateDevicesTable();
    setStatus("Device deleted");
}

// ============================================================================
// Geometry – Vertices
// ============================================================================

void MainWindow::populateVerticesTable()
{
    auto* t = m_verticesTable;
    t->setRowCount(m_data.vertices.size());
    for (int i = 0; i < m_data.vertices.size(); i++) {
        QVariantList v = m_data.vertices[i].toList();
        setTableItem(t,i,0,QString::number(i));
        setTableItem(t,i,1,fmtNum(v.value(0).toDouble()));
        setTableItem(t,i,2,fmtNum(v.value(1).toDouble()));
        setTableItem(t,i,3,fmtNum(v.value(2).toDouble()));
    }
}

void MainWindow::onVertexSelected(int row, int)
{
    m_selVertex = row;
    if (row < 0 || row >= m_data.vertices.size()) return;
    QVariantList v = m_data.vertices[row].toList();
    m_vX->setText(fmtNum(v.value(0).toDouble()));
    m_vY->setText(fmtNum(v.value(1).toDouble()));
    m_vZ->setText(fmtNum(v.value(2).toDouble()));
}

void MainWindow::applyVertexForm(int idx)
{
    m_data.vertices[idx] = QVariantList{
        m_vX->text().toDouble(),
        m_vY->text().toDouble(),
        m_vZ->text().toDouble()
    };
    populateVerticesTable();
    m_verticesTable->selectRow(idx);
}

void MainWindow::onApplyVertex()
{
    if (m_selVertex < 0 || m_selVertex >= m_data.vertices.size()) return;
    applyVertexForm(m_selVertex); setStatus("Vertex updated");
}

void MainWindow::onAddVertex()
{
    if (!m_loaded) return;
    m_data.vertices.append(QVariantList{0.0, 0.0, 0.0});
    populateVerticesTable();
    int row = m_data.vertices.size()-1;
    m_verticesTable->selectRow(row);
    onVertexSelected(row,0);
    setStatus("Vertex added");
}

void MainWindow::onDeleteVertex()
{
    if (m_selVertex < 0 || m_selVertex >= m_data.vertices.size()) return;
    auto r = QMessageBox::question(this,"Delete Vertex",
        QString("Delete vertex %1?").arg(m_selVertex));
    if (r != QMessageBox::Yes) return;
    m_data.vertices.removeAt(m_selVertex);
    m_selVertex = -1; populateVerticesTable(); setStatus("Vertex deleted");
}

// ============================================================================
// Geometry – Faces
// ============================================================================

void MainWindow::populateFacesTable()
{
    auto* t = m_facesTable;
    t->setRowCount(m_data.faces.size());
    for (int i = 0; i < m_data.faces.size(); i++) {
        QVariantMap f = m_data.faces[i].toMap();
        QVariantList com = f.value("com").toList();
        setTableItem(t,i,0,QString::number(i));
        setTableItem(t,i,1,fmtNum(f.value("area").toDouble()));
        setTableItem(t,i,2,fmtNum(com.value(0).toDouble()));
        setTableItem(t,i,3,fmtNum(com.value(1).toDouble()));
        setTableItem(t,i,4,fmtNum(com.value(2).toDouble()));
        setTableItem(t,i,5,intListToStr(f.value("triangle_idx").toList()));
    }
}

void MainWindow::onFaceSelected(int row, int)
{
    m_selFace = row;
    if (row < 0 || row >= m_data.faces.size()) return;
    QVariantMap f = m_data.faces[row].toMap();
    QVariantList com = f.value("com").toList();
    QVariantList nrm = f.value("normal").toList();
    m_fArea->setText(fmtNum(f.value("area").toDouble()));
    m_fNX->setText(fmtNum(nrm.value(0).toDouble()));
    m_fNY->setText(fmtNum(nrm.value(1).toDouble()));
    m_fNZ->setText(fmtNum(nrm.value(2).toDouble()));
    m_fCX->setText(fmtNum(com.value(0).toDouble()));
    m_fCY->setText(fmtNum(com.value(1).toDouble()));
    m_fCZ->setText(fmtNum(com.value(2).toDouble()));
    m_fTris->setText(intListToStr(f.value("triangle_idx").toList()));
}

void MainWindow::applyFaceForm(int idx)
{
    QVariantMap f;
    QVariantList tris = parseIntList(m_fTris->text());
    f["area"]          = m_fArea->text().toDouble();
    f["normal"]        = parseVec3(m_fNX, m_fNY, m_fNZ);
    f["com"]           = parseVec3(m_fCX, m_fCY, m_fCZ);
    f["triangle_cnt"]  = tris.size();
    f["triangle_idx"]  = tris;
    m_data.faces[idx]  = f;
    populateFacesTable();
    m_facesTable->selectRow(idx);
}

void MainWindow::onApplyFace()
{
    if (m_selFace < 0 || m_selFace >= m_data.faces.size()) return;
    applyFaceForm(m_selFace); setStatus("Face updated");
}

void MainWindow::onAddFace()
{
    if (!m_loaded) return;
    QVariantMap f;
    f["area"]=0.0; f["normal"]=QVariantList{0.0,0.0,0.0};
    f["com"]=QVariantList{0.0,0.0,0.0};
    f["triangle_cnt"]=0; f["triangle_idx"]=QVariantList{};
    m_data.faces.append(f); populateFacesTable();
    int row = m_data.faces.size()-1;
    m_facesTable->selectRow(row); onFaceSelected(row,0); setStatus("Face added");
}

void MainWindow::onDeleteFace()
{
    if (m_selFace < 0 || m_selFace >= m_data.faces.size()) return;
    auto r = QMessageBox::question(this,"Delete Face",
        QString("Delete face %1?").arg(m_selFace));
    if (r != QMessageBox::Yes) return;
    m_data.faces.removeAt(m_selFace);
    m_selFace=-1; populateFacesTable(); setStatus("Face deleted");
}

// ============================================================================
// Geometry – Strucs
// ============================================================================

void MainWindow::populateStrucsTable()
{
    auto* t = m_strucsTable;
    t->setRowCount(m_data.strucs.size());
    for (int i = 0; i < m_data.strucs.size(); i++) {
        QVariantMap s = m_data.strucs[i].toMap();
        setTableItem(t,i,0,QString::number(i));
        setTableItem(t,i,1,fmtNum(s.value("mass").toDouble()));
        setTableItem(t,i,2,fmtNum(s.value("volume").toDouble()));
        setTableItem(t,i,3,fmtNum(s.value("area").toDouble()));
        setTableItem(t,i,4,intListToStr(s.value("face_idx").toList()));
    }
}

void MainWindow::onStrucSelected(int row, int)
{
    m_selStruc = row;
    if (row < 0 || row >= m_data.strucs.size()) return;
    QVariantMap s = m_data.strucs[row].toMap();
    QVariantList com = s.value("com").toList();
    m_sMass->setText(fmtNum(s.value("mass").toDouble()));
    m_sVol ->setText(fmtNum(s.value("volume").toDouble()));
    m_sArea->setText(fmtNum(s.value("area").toDouble()));
    m_sCX->setText(fmtNum(com.value(0).toDouble()));
    m_sCY->setText(fmtNum(com.value(1).toDouble()));
    m_sCZ->setText(fmtNum(com.value(2).toDouble()));
    m_sFaces->setText(intListToStr(s.value("face_idx").toList()));
}

void MainWindow::applyStrucForm(int idx)
{
    QVariantMap s;
    QVariantList faces = parseIntList(m_sFaces->text());
    s["mass"]     = m_sMass->text().toDouble();
    s["volume"]   = m_sVol ->text().toDouble();
    s["area"]     = m_sArea->text().toDouble();
    s["com"]      = parseVec3(m_sCX, m_sCY, m_sCZ);
    s["face_cnt"] = faces.size();
    s["face_idx"] = faces;
    m_data.strucs[idx] = s;
    populateStrucsTable();
    m_strucsTable->selectRow(idx);
}

void MainWindow::onApplyStruc()
{
    if (m_selStruc < 0 || m_selStruc >= m_data.strucs.size()) return;
    applyStrucForm(m_selStruc); setStatus("Struc updated");
}

void MainWindow::onAddStruc()
{
    if (!m_loaded) return;
    QVariantMap s;
    s["mass"]=0.0; s["volume"]=0.0; s["area"]=0.0;
    s["com"]=QVariantList{0.0,0.0,0.0};
    s["face_cnt"]=0; s["face_idx"]=QVariantList{};
    m_data.strucs.append(s); populateStrucsTable();
    int row = m_data.strucs.size()-1;
    m_strucsTable->selectRow(row); onStrucSelected(row,0); setStatus("Struc added");
}

void MainWindow::onDeleteStruc()
{
    if (m_selStruc < 0 || m_selStruc >= m_data.strucs.size()) return;
    auto r = QMessageBox::question(this,"Delete Struc",
        QString("Delete struc %1?").arg(m_selStruc));
    if (r != QMessageBox::Yes) return;
    m_data.strucs.removeAt(m_selStruc);
    m_selStruc=-1; populateStrucsTable(); setStatus("Struc deleted");
}

// ============================================================================
// Ports
// ============================================================================

void MainWindow::populatePortsTable()
{
    auto* t = m_portsTable;
    t->setRowCount(m_data.ports.size());
    for (int i = 0; i < m_data.ports.size(); i++) {
        QVariantMap p = m_data.ports[i].toMap();
        setTableItem(t,i,0,QString::number(i));
        setTableItem(t,i,1,p.value("name").toString());
        setTableItem(t,i,2,p.value("type","0").toString());
        setTableItem(t,i,3,fmtNum(p.value("utc").toDouble()));
    }
}

void MainWindow::onPortSelected(int row, int)
{
    m_selPort = row;
    if (row < 0 || row >= m_data.ports.size()) return;
    QVariantMap p = m_data.ports[row].toMap();
    m_ptName->setText(p.value("name").toString());
    m_ptType->setText(p.value("type","0").toString());
    m_ptUtc ->setText(fmtNum(p.value("utc").toDouble()));
}

void MainWindow::applyPortForm(int idx)
{
    QVariantMap p;
    p["name"] = m_ptName->text();
    p["type"] = m_ptType->text().toInt();
    p["utc"]  = m_ptUtc->text().toDouble();
    m_data.ports[idx] = p;
    populatePortsTable();
    m_portsTable->selectRow(idx);
}

void MainWindow::onApplyPort()
{
    if (m_selPort < 0 || m_selPort >= m_data.ports.size()) return;
    applyPortForm(m_selPort); setStatus("Port updated");
}

void MainWindow::onAddPort()
{
    if (!m_loaded) return;
    QVariantMap p;
    p["name"] = QString("port%1").arg(m_data.ports.size());
    p["type"] = 0; p["utc"] = 0.0;
    m_data.ports.append(p); populatePortsTable();
    int row = m_data.ports.size()-1;
    m_portsTable->selectRow(row); onPortSelected(row,0); setStatus("Port added");
}

void MainWindow::onDeletePort()
{
    if (m_selPort < 0 || m_selPort >= m_data.ports.size()) return;
    auto r = QMessageBox::question(this,"Delete Port",
        QString("Delete port %1?").arg(m_selPort));
    if (r != QMessageBox::Yes) return;
    m_data.ports.removeAt(m_selPort);
    m_selPort=-1; populatePortsTable(); setStatus("Port deleted");
}
