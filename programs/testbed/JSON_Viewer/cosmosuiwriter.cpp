/********************************************************************
* Copyright (C) 2015 by Interstel Technologies, Inc.
*   and Hawaii Space Flight Laboratory.
*
* This file is part of the COSMOS/core that is the central
* module for COSMOS. For more information on COSMOS go to
* <http://cosmos-project.com>
*
* The COSMOS/core software is licenced under the
* GNU Lesser General Public License (LGPL) version 3 licence.
*
* You should have received a copy of the
* GNU Lesser General Public License
* If not, go to <http://www.gnu.org/licenses/>
*
* COSMOS/core is free software: you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public License
* as published by the Free Software Foundation, either version 3 of
* the License, or (at your option) any later version.
*
* COSMOS/core is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
*
* Refer to the "licences" folder for further information on the
* condititons and terms to use this software.
********************************************************************/

#include "cosmosuiwriter.h"

COSMOSUIWriter::COSMOSUIWriter(QIODevice * device) :
    QXmlStreamWriter(device)
{
    indentLevel = 0;
    inLayout = false;
}

void COSMOSUIWriter::writeStartUiDocument(QString className, QString version) {
    writeStartDocument();
    writeStartElement("ui");
    writeAttribute("version", version);
    writeTextElement("class", className);
    writeWidgetStart("QWidget", className);
}

void COSMOSUIWriter::writeQSpinBox(QString name) {
    writeQSpinBox(name, 30, 30, 42, 22);
}

void COSMOSUIWriter::writeQSpinBox(QString name, int x, int y, int width, int height) {
    writeWidgetStart("QSpinBox", name);
    if (!inLayout) writeGeometry(x, y, width, height);
    writeEndElement();
}

void COSMOSUIWriter::writeQSpinBox(QString name, int value, int minimum, int maximum) {
    writeQSpinBox(name, value, minimum, maximum, 30, 30, 42, 22);
}

void COSMOSUIWriter::writeQSpinBox(QString name, int value, int minimum, int maximum, int x, int y, int width, int height) {
    writeWidgetStart("QSpinBox", name);
    if (!inLayout) writeGeometry(x, y, width, height);
    writePropertyStart("minimum");
    writeTextElement("number", QString::number(minimum));
    writeEndElement();
    writePropertyStart("maximum");
    writeTextElement("number", QString::number(maximum));
    writeEndElement();
    writePropertyStart("value");
    writeTextElement("number", QString::number(value));
    writeEndElement();
    writeEndElement();
}

void COSMOSUIWriter::writeQSpinBox(QString name, int value, int minimum, int maximum, int singleStep, bool wrapping, bool readOnly) {
    writeQSpinBox(name, value, minimum, maximum, singleStep, wrapping, readOnly, 30, 30, 42, 22);
}

void COSMOSUIWriter::writeQSpinBox(QString name, int value, int minimum, int maximum, int singleStep, bool wrapping, bool readOnly, int x, int y, int width, int height) {
    writeWidgetStart("QSpinBox", name);
    if (!inLayout) writeGeometry(x, y, width, height);
    writePropertyStart("wrapping");
    if (wrapping) writeTextElement("bool", "true");
    else writeTextElement("bool", "false");
    writeEndElement();
    writePropertyStart("readOnly");
    if (readOnly) writeTextElement("bool", "true");
    else writeTextElement("bool", "false");
    writeEndElement();
    writePropertyStart("minimum");
    writeTextElement("number", QString::number(minimum));
    writeEndElement();
    writePropertyStart("maximum");
    writeTextElement("number", QString::number(maximum));
    writeEndElement();
    writePropertyStart("singleStep");
    writeTextElement("number", QString::number(singleStep));
    writeEndElement();
    writePropertyStart("value");
    writeTextElement("number", QString::number(value));
    writeEndElement();
    writeEndElement();
}

void COSMOSUIWriter::writeQSpinBox(QString name, int value, int minimum, int maximum, int singleStep, bool wrapping, bool readOnly, bool frame, QAbstractSpinBox::ButtonSymbols buttonSymbols) {
    writeQSpinBox(name, value, minimum, maximum, singleStep, wrapping, readOnly, frame, buttonSymbols, 30, 30, 42, 22);
}

void COSMOSUIWriter::writeQSpinBox(QString name, int value, int minimum, int maximum, int singleStep, bool wrapping, bool readOnly, bool frame, QAbstractSpinBox::ButtonSymbols buttonSymbols, int x, int y, int width, int height) {
    writeWidgetStart("QSpinBox", name);
    if (!inLayout) writeGeometry(x, y, width, height);
    writePropertyStart("wrapping");
    if (wrapping) writeTextElement("bool", "true");
    else writeTextElement("bool", "false");
    writeEndElement();
    writePropertyStart("frame");
    if (frame) writeTextElement("bool", "true");
    else writeTextElement("bool", "false");
    writeEndElement();
    writePropertyStart("readOnly");
    if (readOnly) writeTextElement("bool", "true");
    else writeTextElement("bool", "false");
    writeEndElement();
    writePropertyStart("buttonSymbols");
    switch (buttonSymbols) {
    case QAbstractSpinBox::PlusMinus :
        writeTextElement("enum", "QAbstractSpinBox::PlusMinus"); //strange: apparently there's either a bug or Qt no longer supprots this.
        break;
    case QAbstractSpinBox::UpDownArrows :
        writeTextElement("enum", "QAbstractSpinBox::UpDownArrows");
        break;
    default :
        writeTextElement("enum", "QAbstractSpinBox::NoButtons");
        break;
    }
    writeEndElement();
    writePropertyStart("minimum");
    writeTextElement("number", QString::number(minimum));
    writeEndElement();
    writePropertyStart("maximum");
    writeTextElement("number", QString::number(maximum));
    writeEndElement();
    writePropertyStart("singleStep");
    writeTextElement("number", QString::number(singleStep));
    writeEndElement();
    writePropertyStart("value");
    writeTextElement("number", QString::number(value));
    writeEndElement();
    writeEndElement();
}

void COSMOSUIWriter::writeQDoubleSpinBox(QString name) {
    writeQSpinBox(name, 30, 30, 42, 22);
}

void COSMOSUIWriter::writeQDoubleSpinBox(QString name, int x, int y, int width, int height) {
    writeWidgetStart("QDoubleSpinBox", name);
    if (!inLayout) writeGeometry(x, y, width, height);
    writeEndElement();
}

void COSMOSUIWriter::writeQDoubleSpinBox(QString name, double value, double minimum, double maximum) {
    writeQSpinBox(name, value, minimum, maximum, 30, 30, 42, 22);
}

void COSMOSUIWriter::writeQDoubleSpinBox(QString name, double value, double minimum, double maximum, int x, int y, int width, int height) {
    writeWidgetStart("QDoubleSpinBox", name);
    if (!inLayout) writeGeometry(x, y, width, height);
    writePropertyStart("minimum");
    writeTextElement("double", QString::number(minimum));
    writeEndElement();
    writePropertyStart("maximum");
    writeTextElement("double", QString::number(maximum));
    writeEndElement();
    writePropertyStart("value");
    writeTextElement("double", QString::number(value));
    writeEndElement();
    writeEndElement();
}

void COSMOSUIWriter::writeQDoubleSpinBox(QString name, double value, double minimum, double maximum, double singleStep, bool wrapping, bool readOnly) {
    writeQSpinBox(name, value, minimum, maximum, singleStep, wrapping, readOnly, 30, 30, 42, 22);
}

void COSMOSUIWriter::writeQDoubleSpinBox(QString name, double value, double minimum, double maximum, double singleStep, bool wrapping, bool readOnly, int x, int y, int width, int height) {
    writeWidgetStart("QDoubleSpinBox", name);
    if (!inLayout) writeGeometry(x, y, width, height);
    writePropertyStart("wrapping");
    if (wrapping) writeTextElement("bool", "true");
    else writeTextElement("bool", "false");
    writeEndElement();
    writePropertyStart("readOnly");
    if (readOnly) writeTextElement("bool", "true");
    else writeTextElement("bool", "false");
    writeEndElement();
    writePropertyStart("minimum");
    writeTextElement("double", QString::number(minimum));
    writeEndElement();
    writePropertyStart("maximum");
    writeTextElement("double", QString::number(maximum));
    writeEndElement();
    writePropertyStart("singleStep");
    writeTextElement("double", QString::number(singleStep));
    writeEndElement();
    writePropertyStart("value");
    writeTextElement("double", QString::number(value));
    writeEndElement();
    writeEndElement();
}

void COSMOSUIWriter::writeQDoubleSpinBox(QString name, double value, double minimum, double maximum, double singleStep, bool wrapping, bool readOnly, bool frame, QAbstractSpinBox::ButtonSymbols buttonSymbols) {
    writeQSpinBox(name, value, minimum, maximum, singleStep, wrapping, readOnly, frame, buttonSymbols, 30, 30, 42, 22);
}

void COSMOSUIWriter::writeQDoubleSpinBox(QString name, double value, double minimum, double maximum, double singleStep, bool wrapping, bool readOnly, bool frame, QAbstractSpinBox::ButtonSymbols buttonSymbols, int x, int y, int width, int height) {
    writeWidgetStart("QDoubleSpinBox", name);
    if (!inLayout) writeGeometry(x, y, width, height);
    writePropertyStart("wrapping");
    if (wrapping) writeTextElement("bool", "true");
    else writeTextElement("bool", "false");
    writeEndElement();
    writePropertyStart("frame");
    if (frame) writeTextElement("bool", "true");
    else writeTextElement("bool", "false");
    writeEndElement();
    writePropertyStart("readOnly");
    if (readOnly) writeTextElement("bool", "true");
    else writeTextElement("bool", "false");
    writeEndElement();
    writePropertyStart("buttonSymbols");
    switch (buttonSymbols) {
    case QAbstractSpinBox::PlusMinus :
        writeTextElement("enum", "QAbstractSpinBox::PlusMinus"); //strange: apparently there's either a bug or Qt no longer supprots this.
        break;
    case QAbstractSpinBox::UpDownArrows :
        writeTextElement("enum", "QAbstractSpinBox::UpDownArrows");
        break;
    default :
        writeTextElement("enum", "QAbstractSpinBox::NoButtons");
        break;
    }
    writeEndElement();
    writePropertyStart("minimum");
    writeTextElement("double", QString::number(minimum));
    writeEndElement();
    writePropertyStart("maximum");
    writeTextElement("double", QString::number(maximum));
    writeEndElement();
    writePropertyStart("singleStep");
    writeTextElement("double", QString::number(singleStep));
    writeEndElement();
    writePropertyStart("value");
    writeTextElement("double", QString::number(value));
    writeEndElement();
    writeEndElement();
}

void COSMOSUIWriter::writeQRadioButton(QString name, QString text) {
    writeQRadioButton(name, text, 30, 30, 80, 16);
}

void COSMOSUIWriter::writeQRadioButton(QString name, QString text, int x, int y, int width, int height) {
    writeWidgetStart("QRadioButton", name);
    if (!inLayout) writeGeometry(x, y, width, height);
    writePropertyStart("text");
    writeTextElement("string", text);
    writeEndElement();
    writeEndElement();
}

void COSMOSUIWriter::writeQRadioButton(QString name, QString text, bool checkable, bool checked) {
    writeQRadioButton(name, text, checkable, checked, 30, 30, 80, 16);
}

void COSMOSUIWriter::writeQRadioButton(QString name, QString text, bool checkable, bool checked, int x, int y, int width, int height) {
    writeWidgetStart("QRadioButton", name);
    if (!inLayout) writeGeometry(x, y, width, height);
    writePropertyStart("text");
    writeTextElement("string", text);
    writeEndElement();
    writePropertyStart("checkable");
    if (checkable) {
        writeTextElement("bool", "true");
        writeEndElement();
        writePropertyStart("checked");
        if (checked) writeTextElement("bool", "true");
        else writeTextElement("bool", "false");
        writeEndElement();
    } else {
        writeTextElement("bool", "false");
        writeEndElement();
    }
    writeEndElement();
}

void COSMOSUIWriter::writeQLCDNumber(QString name){
    writeQLCDNumber(name, 30, 30, 60, 30);
}

void COSMOSUIWriter::writeQLCDNumber(QString name, double value) {
    writeQLCDNumber(name, value, 30, 30, 60, 30);
}

void COSMOSUIWriter::writeQLCDNumber(QString name, int x, int y, int width, int height) {
    writeWidgetStart("QLCDNumber", name);
    if (!inLayout) writeGeometry(x, y, width, height);
    writeEndElement();
}

void COSMOSUIWriter::writeQLCDNumber(QString name, double value, int x, int y, int width, int height) {
    writeWidgetStart("QLCDNumber", name);
    if (!inLayout) writeGeometry(x, y, width, height);
    writePropertyStart("value");
    writeTextElement("double", QString::number(value));
    writeEndElement();
    writeEndElement();
}

void COSMOSUIWriter::writeQLCDNumber(QString name, bool smalldecimalpt) {
    writeQLCDNumber(name, smalldecimalpt, 30, 30, 60, 30);
}

void COSMOSUIWriter::writeQLCDNumber(QString name, bool smalldecimalpt, int x, int y, int width, int height) {
    writeWidgetStart("QLCDNumber", name);
    if (!inLayout) writeGeometry(x, y, width, height);
    writePropertyStart("smallDecimalPoint");
    if (smalldecimalpt) writeTextElement("bool", "true");
    else writeTextElement("bool", "false");
    writeEndElement();
    writeEndElement();
}

void COSMOSUIWriter::writeQLCDNumber(QString name, double value, bool smalldecimalpt) {
    writeQLCDNumber(name, value, smalldecimalpt, 30, 30, 60, 30);
}

void COSMOSUIWriter::writeQLCDNumber(QString name, double value, bool smalldecimalpt, int x, int y, int width, int height) {
    writeWidgetStart("QLCDNumber", name);
    if (!inLayout) writeGeometry(x, y, width, height);
    writePropertyStart("smallDecimalPoint");
    if (smalldecimalpt) writeTextElement("bool", "true");
    else writeTextElement("bool", "false");
    writeEndElement();
    writePropertyStart("value");
    writeTextElement("double", QString::number(value));
    writeEndElement();
    writeEndElement();
}

void COSMOSUIWriter::writeQLCDNumber(QString name, bool smalldecimalpt, QLCDNumber::Mode mode, QFrame::Shape shape, QFrame::Shadow shadow) {
    writeQLCDNumber(name, smalldecimalpt, mode, shape, shadow, 30, 30, 60, 30);
}

void COSMOSUIWriter::writeQLCDNumber(QString name, bool smalldecimalpt, QLCDNumber::Mode mode, QFrame::Shape shape, QFrame::Shadow shadow, int x, int y, int width, int height) {
    writeWidgetStart("QLCDNumber", name);
    if (!inLayout) writeGeometry(x, y, width, height);
    writePropertyStart("smallDecimalPoint");
    if (smalldecimalpt) writeTextElement("bool", "true");
    else writeTextElement("bool", "false");
    writeEndElement();
    writePropertyStart("mode");
    switch (mode) {
    case QLCDNumber::Dec :
        writeTextElement("enum", "QLCDNumber::Dec");
        break;
    case QLCDNumber::Hex :
        writeTextElement("enum", "QLCDNumber::Hex");
        break;
    case QLCDNumber::Oct :
        writeTextElement("enum", "QLCDNumber::Oct");
        break;
    case QLCDNumber::Bin :
        writeTextElement("enum", "QLCDNumber::Bin");
        break;
    }
    writeEndElement();
    writePropertyStart("frameShape");
    switch(shape) {
    case QFrame::Box :
        writeTextElement("enum", "QFrame::Box");
        break;
    case QFrame::Panel :
        writeTextElement("enum", "QFrame::Panel");
        break;
    case QFrame::WinPanel:
        writeTextElement("enum", "QFrame::WinPanel");
        break;
    case QFrame::HLine:
        writeTextElement("enum", "QFrame::HLine");
        break;
    case QFrame::VLine:
        writeTextElement("enum", "QFrame::VLine");
        break;
    case QFrame::StyledPanel:
        writeTextElement("enum", "QFrame::StyledPanel");
        break;
    default :
        writeTextElement("enum", "QFrame::NoFrame");
        break;
    }
    writeEndElement();
    writePropertyStart("frameShadow");
    switch(shadow) {
    case QFrame::Raised :
        writeTextElement("enum", "QFrame::Raised");
        break;
    case QFrame::Sunken :
        writeTextElement("enum", "QFrame::Sunken");
        break;
    default :
        writeTextElement("enum", "QFrame::Plain");
        break;
    }
    writeEndElement();
    writeEndElement();
}

void COSMOSUIWriter::writeQLCDNumber(QString name, double value, bool smalldecimalpt, QLCDNumber::Mode mode, QFrame::Shape shape, QFrame::Shadow shadow) {
    writeQLCDNumber(name, value, smalldecimalpt, mode, shape, shadow, 30, 30, 60, 30);
}

void COSMOSUIWriter::writeQLCDNumber(QString name, double value, bool smalldecimalpt, QLCDNumber::Mode mode, QFrame::Shape shape, QFrame::Shadow shadow, int x, int y, int width, int height) {
    writeWidgetStart("QLCDNumber", name);
    if (!inLayout) writeGeometry(x, y, width, height);
    writePropertyStart("smallDecimalPoint");
    if (smalldecimalpt) writeTextElement("bool", "true");
    else writeTextElement("bool", "false");
    writeEndElement();
    writePropertyStart("mode");
    switch (mode) {
    case QLCDNumber::Dec :
        writeTextElement("enum", "QLCDNumber::Dec");
        break;
    case QLCDNumber::Hex :
        writeTextElement("enum", "QLCDNumber::Hex");
        break;
    case QLCDNumber::Oct :
        writeTextElement("enum", "QLCDNumber::Oct");
        break;
    case QLCDNumber::Bin :
        writeTextElement("enum", "QLCDNumber::Bin");
        break;
    }
    writeEndElement();
    writePropertyStart("frameShape");
    switch(shape) {
    case QFrame::Box :
        writeTextElement("enum", "QFrame::Box");
        break;
    case QFrame::Panel :
        writeTextElement("enum", "QFrame::Panel");
        break;
    case QFrame::WinPanel:
        writeTextElement("enum", "QFrame::WinPanel");
        break;
    case QFrame::HLine:
        writeTextElement("enum", "QFrame::HLine");
        break;
    case QFrame::VLine:
        writeTextElement("enum", "QFrame::VLine");
        break;
    case QFrame::StyledPanel:
        writeTextElement("enum", "QFrame::StyledPanel");
        break;
    default :
        writeTextElement("enum", "QFrame::NoFrame");
        break;
    }
    writeEndElement();
    writePropertyStart("frameShadow");
    switch(shadow) {
    case QFrame::Raised :
        writeTextElement("enum", "QFrame::Raised");
        break;
    case QFrame::Sunken :
        writeTextElement("enum", "QFrame::Sunken");
        break;
    default :
        writeTextElement("enum", "QFrame::Plain");
        break;
    }
    writeEndElement();
    writePropertyStart("value");
    writeTextElement("double", QString::number(value));
    writeEndElement();
    writeEndElement();
}

void COSMOSUIWriter::writeQLineEdit(QString name) {
    writeQLineEdit(name, 30, 30, 60, 16);
}

void COSMOSUIWriter::writeQLineEdit(QString name, QString text) {
    writeQLineEdit(name, text, 30, 30, 60, 16);
}

void COSMOSUIWriter::writeQLineEdit(QString name, int x, int y, int width, int height) {
    writeWidgetStart("QLineEdit", name);
    if (!inLayout) writeGeometry(x, y, width, height);
    writeEndElement();
}

void COSMOSUIWriter::writeQLineEdit(QString name, QString text, int x, int y, int width, int height) {
    writeWidgetStart("QLineEdit", name);
    if (!inLayout) writeGeometry(x, y, width, height);
    writePropertyStart("text");
    writeTextElement("string", text);
    writeEndElement();
    writeEndElement();
}

void COSMOSUIWriter::writeQLabel(QString name, QString text) {
    writeQLabel(name, text, 30, 30, 8*text.length(), 16);
}

void COSMOSUIWriter::writeQLabel(QString name, QString text, int x, int y) {
    writeQLabel(name, text, x, y, 8*text.length(), 16);
}

void COSMOSUIWriter::writeQLabel(QString name, QString text, int x, int y, int width, int height) {
    writeWidgetStart("QLabel", name);
    if (!inLayout) writeGeometry(x, y, width, height);
    writePropertyStart("text");
    writeTextElement("string", text);
    writeEndElement();
    writeEndElement();
}

void COSMOSUIWriter::beginQGroupBox(QString name, QString title) {
    beginQGroupBox(name, title, 30, 30, 100, 100);
}

void COSMOSUIWriter::beginQGroupBox(QString name, QString title, bool flat, bool checkable, bool checked) {
    beginQGroupBox(name, title, flat, checkable, checked, 30, 30, 100, 100);
}

void COSMOSUIWriter::beginQGroupBox(QString name, QString title, int x, int y, int width, int height) {
    writeWidgetStart("QGroupBox", name);
    if (!inLayout) writeGeometry(x, y, width, height);
    writePropertyStart("title");
    writeTextElement("string", title);
    writeEndElement();
}

void COSMOSUIWriter::beginQGroupBox(QString name, QString title, bool flat, bool checkable, bool checked, int x, int y, int width, int height) {
    beginQGroupBox(name, title, x, y, width, height);
    writePropertyStart("flat");
    if (flat) writeTextElement("bool", "true");
    writeEndElement();
    if (checkable) {
        writePropertyStart("checkable");
        writeTextElement("bool", "true");
        writeEndElement();
        writePropertyStart("checked");
        if (checked) writeTextElement("bool", "true");
        else writeTextElement("bool", "false");
        writeEndElement();
    }
}

void COSMOSUIWriter::writePropertyStart(QString name) {
    writeStartElement("property");
    writeAttribute("name", name);
}

void COSMOSUIWriter::writeWidgetStart(QString widgetClass, QString widgetName) {
    writeStartElement("widget");
    writeAttribute("class", widgetClass);
    writeAttribute("name", widgetName);
}

void COSMOSUIWriter::writeGeometry(int width, int height) {
    writeGeometry(0, 0, width, height);
}

void COSMOSUIWriter::writeGeometry(int x, int y, int width, int height) {
    writePropertyStart("geometry");
    writeStartElement("rect");
    writeTextElement("x", QString::number(x));
    writeTextElement("y", QString::number(y));
    writeTextElement("width", QString::number(width));
    writeTextElement("height", QString::number(height));
    writeEndElement();
    writeEndElement();
}

void COSMOSUIWriter::writeSize(int width, int height) {
    writeStartElement("size");
    writeTextElement("width", QString::number(width));
    writeTextElement("height", QString::number(height));
    writeEndElement();
}

void COSMOSUIWriter::writeSize(QString type, int width, int height) {
    writePropertyStart(type);
    writeSize(width, height);
    writeEndElement();
}

void COSMOSUIWriter::writeSpacer(QString name, Qt::Orientation orientation, int widthHint, int heightHint) {
    writeStartElement("spacer");
    writeAttribute("name", name);
    writePropertyStart("orientation");
    switch (orientation) {
        case Qt::Horizontal :
            writeTextElement("enum", "Qt::Horizontal");
            break;
        default :
            writeTextElement("enum", "Qt::Vertical");
            break;
    }
    writeEndElement();
    writePropertyStart("sizeHint");
    writeAttribute("stdset", "0");
    writeSize(widthHint, heightHint);
    writeEndElement();
    writeEndElement();
}

void COSMOSUIWriter::writeSizePolicy(QString Policy) {
    writeSizePolicy(Policy, Policy, 0, 0);
}

void COSMOSUIWriter::writeSizePolicy(QString Policy, int stretch) {
    writeSizePolicy(Policy, Policy, stretch, stretch);
}

void COSMOSUIWriter::writeSizePolicy(QString Policy, int horizontalStretch, int verticalStretch) {
    writeSizePolicy(Policy, Policy, horizontalStretch, verticalStretch);
}

void COSMOSUIWriter::writeSizePolicy(QString horizontalPolicy, QString verticalPolicy, int horizontalStretch, int verticalStretch) {
    writePropertyStart("sizePolicy");
    writeStartElement("sizepolicy");
    writeAttribute("hsizetype", horizontalPolicy);
    writeAttribute("vsizetype", verticalPolicy);
    writeTextElement("horstretch", QString::number(horizontalStretch));
    writeTextElement("verstretch", QString::number(verticalStretch));
    writeEndElement();
    writeEndElement();
}

void COSMOSUIWriter::writeWindowTitle(QString title) {
    writePropertyStart("windowTitle");
    writeTextElement("string", title);
    writeEndElement();
}

void COSMOSUIWriter::beginLayout(QString layoutClass, QString name) {
    writeStartElement("layout");
    writeAttribute("class", layoutClass);
    writeAttribute("name", name);
    activeLayouts.append(indentLevel);
}

void COSMOSUIWriter::writeStartElement(const QString &qualifiedName) {
    if (!activeLayouts.isEmpty()) {
        if (activeLayouts.last()==indentLevel) {
            writeStartItem();
            inLayout = true;
        } else inLayout = false;
    } else inLayout = false;
    indentLevel++;
    QXmlStreamWriter::writeStartElement(qualifiedName);
}

void COSMOSUIWriter::writeStartElement(const QString &namespaceUri, const QString &name) {
    if (!activeLayouts.isEmpty()) {
        if (activeLayouts.last()==indentLevel) {
            writeStartItem();
            inLayout = true;
        } else inLayout = false;
    } else inLayout = false;
    indentLevel++;
    QXmlStreamWriter::writeStartElement(namespaceUri, name);
}

void COSMOSUIWriter::writeStartItem() {
    QXmlStreamWriter::writeStartElement("item");
    indentLevel++;
    invisibleElements.append(indentLevel);
}

void COSMOSUIWriter::writeEndElement() {
    if (!invisibleElements.isEmpty()) if (invisibleElements.last()==indentLevel) {
        QXmlStreamWriter::writeEndElement();
        invisibleElements.pop_back();
        if (indentLevel>0) indentLevel--;
    }
    if (indentLevel>0) indentLevel--;
    QXmlStreamWriter::writeEndElement();
    if (!invisibleElements.isEmpty()) if (invisibleElements.last()==indentLevel) {
        QXmlStreamWriter::writeEndElement();
        invisibleElements.pop_back();
        if (indentLevel>0) indentLevel--;
    }
    if (!activeLayouts.isEmpty()) if (activeLayouts.last()>indentLevel) activeLayouts.pop_back();
    if (!activeLayouts.isEmpty()) {
        if (activeLayouts.last()==indentLevel) {
            writeStartItem();
            inLayout = true;
        } else inLayout = false;
    } else inLayout = false;
}
