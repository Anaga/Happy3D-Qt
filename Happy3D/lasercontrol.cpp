#include "lasercontrol.h"

LaserControl::LaserControl(QObject *parent) : QObject(parent)
{

}

QString LaserControl::movePlate(bool direction, qreal distance, qreal speed)
{
    qDebug() << __PRETTY_FUNCTION__;
    QString commandRow = "#mx = %1,%2,%3";
    if (direction) {
        commandRow = commandRow.arg(1).arg(distance).arg(speed);
    }else {
        commandRow = commandRow.arg(2).arg(distance).arg(speed);
    }
    qDebug() << "commandRow " << commandRow;
    return commandRow;
}

QString LaserControl::moveWiper(bool direction, qreal distance, qreal speed)
{
    qDebug() << __PRETTY_FUNCTION__;
    QString commandRow = "#my = %1,%2,%3";
    if (direction) {
        commandRow = commandRow.arg(1).arg(distance).arg(speed);
    }else {
        commandRow = commandRow.arg(2).arg(distance).arg(speed);
    }
    qDebug() << "commandRow " << commandRow;
    return commandRow;
}
