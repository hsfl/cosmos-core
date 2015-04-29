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

#ifndef COSMOSUIWRITER_H
#define COSMOSUIWRITER_H

#include <QXmlStreamWriter>
#include <QVector>
#include <QLCDNumber>
#include <QAbstractSpinBox>

class COSMOSUIWriter : public QXmlStreamWriter
{
public:
    COSMOSUIWriter(QIODevice * device);

    void writeStartUiDocument(QString className, QString version);

    void writeStartElement(const QString &qualifiedName);
    void writeStartElement(const QString &namespaceUri, const QString &name);

    void writeEndElement();

    void writePropertyStart(QString name);

    void writeWidgetStart(QString widgetClass, QString widgetName);

    void writeGeometry(int width, int height);
    void writeGeometry(int x, int y, int width, int height);

    void writeSize(int width, int height);
    void writeSize(QString type, int width, int height);

    void writeSizePolicy(QString Policy);
    void writeSizePolicy(QString Policy, int stretch);
    void writeSizePolicy(QString Policy, int horizontalStretch, int verticalStretch);
    void writeSizePolicy(QString horizontalPolicy, QString verticalPolicy, int horizontalStretch, int verticalStretch);

    void writeWindowTitle(QString title);

    void beginLayout(QString layoutClass, QString name);

    void beginQGroupBox(QString name, QString title);
    void beginQGroupBox(QString name, QString title, bool flat, bool checkable, bool checked);
    void beginQGroupBox(QString name, QString title, int x, int y, int width, int height);
    void beginQGroupBox(QString name, QString title, bool flat, bool checkable, bool checked, int x, int y, int width, int height);

    void writeSpacer(QString name, Qt::Orientation orientation, int widthHint, int heightHint);

    void writeQLabel(QString name, QString text);
    void writeQLabel(QString name, QString text, int x, int y);
    void writeQLabel(QString name, QString text, int x, int y, int width, int height);

    void writeQLineEdit(QString name);
    void writeQLineEdit(QString name, QString text);
    void writeQLineEdit(QString name, int x, int y, int width, int height);
    void writeQLineEdit(QString name, QString text, int x, int y, int width, int height);

    void writeQLCDNumber(QString name);
    void writeQLCDNumber(QString name, double value);
    void writeQLCDNumber(QString name, int x, int y, int width, int height);
    void writeQLCDNumber(QString name, double value, int x, int y, int width, int height);
    void writeQLCDNumber(QString name, bool smalldecimalpt);
    void writeQLCDNumber(QString name, double value, bool smalldecimalpt);
    void writeQLCDNumber(QString name, bool smalldecimalpt, int x, int y, int width, int height);
    void writeQLCDNumber(QString name, double value, bool smalldecimalpt, int x, int y, int width, int height);
    void writeQLCDNumber(QString name, bool smalldecimalpt, QLCDNumber::Mode mode, QFrame::Shape shape, QFrame::Shadow shadow);
    void writeQLCDNumber(QString name, double value, bool smalldecimalpt, QLCDNumber::Mode mode, QFrame::Shape shape, QFrame::Shadow shadow);
    void writeQLCDNumber(QString name, bool smalldecimalpt, QLCDNumber::Mode mode, QFrame::Shape shape, QFrame::Shadow shadow, int x, int y, int width, int height);
    void writeQLCDNumber(QString name, double value, bool smalldecimalpt, QLCDNumber::Mode mode, QFrame::Shape shape, QFrame::Shadow shadow, int x, int y, int width, int height);

    void writeQRadioButton(QString name, QString text);
    void writeQRadioButton(QString name, QString text, bool checkable, bool checked);
    void writeQRadioButton(QString name, QString text, int x, int y, int width, int height);
    void writeQRadioButton(QString name, QString text, bool checkable, bool checked, int x, int y, int width, int height);

    void writeQSpinBox(QString name);
    void writeQSpinBox(QString name, int x, int y, int width, int height);
    void writeQSpinBox(QString name, int value, int minimum, int maximum);
    void writeQSpinBox(QString name, int value, int minimum, int maximum, int x, int y, int width, int height);
    void writeQSpinBox(QString name, int value, int minimum, int maximum, int singleStep, bool wrapping, bool readOnly);
    void writeQSpinBox(QString name, int value, int minimum, int maximum, int singleStep, bool wrapping, bool readOnly, int x, int y, int width, int height);
    void writeQSpinBox(QString name, int value, int minimum, int maximum, int singleStep, bool wrapping, bool readOnly, bool frame, QAbstractSpinBox::ButtonSymbols buttonSymbols);
    void writeQSpinBox(QString name, int value, int minimum, int maximum, int singleStep, bool wrapping, bool readOnly, bool frame, QAbstractSpinBox::ButtonSymbols buttonSymbols, int x, int y, int width, int height);

    void writeQDoubleSpinBox(QString name);
    void writeQDoubleSpinBox(QString name, int x, int y, int width, int height);
    void writeQDoubleSpinBox(QString name, double value, double minimum, double maximum);
    void writeQDoubleSpinBox(QString name, double value, double minimum, double maximum, int x, int y, int width, int height);
    void writeQDoubleSpinBox(QString name, double value, double minimum, double maximum, double singleStep, bool wrapping, bool readOnly);
    void writeQDoubleSpinBox(QString name, double value, double minimum, double maximum, double singleStep, bool wrapping, bool readOnly, int x, int y, int width, int height);
    void writeQDoubleSpinBox(QString name, double value, double minimum, double maximum, double singleStep, bool wrapping, bool readOnly, bool frame, QAbstractSpinBox::ButtonSymbols buttonSymbols);
    void writeQDoubleSpinBox(QString name, double value, double minimum, double maximum, double singleStep, bool wrapping, bool readOnly, bool frame, QAbstractSpinBox::ButtonSymbols buttonSymbols, int x, int y, int width, int height);

private:
    void writeStartItem();
    bool inLayout;
    int indentLevel; //current indent level (increases when elements are opened and decreases when they are closed).
    QVector<int> invisibleElements; //the indent levels of active invisible elements (such as items in a layout, which are closed and opened behind the scenes by COSMOSUIWriter).
    QVector<int> activeLayouts; //the indent levels of all currently active layouts (only need to worry about those at the current level).
};

#endif // COSMOSUIWRITER_H
