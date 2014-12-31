#include "dataview.h"
#include "cosmosuiwriter.h"

DataView::DataView(QWidget *parent) :
    QWidget(parent)
{
    this->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    this->setMinimumSize(400, 300);//NEED A WAY TO GET THIS INFORMATION FROM THE FILE.
    QFile XMLFile("../JSON_Viewer_Layouts/customLayout.ui");
    if (true/*!XMLFile.exists()*/) {
        XMLFile.open(QIODevice::ReadWrite | QIODevice::Text);
        COSMOSUIWriter newLayoutFile(&XMLFile);
        newLayoutFile.setAutoFormatting(true);
        //Start Qwidget, set class name form geometry, and window title.
        newLayoutFile.writeStartUiDocument("DataView", "4.0");
        newLayoutFile.writeGeometry(400, 300);
        newLayoutFile.writeSizePolicy("Minimum");
        newLayoutFile.writeSize("minimumSize", 400, 300);
        newLayoutFile.writeSize("baseSize", 400, 300);
        newLayoutFile.writeWindowTitle("JSON Viewer");
        //Form content
        //QLabel:
        newLayoutFile.writeQLabel("label", "Some Stuff", 30, 30);
        //QLineEdit:
        newLayoutFile.writeQLineEdit("lineEdit_2", 230, 190, 113, 20);
        //QLineEdit (agian):
        newLayoutFile.writeQLineEdit("lineEdit_3", 60, 200, 113, 20);
        //QGroupBox start
        newLayoutFile.beginQGroupBox("groupbox", "Some Other Stuff", 120, 10, 271, 121);
        //Layout start
        newLayoutFile.beginLayout("QHBoxLayout", "horizontalLayout");
        newLayoutFile.writeQLineEdit("lineEdit");
        newLayoutFile.writeQLabel("label_2", "Some Stuff");
        //Spacer:
        newLayoutFile.writeSpacer("horizontalSpacer", Qt::Horizontal, 40, 20);
        //QLCDNumber
        newLayoutFile.writeQLCDNumber("qlcdnumber", 3.14, true);
        newLayoutFile.writeEndElement(); //Layout End.
        newLayoutFile.writeEndElement(); //QGroupBox End.
        //QRadioButton:
        newLayoutFile.writeQRadioButton("radioButton", "Station 1", 10, 70, 82, 17);
        //QRadioButton (agian):
        newLayoutFile.writeQRadioButton("radioButton_2", "Station 2", true, true, 10, 100, 82, 17);
        //QSpinBoxes:
        newLayoutFile.writeQSpinBox("spinBox", 30, 150, 42, 22);
        newLayoutFile.writeQDoubleSpinBox("doubleSpinBox", 98.3, 0.00, 115.9, 1.10, true, false, true, QAbstractSpinBox::UpDownArrows, 80, 150, 49, 20);
        //Close form
        newLayoutFile.writeEndElement();
        //Close document
        newLayoutFile.writeStartElement("resources");
        newLayoutFile.writeEndElement();
        newLayoutFile.writeStartElement("connections");
        newLayoutFile.writeEndElement();
        newLayoutFile.writeEndDocument();
        XMLFile.close();
    }
    XMLFile.open(QFile::ReadOnly);
    QUiLoader DataLayout;
    DataLayout.load(&XMLFile, this);
    XMLFile.close();
    //this is how you access stuff:
    textField = qFindChild<QLineEdit*>(this, "lineEdit_2");
    textField->setText("ta-da!");
}

DataView::~DataView()
{
}
