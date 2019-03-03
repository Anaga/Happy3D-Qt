#include "lasercontrol.h"

LaserControl::LaserControl(QObject *parent) : QObject(parent)
{

}

QString LaserControl::moveMotors(MoveDirection direction, long distance, long speed)
{
    qDebug() << __PRETTY_FUNCTION__;
    QString commandRow = "";

    switch (direction) {
    case Up:
        commandRow = "#mx=2,%1,%2";
        commandRow = commandRow.arg(distance).arg(speed);
        break;
    case Down:
        commandRow = "#mx=1,%1,%2";
        commandRow = commandRow.arg(distance).arg(speed);
        break;
    case Left:
        commandRow = "#my=2,%1,%2";
        commandRow = commandRow.arg(distance).arg(speed);
        break;
    case Right:
        commandRow = "#my=1,%1,%2";
        commandRow = commandRow.arg(distance).arg(speed);
        break;
    }

    qDebug() << "commandRow " << commandRow;
    return commandRow;
}

QString LaserControl::initMotors(MotorAxis ax)
{
    QString commandRow = "#m%1=S,200";

    switch (ax) {
    case X: commandRow = "#mx=S,200"; break;
    case Y: commandRow = "#my=S,200"; break;
    }
    qDebug() << "commandRow " << commandRow;
    return commandRow;
}

QString LaserControl::stopLaser()
{
    return "#pow=0";
}

QString LaserControl::stopMotor()
{
    return "#my=0";
}
