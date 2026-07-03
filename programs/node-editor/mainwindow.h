#pragma once
#include <QMainWindow>
#include <QComboBox>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QSplitter>
#include <QTabWidget>
#include <QTableWidget>
#include <QFormLayout>
#include <QScrollArea>
#include "nodedata.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(const QString& nodesDir, QWidget* parent = nullptr);

private slots:
    // Top bar
    void onNodeSelected(const QString& name);
    void onNewNode();
    void onSave();

    // Pieces
    void onPieceSelected(int row, int);
    void onApplyPiece();
    void onAddPiece();
    void onDeletePiece();

    // Devices
    void onDeviceSelected(int row, int);
    void onApplyDevice();
    void onAddDevice();
    void onDeleteDevice();
    void onDeviceTypeChanged(int index);

    // Geometry – vertices
    void onVertexSelected(int row, int);
    void onApplyVertex();
    void onAddVertex();
    void onDeleteVertex();

    // Geometry – faces
    void onFaceSelected(int row, int);
    void onApplyFace();
    void onAddFace();
    void onDeleteFace();

    // Geometry – strucs
    void onStrucSelected(int row, int);
    void onApplyStruc();
    void onAddStruc();
    void onDeleteStruc();

    // Ports
    void onPortSelected(int row, int);
    void onApplyPort();
    void onAddPort();
    void onDeletePort();

private:
    // Layout builders
    void setupUi();
    QWidget* makePiecesTab();
    QWidget* makeDevicesTab();
    QWidget* makeGeometryTab();
    QWidget* makePortsTab();
    QWidget* makeNodeTab();

    // Form-panel builder helpers
    QScrollArea* makeFormScroll(QWidget* formContent);
    QWidget*     makeSectionHeader(const QString& title);
    QLineEdit*   addRow(QFormLayout* fl, const QString& label);
    QWidget*     makeVec3Row(QFormLayout* fl, const QString& label,
                              QLineEdit** x, QLineEdit** y, QLineEdit** z);

    // Data → UI
    void refreshNodeList();
    void loadNode(const QString& name);
    void populatePiecesTable();
    void populateDevicesTable();
    void populateVerticesTable();
    void populateFacesTable();
    void populateStrucsTable();
    void populatePortsTable();

    // UI → Data
    void applyPieceForm(int idx);
    void applyDeviceForm(int idx);
    void applyVertexForm(int idx);
    void applyFaceForm(int idx);
    void applyStrucForm(int idx);
    void applyPortForm(int idx);

    // Table helpers
    void setTableItem(QTableWidget* t, int row, int col, const QString& text);
    void updateDeviceSpecGroup(int deviceIdx);

    void setStatus(const QString& msg, bool error = false);

    // ----------------------------------------------------------------
    // State
    // ----------------------------------------------------------------
    QString  m_nodesDir;
    NodeData m_data;
    bool     m_loaded = false;

    int m_selPiece  = -1;
    int m_selDevice = -1;
    int m_selVertex = -1;
    int m_selFace   = -1;
    int m_selStruc  = -1;
    int m_selPort   = -1;

    // ----------------------------------------------------------------
    // Widgets
    // ----------------------------------------------------------------
    QComboBox*    m_nodeSelector;
    QLabel*       m_statusLabel;
    QTabWidget*   m_tabs;

    // Node tab
    QLineEdit*    m_nodeName;
    QLineEdit*    m_nodeType;

    // Pieces tab
    QTableWidget* m_piecesTable;
    QLineEdit*    m_pName;
    QLineEdit*    m_pCidx;
    QLineEdit*    m_pStrucIdx;
    QLineEdit*    m_pMass;
    QLineEdit*    m_pDensity;
    QLineEdit*    m_pArea;
    QLineEdit*    m_pVolume;
    QLineEdit*    m_pDim;
    QLineEdit*    m_pComX, *m_pComY, *m_pComZ;
    QLineEdit*    m_pEmi;
    QLineEdit*    m_pAbs;
    QLineEdit*    m_pHcap;
    QLineEdit*    m_pHcon;

    // Devices tab
    QTableWidget* m_devicesTable;
    QLineEdit*    m_dName;
    QComboBox*    m_dType;
    QLineEdit*    m_dModel;
    QLineEdit*    m_dDidx;
    QLineEdit*    m_dPidx;
    QLineEdit*    m_dBidx;
    QLineEdit*    m_dAddr;
    QLineEdit*    m_dPortidx;
    QLineEdit*    m_dNvolt;
    QLineEdit*    m_dNamp;
    QLineEdit*    m_dFlag;
    QGroupBox*    m_devSpecGroup;
    QFormLayout*  m_devSpecLayout;

    // Geometry – vertices
    QTableWidget* m_verticesTable;
    QLineEdit*    m_vX, *m_vY, *m_vZ;

    // Geometry – faces
    QTableWidget* m_facesTable;
    QLineEdit*    m_fArea;
    QLineEdit*    m_fNX,  *m_fNY,  *m_fNZ;
    QLineEdit*    m_fCX,  *m_fCY,  *m_fCZ;
    QLineEdit*    m_fTris; // comma-separated triangle indices

    // Geometry – strucs
    QTableWidget* m_strucsTable;
    QLineEdit*    m_sCX,  *m_sCY,  *m_sCZ;
    QLineEdit*    m_sMass;
    QLineEdit*    m_sVol;
    QLineEdit*    m_sArea;
    QLineEdit*    m_sFaces; // comma-separated face indices

    // Geometry – triangles (display only)
    QTableWidget* m_trianglesTable;

    // Ports
    QTableWidget* m_portsTable;
    QLineEdit*    m_ptName;
    QLineEdit*    m_ptType;
    QLineEdit*    m_ptUtc;
};
