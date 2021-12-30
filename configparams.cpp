/*
    Original copyright 2018 Benjamin Vedder benjamin@vedder.se and the VESC Tool project ( https://github.com/vedderb/vesc_tool )

    Forked to:
    Copyright 2018 Danny Bokma github@diebie.nl (https://github.com/DieBieEngineering/DieBieMS-Tool)

    Now forked to:
    Copyright 2019 - 2020 Kevin Dionne kevin.dionne@ennoid.me (https://github.com/EnnoidMe/ENNOID-BMS-Tool)

    This file is part of ENNOID-BMS Tool.

    ENNOID-BMS Tool is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    ENNOID-BMS Tool is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "configparams.h"
#include <QDebug>
#include "widgets/parameditdouble.h"
#include "widgets/parameditint.h"
#include "widgets/parameditstring.h"
#include "widgets/parameditenum.h"
#include "widgets/parameditbool.h"
#include <QFile>
#include <QFileInfo>
#include <QBuffer>
#include <cmath>
#include "utility.h"

ConfigParams::ConfigParams(QObject *parent) : QObject(parent)
{
    mUpdateOnlyName.clear();
    mXmlStatus = tr("OK");
    mUpdatesEnabled = true;
}

void ConfigParams::addParam(const QString &name, ConfigParam param)
{
    if (!mParams.contains(name)) {
        mParams.insert(name, param);
        mParamList.append(name);
    } else {
        qWarning() << name << "already present.";
    }
}

void ConfigParams::deleteParam(const QString &name)
{
    mParams.remove(name);
    for (int i = 0;i < mParamList.size();i++) {
        if (mParamList.at(i) == name) {
            mParamList.removeAt(i);
            break;
        }
    }
}

/**
 * @brief ConfigParams::setUpdateOnly
 * Only update parameter with the following name.
 *
 * @param name
 * The name of the parameter to update. If empty all parameters will be updated.
 */
void ConfigParams::setUpdateOnly(const QString &name)
{
    mUpdateOnlyName = name;
}

QString ConfigParams::getUpdateOnly()
{
    return mUpdateOnlyName;
}

bool ConfigParams::getUpdatesEnabled() const
{
    return mUpdatesEnabled;
}

void ConfigParams::setUpdatesEnabled(bool updatesEnabled)
{
    mUpdatesEnabled = updatesEnabled;
}

void ConfigParams::clearParams()
{
    mParams.clear();
    mParamList.clear();
}

void ConfigParams::clearAll()
{
    clearParams();
    clearSerializeOrder();
}

bool ConfigParams::hasParam(const QString &name)
{
    return mParams.contains(name);
}

ConfigParam *ConfigParams::getParam(const QString &name)
{
    ConfigParam *retVal = 0;

    if (mParams.contains(name)) {
        retVal = &mParams[name];
    } else {
        qWarning() << name << "not found";
    }

    return retVal;
}

ConfigParam ConfigParams::getParamCopy(const QString &name) const
{
    ConfigParam retVal;

    if (mParams.contains(name)) {
        retVal = mParams.value(name);
    } else {
        qWarning() << name << "not found";
    }

    return retVal;
}

bool ConfigParams::isParamDouble(const QString &name)
{
    if (mParams.contains(name) && mParams[name].type == CFG_T_DOUBLE) {
        return true;
    } else {
        return false;
    }
}

bool ConfigParams::isParamInt(const QString &name)
{
    if (mParams.contains(name) && mParams[name].type == CFG_T_INT) {
        return true;
    } else {
        return false;
    }
}

bool ConfigParams::isParamEnum(const QString &name)
{
    if (mParams.contains(name) && mParams[name].type == CFG_T_ENUM) {
        return true;
    } else {
        return false;
    }
}

bool ConfigParams::isParamQString(const QString &name)
{
    if (mParams.contains(name) && mParams[name].type == CFG_T_QSTRING) {
        return true;
    } else {
        return false;
    }
}

bool ConfigParams::isParamBool(const QString &name)
{
    if (mParams.contains(name) && mParams[name].type == CFG_T_BOOL) {
        return true;
    } else {
        return false;
    }
}

double ConfigParams::getParamDouble(const QString &name)
{
    double retVal = 0.0;

    if (mParams.contains(name)) {
        ConfigParam &p = mParams[name];

        if (p.type == CFG_T_DOUBLE) {
            retVal = p.valDouble;
        } else {
            qWarning() << name << "wrong type";
        }
    } else {
        qWarning() << name << "not found";
    }

    return retVal;
}

int ConfigParams::getParamInt(const QString &name)
{
    int retVal = 0;

    if (mParams.contains(name)) {
        ConfigParam &p = mParams[name];

        if (p.type == CFG_T_INT) {
            retVal = p.valInt;
        } else {
            qWarning() << name << "wrong type";
        }
    } else {
        qWarning() << name << "not found";
    }

    return retVal;
}

int ConfigParams::getParamEnum(const QString &name)
{
    int retVal = 0;

    if (mParams.contains(name)) {
        ConfigParam &p = mParams[name];

        if (p.type == CFG_T_ENUM) {
            retVal = p.valInt;
        } else {
            qWarning() << name << "wrong type";
        }
    } else {
        qWarning() << name << "not found";
    }

    return retVal;
}

QString ConfigParams::getParamQString(const QString &name)
{
    QString retVal = "";

    if (mParams.contains(name)) {
        ConfigParam &p = mParams[name];

        if (p.type == CFG_T_QSTRING) {
            retVal = p.valString;
        } else {
            qWarning() << name << "wrong type";
        }
    } else {
        qWarning() << name << "not found";
    }

    return retVal;
}

bool ConfigParams::getParamBool(const QString &name)
{
    bool retVal = false;

    if (mParams.contains(name)) {
        ConfigParam &p = mParams[name];

        if (p.type == CFG_T_BOOL) {
            retVal = p.valInt;
        } else {
            qWarning() << name << "wrong type";
        }
    } else {
        qWarning() << name << "not found";
    }

    return retVal;
}

QString ConfigParams::getLongName(const QString &name)
{
    QString retVal = "";

    if (mParams.contains(name)) {
        retVal = mParams[name].longName;
    } else {
        qWarning() << name << "not found";
    }

    return retVal;
}

QString ConfigParams::getDescription(const QString &name)
{
    QString retVal = "";

    if (mParams.contains(name)) {
        retVal = mParams[name].description;
    } else {
        qWarning() << name << "not found";
    }

    return retVal;
}

double ConfigParams::getParamMaxDouble(const QString &name)
{
    double retVal = 0.0;

    if (mParams.contains(name)) {
        ConfigParam &p = mParams[name];

        if (p.type == CFG_T_DOUBLE) {
            retVal = p.maxDouble;
        } else {
            qWarning() << name << "wrong type";
        }
    } else {
        qWarning() << name << "not found";
    }

    return retVal;
}

double ConfigParams::getParamMinDouble(const QString &name)
{
    double retVal = 0.0;

    if (mParams.contains(name)) {
        ConfigParam &p = mParams[name];

        if (p.type == CFG_T_DOUBLE) {
            retVal = p.minDouble;
        } else {
            qWarning() << name << "wrong type";
        }
    } else {
        qWarning() << name << "not found";
    }

    return retVal;
}

double ConfigParams::getParamStepDouble(const QString &name)
{
    double retVal = 0.0;

    if (mParams.contains(name)) {
        ConfigParam &p = mParams[name];

        if (p.type == CFG_T_DOUBLE) {
            retVal = p.stepDouble;
        } else {
            qWarning() << name << "wrong type";
        }
    } else {
        qWarning() << name << "not found";
    }

    return retVal;
}

int ConfigParams::getParamDecimalsDouble(const QString &name)
{
    int retVal = 0;

    if (mParams.contains(name)) {
        ConfigParam &p = mParams[name];

        if (p.type == CFG_T_DOUBLE) {
            retVal = p.editorDecimalsDouble;
        } else {
            qWarning() << name << "wrong type";
        }
    } else {
        qWarning() << name << "not found";
    }

    return retVal;
}

int ConfigParams::getParamMaxInt(const QString &name)
{
    int retVal = 0;

    if (mParams.contains(name)) {
        ConfigParam &p = mParams[name];

        if (p.type == CFG_T_INT) {
            retVal = p.maxInt;
        } else {
            qWarning() << name << "wrong type";
        }
    } else {
        qWarning() << name << "not found";
    }

    return retVal;
}

int ConfigParams::getParamMinInt(const QString &name)
{
    int retVal = 0;

    if (mParams.contains(name)) {
        ConfigParam &p = mParams[name];

        if (p.type == CFG_T_INT) {
            retVal = p.minInt;
        } else {
            qWarning() << name << "wrong type";
        }
    } else {
        qWarning() << name << "not found";
    }

    return retVal;
}

int ConfigParams::getParamStepInt(const QString &name)
{
    int retVal = 0;

    if (mParams.contains(name)) {
        ConfigParam &p = mParams[name];

        if (p.type == CFG_T_INT) {
            retVal = p.stepInt;
        } else {
            qWarning() << name << "wrong type";
        }
    } else {
        qWarning() << name << "not found";
    }

    return retVal;
}

QStringList ConfigParams::getParamEnumNames(const QString &name)
{
    QStringList retVal;

    if (mParams.contains(name)) {
        ConfigParam &p = mParams[name];

        if (p.type == CFG_T_ENUM) {
            retVal = p.enumNames;
        } else {
            qWarning() << name << "wrong type";
        }
    } else {
        qWarning() << name << "not found";
    }

    return retVal;
}

double ConfigParams::getParamEditorScale(const QString &name)
{
    double retVal = 0.0;

    if (mParams.contains(name)) {
        retVal = mParams[name].editorScale;
    } else {
        qWarning() << name << "not found";
    }

    return retVal;
}

QString ConfigParams::getParamSuffix(const QString &name)
{
    QString retVal = "";

    if (mParams.contains(name)) {
        retVal = mParams[name].suffix;
    } else {
        qWarning() << name << "not found";
    }

    return retVal;
}

bool ConfigParams::getParamEditAsPercentage(const QString &name)
{
    bool retVal = false;

    if (mParams.contains(name)) {
        retVal = mParams[name].editAsPercentage;
    } else {
        qWarning() << name << "not found";
    }

    return retVal;
}

bool ConfigParams::getParamShowDisplay(const QString &name)
{
    bool retVal = false;

    if (mParams.contains(name)) {
        retVal = mParams[name].showDisplay;
    } else {
        qWarning() << name << "not found";
    }

    return retVal;
}

bool ConfigParams::getParamTransmittable(const QString &name)
{
    bool retVal = false;

    if (mParams.contains(name)) {
        retVal = mParams[name].transmittable;
    } else {
        qWarning() << name << "not found";
    }

    return retVal;
}

QStringList ConfigParams::getParamOrder() const
{
    return mParamList;
}

void ConfigParams::setParamOrder(const QStringList &order)
{
    // TODO: Add check to make sure that the new order is valid.
    mParamList = order;
}

QWidget *ConfigParams::getEditor(const QString &name, QWidget *parent)
{
    QWidget *retVal = 0;

    if (mParams.contains(name)) {
        ConfigParam &p = mParams[name];

        switch (p.type) {
        case CFG_T_DOUBLE: {
            ParamEditDouble *edit = new ParamEditDouble(parent);
            edit->setName(name);
            edit->setSuffix(p.suffix);
            edit->setDecimals(p.editorDecimalsDouble);
            edit->setShowAsPercentage(p.editAsPercentage);
            edit->showDisplay(p.showDisplay);
            edit->setConfig(this);
            retVal = edit;
        } break;

        case CFG_T_INT: {
            ParamEditInt *edit = new ParamEditInt(parent);
            edit->setName(name);
            edit->setSuffix(p.suffix);
            edit->setShowAsPercentage(p.editAsPercentage);
            edit->showDisplay(p.showDisplay);
            edit->setConfig(this);
            retVal = edit;
        } break;

        case CFG_T_QSTRING: {
            ParamEditString *edit = new ParamEditString(parent);
            edit->setName(name);
            edit->setConfig(this);
            retVal = edit;
        } break;

        case CFG_T_ENUM: {
            ParamEditEnum *edit = new ParamEditEnum(parent);
            edit->setName(name);
            edit->setConfig(this);
            retVal = edit;
        } break;

        case CFG_T_BOOL: {
            ParamEditBool *edit = new ParamEditBool(parent);
            edit->setName(name);
            edit->setConfig(this);
            retVal = edit;
        } break;

        default:
            qWarning() << "no editor for" << name << "could be created";
            break;
        }

    } else {
        qWarning() << name << "not found";
    }

    return retVal;
}

void ConfigParams::getParamSerial(VByteArray &vb, const QString &name)
{
    if (mParams.contains(name)) {
        ConfigParam &p = mParams[name];

        switch (p.type) {
        case CFG_T_UNDEFINED:
            qWarning() << name << ": type not defined.";
            break;

        case CFG_T_DOUBLE:
            if (p.vTx == VESC_TX_DOUBLE16) {
                vb.vbAppendDouble16(p.valDouble, p.vTxDoubleScale);
            } else if (p.vTx == VESC_TX_DOUBLE32) {
                vb.vbAppendDouble32(p.valDouble, p.vTxDoubleScale);
            } else if (p.vTx == VESC_TX_DOUBLE32_AUTO) {
                vb.vbAppendDouble32Auto(p.valDouble);
            } else {
                qWarning() << name << ": wrong tx type set.";
            }
            break;

        case CFG_T_INT:
            if (p.vTx == VESC_TX_UINT8) {
                vb.vbAppendUint8(p.valInt);
            } else if (p.vTx == VESC_TX_INT8) {
                vb.vbAppendInt8(p.valInt);
            } else if (p.vTx == VESC_TX_UINT16) {
                vb.vbAppendUint16(p.valInt);
            } else if (p.vTx == VESC_TX_INT16) {
                vb.vbAppendInt16(p.valInt);
            } else if (p.vTx == VESC_TX_UINT32) {
                vb.vbAppendUint32(p.valInt);
            } else if (p.vTx == VESC_TX_INT32) {
                vb.vbAppendInt32(p.valInt);
            } else {
                qWarning() << name << ": wrong tx type set.";
            }
            break;

        case CFG_T_QSTRING:
            qWarning() << name << ": QString not supported.";
            break;

        case CFG_T_ENUM:
        case CFG_T_BOOL:
            vb.vbAppendInt8(p.valInt);
            break;
        }
    } else {
        qWarning() << name << "not found";
    }
}

void ConfigParams::setParamSerial(VByteArray &vb, const QString &name, QObject *src)
{
    if (mParams.contains(name)) {
        ConfigParam &p = mParams[name];

        switch (p.type) {
        case CFG_T_UNDEFINED:
            qWarning() << name << ": type not defined.";
            break;

        case CFG_T_DOUBLE: {
            double val = 0.0;
            if (p.vTx == VESC_TX_DOUBLE16) {
                val = vb.vbPopFrontDouble16(p.vTxDoubleScale);
            } else if (p.vTx == VESC_TX_DOUBLE32) {
                val = vb.vbPopFrontDouble32(p.vTxDoubleScale);
            } else if (p.vTx == VESC_TX_DOUBLE32_AUTO) {
                val = vb.vbPopFrontDouble32Auto();
            } else {
                qWarning() << name << ": wrong tx type set.";
            }

            if (mUpdatesEnabled && (mUpdateOnlyName.isEmpty() || mUpdateOnlyName == name)) {
                if (p.valDouble != val) {
                    p.valDouble = val;
                    emit paramChangedDouble(src, name, val);
                }
            }
        } break;

        case CFG_T_INT: {
            int val = 0;

            if (p.vTx == VESC_TX_UINT8) {
                val = vb.vbPopFrontUint8();
            } else if (p.vTx == VESC_TX_INT8) {
                val = vb.vbPopFrontInt8();
            } else if (p.vTx == VESC_TX_UINT16) {
                val = vb.vbPopFrontUint16();
            } else if (p.vTx == VESC_TX_INT16) {
                val = vb.vbPopFrontInt16();
            } else if (p.vTx == VESC_TX_UINT32) {
                val = vb.vbPopFrontUint32();
            } else if (p.vTx == VESC_TX_INT32) {
                val = vb.vbPopFrontInt32();
            } else {
                qWarning() << name << ": wrong tx type set.";
            }

            if (mUpdatesEnabled && (mUpdateOnlyName.isEmpty() || mUpdateOnlyName == name)) {
                if (p.valInt != val) {
                    p.valInt = val;
                    emit paramChangedInt(src, name, val);
                }
            }
        } break;

        case CFG_T_QSTRING:
            qWarning() << name << ": QString not supported.";
            break;

        case CFG_T_ENUM:
        case CFG_T_BOOL: {
            int val = vb.vbPopFrontInt8();

            if (mUpdatesEnabled && (mUpdateOnlyName.isEmpty() || mUpdateOnlyName == name)) {
                if (p.valInt != val) {
                    p.valInt = val;
                    if (p.type == CFG_T_BOOL) {
                        emit paramChangedBool(src, name, val);
                    } else {
                        emit paramChangedEnum(src, name, val);
                    }
                }
            }
        } break;
        }
    } else {
        qWarning() << name << "not found";
    }
}

void ConfigParams::updateParamDouble(QString name, double param, QObject *src)
{
    if (!mUpdatesEnabled || (!mUpdateOnlyName.isEmpty() && mUpdateOnlyName != name)) {
        return;
    }

    if (mParams.contains(name)) {
        ConfigParam &p = mParams[name];
        if (p.type == CFG_T_DOUBLE) {
            if (p.valDouble != param) {
                p.valDouble = param;
                emit paramChangedDouble(src, name, param);
            }
        } else {
            qWarning() << name << "wrong type";
        }
    } else {
        qWarning() << name << "not found";
    }
}

void ConfigParams::updateParamInt(QString name, int param, QObject *src)
{
    if (!mUpdatesEnabled || (!mUpdateOnlyName.isEmpty() && mUpdateOnlyName != name)) {
        return;
    }

    if (mParams.contains(name)) {
        ConfigParam &p = mParams[name];
        if (p.type == CFG_T_INT) {
            if (p.valInt != param) {
                p.valInt = param;
                emit paramChangedInt(src, name, param);
            }
        } else {
            qWarning() << name << "wrong type";
        }
    } else {
        qWarning() << name << "not found";
    }
}

void ConfigParams::updateParamEnum(QString name, int param, QObject *src)
{
    if (!mUpdatesEnabled || (!mUpdateOnlyName.isEmpty() && mUpdateOnlyName != name)) {
        return;
    }

    if (mParams.contains(name)) {
        ConfigParam &p = mParams[name];
        if (p.type == CFG_T_ENUM) {
            if (p.valInt != param) {
                p.valInt = param;
                emit paramChangedEnum(src, name, param);
            }
        } else {
            qWarning() << name << "wrong type";
        }
    } else {
        qWarning() << name << "not found";
    }
}

void ConfigParams::updateParamString(QString name, QString param, QObject *src)
{
    if (!mUpdatesEnabled || (!mUpdateOnlyName.isEmpty() && mUpdateOnlyName != name)) {
        return;
    }

    if (mParams.contains(name)) {
        ConfigParam &p = mParams[name];
        if (p.type == CFG_T_QSTRING) {
            if (p.valString != param) {
                p.valString = param;
                emit paramChangedQString(src, name, param);
            }
        } else {
            qWarning() << name << "wrong type";
        }
    } else {
        qWarning() << name << "not found";
    }
}

void ConfigParams::updateParamBool(QString name, bool param, QObject *src)
{
    if (!mUpdatesEnabled || (!mUpdateOnlyName.isEmpty() && mUpdateOnlyName != name)) {
        return;
    }

    if (mParams.contains(name)) {
        ConfigParam &p = mParams[name];
        if (p.type == CFG_T_BOOL) {
            if (p.valInt != param) {
                p.valInt = param;
                emit paramChangedBool(src, name, param);
            }
        } else {
            qWarning() << name << "wrong type";
        }
    } else {
        qWarning() << name << "not found";
    }
}

void ConfigParams::requestUpdate()
{
    emit updateRequested();
}

void ConfigParams::requestUpdateDefault()
{
    emit updateRequestDefault();
}

void ConfigParams::updateDone()
{
    // Accept all names from now on again.
    mUpdateOnlyName.clear();
    emit updated();
}

void ConfigParams::storingDone()
{
    emit stored();
}

// http://realtimecollisiondetection.net/blog/?p=89
bool ConfigParams::almostEqual(float A, float B, float eps)
{
    return fabsf(A - B) <= eps * fmaxf(1.0f, fmaxf(fabsf(A), fabsf(B)));
}

QStringList ConfigParams::getSerializeOrder() const
{
    return mSerializeOrder;
}

void ConfigParams::setSerializeOrder(const QStringList &serializeOrder)
{
    mSerializeOrder = serializeOrder;
}

void ConfigParams::clearSerializeOrder()
{
    mSerializeOrder.clear();
}

void ConfigParams::serialize(VByteArray &vb)
{
    for (int i = 0;i < mSerializeOrder.size();i++) {
        getParamSerial(vb, mSerializeOrder.at(i));
    }
}

void ConfigParams::deSerialize(VByteArray &vb)
{
    for (int i = 0;i < mSerializeOrder.size();i++) {
        setParamSerial(vb, mSerializeOrder.at(i));
    }
}

void ConfigParams::getXML(QXmlStreamWriter &stream, QString configName)
{
    stream.writeStartDocument();
    stream.writeStartElement(configName);

    QHashIterator<QString, ConfigParam> i(mParams);
    QList<QPair<QString, ConfigParam>> pairs;
    while (i.hasNext()) {
        i.next();

        pairs.append(QPair<QString,ConfigParam>(i.key(), i.value()));
    }

    // sort the list
    for (int a=0; a < pairs.count(); a++) {
        for (int b=a+1; b < pairs.count(); b++) {
            QString first=pairs.at(a).first;
            QString second=pairs.at(b).first;
            if (first.toUpper() > second.toUpper()) {
                pairs.move(b, a);
                a=0;
            }
        }
    }

    QListIterator<QPair<QString, ConfigParam>> j(pairs);
    while (j.hasNext()) {
        QPair<QString, ConfigParam> pair = j.next();

        const ConfigParam &p = pair.second;
        QString name = pair.first;

        switch (p.type) {
        case CFG_T_BOOL:
        case CFG_T_ENUM:
        case CFG_T_INT:
            stream.writeTextElement(name, QString::number(p.valInt));
            break;

        case CFG_T_DOUBLE:
            stream.writeTextElement(name, QString::number(p.valDouble));
            break;

        case CFG_T_QSTRING:
            stream.writeTextElement(name, p.valString);
            break;

        case CFG_T_UNDEFINED:
            // Undefined parameters have no value to save.
            break;

        default:
            qWarning() << name << ": type not supported.";
            break;
        }
    }

    stream.writeEndDocument();
}

bool ConfigParams::setXML(QXmlStreamReader &stream, QString configName)
{
    bool nameFound = false;
    while (stream.readNextStartElement()) {
        if (stream.name() == configName) {
            nameFound = true;
            break;
        }
    }

    if (nameFound) {
        while (stream.readNextStartElement()) {
            QString name = stream.name().toString();

            if (mParams.contains(name)) {
                ConfigParam &p = mParams[name];
                QString text = stream.readElementText();
                int valInt = text.toInt();
                double valDouble = text.toDouble();

                switch (p.type) {
                case CFG_T_BOOL:
                    if (valInt != p.valInt) {
                        p.valInt = valInt;
                        emit paramChangedBool(0, name, valInt);
                    }
                    break;

                case CFG_T_ENUM:
                    if (valInt != p.valInt) {
                        p.valInt = valInt;
                        emit paramChangedEnum(0, name, valInt);
                    }
                    break;

                case CFG_T_INT:
                    if (valInt != p.valInt) {
                        p.valInt = valInt;
                        emit paramChangedInt(0, name, valInt);
                    }
                    break;

                case CFG_T_DOUBLE:
                    if (valDouble != p.valDouble) {
                        p.valDouble = valDouble;
                        emit paramChangedDouble(0, name, valDouble);
                    }
                    break;

                case CFG_T_QSTRING:
                    if (text != p.valString) {
                        p.valString = text;
                        emit paramChangedQString(0, name, text);
                    }
                    break;

                default:
                    qWarning() << name << ": type not supported.";
                    break;
                }
            } else {
                qWarning() << "Parameter not found: " << name;
                stream.skipCurrentElement();
            }

            if (stream.hasError()) {
                qWarning() << "XML ERROR :" << stream.errorString();
                qWarning() << stream.lineNumber() << stream.columnNumber();
            }
        }

        mXmlStatus = tr("OK");
        emit updated();
        return true;
    } else {
        mXmlStatus = tr("tag <b>%1</b> not found").arg(configName);
        qWarning() << mXmlStatus;
        return false;
    }
}

bool ConfigParams::saveXml(QString fileName, QString configName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) {
        mXmlStatus = tr("Could not open %1 for writing").arg(fileName);
        qWarning() << mXmlStatus;
        return false;
    }

    emit savingXml();

    QXmlStreamWriter stream(&file);
    stream.setCodec("UTF-8");
    stream.setAutoFormatting(true);
    getXML(stream, configName);

    file.close();

    mXmlStatus = tr("OK");
    return true;
}

bool ConfigParams::loadXml(QString fileName, QString configName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        mXmlStatus = tr("Could not open %1 for reading").arg(fileName);
        qWarning() << mXmlStatus;
        return false;
    }

    QXmlStreamReader stream(&file);
    bool res = setXML(stream, configName);
    file.close();

    return res;
}

QString ConfigParams::xmlStatus()
{
    return mXmlStatus;
}

void ConfigParams::getParamsXML(QXmlStreamWriter &stream)
{
    stream.writeStartDocument();
    stream.writeStartElement("ConfigParams");
    stream.writeStartElement("Params");

    for (int i = 0;i < mParamList.size();i++) {
        QString paramName = mParamList.at(i);
        ConfigParam *p = getParam(paramName);

        stream.writeStartElement(paramName);

        stream.writeTextElement("longName", p->longName);
        stream.writeTextElement("type", QString::number(p->type));
        stream.writeTextElement("transmittable", QString::number(p->transmittable));
        stream.writeTextElement("description", p->description);
        stream.writeTextElement("cDefine", p->cDefine);

        switch (p->type) {
        case CFG_T_DOUBLE:
            stream.writeTextElement("editorDecimalsDouble", QString::number(p->editorDecimalsDouble));
            stream.writeTextElement("editorScale", QString::number(p->editorScale));
            stream.writeTextElement("editAsPercentage", QString::number(p->editAsPercentage));
            stream.writeTextElement("maxDouble", QString::number(p->maxDouble));
            stream.writeTextElement("minDouble", QString::number(p->minDouble));
            stream.writeTextElement("showDisplay", QString::number(p->showDisplay));
            stream.writeTextElement("stepDouble", QString::number(p->stepDouble));
            stream.writeTextElement("valDouble", QString::number(p->valDouble));
            stream.writeTextElement("vTxDoubleScale", QString::number(p->vTxDoubleScale));
            stream.writeTextElement("suffix", p->suffix);
            stream.writeTextElement("vTx", QString::number(p->vTx));
            break;

        case CFG_T_INT:
            stream.writeTextElement("editorScale", QString::number(p->editorScale));
            stream.writeTextElement("editAsPercentage", QString::number(p->editAsPercentage));
            stream.writeTextElement("maxInt", QString::number(p->maxInt));
            stream.writeTextElement("minInt", QString::number(p->minInt));
            stream.writeTextElement("showDisplay", QString::number(p->showDisplay));
            stream.writeTextElement("stepInt", QString::number(p->stepInt));
            stream.writeTextElement("valInt", QString::number(p->valInt));
            stream.writeTextElement("suffix", p->suffix);
            stream.writeTextElement("vTx", QString::number(p->vTx));
            break;

        case CFG_T_QSTRING:
            stream.writeTextElement("valString", p->valString);
            break;

        case CFG_T_ENUM:
            stream.writeTextElement("valInt", QString::number(p->valInt));
            for (int j = 0;j < p->enumNames.size();j++) {
                stream.writeTextElement("enumNames", p->enumNames.at(j));
            }
            break;

        case CFG_T_BOOL:
            stream.writeTextElement("valInt", QString::number(p->valInt));
            break;

        default:
            break;
        }

        stream.writeEndElement();
    }

    stream.writeEndElement();

    stream.writeStartElement("SerOrder");
    for (int i = 0;i < mSerializeOrder.size();i++) {
        stream.writeTextElement("ser", mSerializeOrder.at(i));
    }

    stream.writeEndDocument();
}

bool ConfigParams::setParamsXML(QXmlStreamReader &stream)
{
    QString configName = "ConfigParams";

    bool nameFound = false;
    while (stream.readNextStartElement()) {
        if (stream.name() == configName) {
            nameFound = true;
            break;
        }
    }

    if (nameFound) {
        while (stream.readNextStartElement()) {
            QString nameFirst = stream.name().toString();

            if (nameFirst == "Params") {
                clearParams();

                while (stream.readNextStartElement()) {
                    QString paramName = stream.name().toString();
                    ConfigParam p;

                    while (stream.readNextStartElement()) {
                        QString name = stream.name().toString();

                        if (name == "description") {
                            p.description = stream.readElementText();
                        } else if (name == "cDefine") {
                            p.cDefine = stream.readElementText();
                        } else if (name == "editorDecimalsDouble") {
                            p.editorDecimalsDouble = stream.readElementText().toDouble();
                        } else if (name == "editorScale") {
                            p.editorScale = stream.readElementText().toDouble();
                        } else if (name == "editAsPercentage") {
                            p.editAsPercentage = stream.readElementText().toInt();
                        } else if (name == "enumNames") {
                            p.enumNames.append(stream.readElementText());
                        } else if (name == "longName") {
                            p.longName = stream.readElementText();
                        } else if (name == "maxDouble") {
                            p.maxDouble = stream.readElementText().toDouble();
                        } else if (name == "maxInt") {
                            p.maxInt = stream.readElementText().toInt();
                        } else if (name == "minDouble") {
                            p.minDouble = stream.readElementText().toDouble();
                        } else if (name == "minInt") {
                            p.minInt = stream.readElementText().toInt();
                        } else if (name == "showDisplay") {
                            p.showDisplay = stream.readElementText().toInt();
                        } else if (name == "stepDouble") {
                            p.stepDouble = stream.readElementText().toDouble();
                        } else if (name == "stepInt") {
                            p.stepInt = stream.readElementText().toInt();
                        } else if (name == "suffix") {
                            p.suffix = stream.readElementText();
                        } else if (name == "type") {
                            p.type = (CFG_T)stream.readElementText().toInt();
                        } else if (name == "transmittable") {
                            p.transmittable = stream.readElementText().toInt();
                        } else if (name == "valDouble") {
                            p.valDouble = stream.readElementText().toDouble();
                        } else if (name == "valInt") {
                            p.valInt = stream.readElementText().toInt();
                        } else if (name == "valString") {
                            p.valString = stream.readElementText();
                        } else if (name == "vTx") {
                            p.vTx = (VESC_TX_T)stream.readElementText().toInt();
                        } else if (name == "vTxDoubleScale") {
                            p.vTxDoubleScale = stream.readElementText().toDouble();
                        } else {
                            qWarning() << "Parameter not found: " << name;
                            stream.skipCurrentElement();
                        }

                        if (stream.hasError()) {
                            qWarning() << "XML ERROR :" << stream.errorString();
                            qWarning() << stream.lineNumber() << stream.columnNumber();
                        }
                    }

                    addParam(paramName, p);
                }
            } else if (nameFirst == "SerOrder") {
                mSerializeOrder.clear();
                while (stream.readNextStartElement()) {
                    QString name = stream.name().toString();

                    if (name == "ser") {
                        mSerializeOrder.append(stream.readElementText());
                    } else {
                        qWarning() << "Parameter not found: " << name;
                        stream.skipCurrentElement();
                    }
                }
            } else {
                qWarning() << "Parameter not found: " << nameFirst;
                stream.skipCurrentElement();
            }
        }

        mXmlStatus = tr("OK");
        return true;
    } else {
        mXmlStatus = tr("tag <b>%1</b> not found").arg(configName);
        qWarning() << mXmlStatus;
        return false;
    }
}

bool ConfigParams::saveParamsXml(QString fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) {
        mXmlStatus = tr("Could not open %1 for writing").arg(fileName);
        qWarning() << mXmlStatus;
        return false;
    }

    QXmlStreamWriter stream(&file);
    stream.setCodec("UTF-8");
    stream.setAutoFormatting(true);

    getParamsXML(stream);

    file.close();

    mXmlStatus = tr("OK");
    return true;
}

bool ConfigParams::loadParamsXml(QString fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        mXmlStatus = tr("Could not open %1 for reading").arg(fileName);
        qWarning() << mXmlStatus;
        return false;
    }

    QXmlStreamReader stream(&file);
    bool res = setParamsXML(stream);

    file.close();

    return res;
}

bool ConfigParams::saveCDefines(const QString &fileName, bool wrapIfdef)
{
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) {
        mXmlStatus = tr("Could not open %1 for writing").arg(fileName);
        qWarning() << mXmlStatus;
        return false;
    }

    QTextStream out(&file);
    QFileInfo info(file);
    QString nameStr = info.fileName().toUpper().replace(".", "_") + "_";

    out << "// This file is autogenerated by EBMS Tool\n\n";

    out << "#ifndef " + nameStr + "\n";
    out << "#define " + nameStr + "\n\n";

    for (int i = 0;i < mSerializeOrder.size();i++) {
        QString name = mSerializeOrder.at(i);

        if (mParams.contains(name)) {
            ConfigParam &p = mParams[name];

            if (!p.cDefine.isEmpty()) {
                out << "// " + p.longName + "\n";
                if (wrapIfdef) {
                    out << "#ifndef " + p.cDefine + "\n";
                }

                switch (p.type) {
                case CFG_T_BOOL:
                case CFG_T_ENUM:
                case CFG_T_INT:
                    out << "#define " + p.cDefine + " " + QString::number(p.valInt) + "\n";
                    break;

                case CFG_T_DOUBLE:
                    out << "#define " + p.cDefine + " " + QString::number(p.valDouble) + "\n";
                    break;

                case CFG_T_QSTRING:
                    out << "#define " + p.cDefine + " " + p.valString + "\n";
                    break;

                default:
                    qWarning() << name << ": type not supported.";
                    break;
                }

                if (wrapIfdef) {
                    out << "#endif\n";
                }

                out << "\n";
            }
        } else {
            qWarning() << name << "not found.";
        }
    }

    out << "// " + nameStr + "\n";
    out << "#endif\n\n";

    out.flush();
    file.close();

    return true;
}

/**
 * @brief ConfigParams::checkDifference
 * Check which parameters differ between this configuration and another one.
 *
 * @param config
 * The configuration to check against.
 *
 * @return
 * A list with parameters that differ.
 */
QStringList ConfigParams::checkDifference(ConfigParams *config)
{
    QStringList res;

    for(QString p: mParamList) {
        ConfigParam *thisParam = this->getParam(p);
        ConfigParam *otherParam = config->getParam(p);

        if (thisParam && otherParam) {
            if (thisParam->type == otherParam->type) {
                switch (thisParam->type) {
                case CFG_T_BOOL:
                case CFG_T_ENUM:
                case CFG_T_INT:
                    if (thisParam->valInt != otherParam->valInt) {
                        res.append(p);
                    }
                    break;

                case CFG_T_DOUBLE:
                    if (!almostEqual(thisParam->valDouble, otherParam->valDouble, 0.0001)) {
                        res.append(p);
                    }
                    break;

                case CFG_T_QSTRING:
                    if (thisParam->valString != otherParam->valString) {
                        res.append(p);
                    }
                    break;

                default:
                    break;
                }
            }
        }
    }

    return res;
}

ConfigParams &ConfigParams::operator=(const ConfigParams &other)
{
    this->mParams = other.mParams;
    this->mParamList = other.mParamList;
    this->mUpdateOnlyName = other.mUpdateOnlyName;
    this->mUpdatesEnabled = other.mUpdatesEnabled;
    this->mSerializeOrder = other.mSerializeOrder;
    this->mXmlStatus = other.mXmlStatus;

    return *this;
}
