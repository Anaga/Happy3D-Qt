#include "lasercontrol.h"

LaserControl::LaserControl(QObject *parent) : QObject(parent)
{

}

QString LaserControl::moveMotors(Enums::MoveDirection direction, long distance, long speed)
{
    qDebug() << __PRETTY_FUNCTION__;
    QString commandRow = "";

    switch (direction) {
    case Enums::Up:
        commandRow = "#mx=2,%1,%2";
        commandRow = commandRow.arg(distance).arg(speed);
        break;
    case Enums::Down:
        commandRow = "#mx=1,%1,%2";
        commandRow = commandRow.arg(distance).arg(speed);
        break;
    case Enums::Left:
        commandRow = "#my=2,%1,%2";
        commandRow = commandRow.arg(distance).arg(speed);
        break;
    case Enums::Right:
        commandRow = "#my=1,%1,%2";
        commandRow = commandRow.arg(distance).arg(speed);
        break;
    }

    qDebug() << "commandRow " << commandRow;
    return commandRow;
}

QString LaserControl::initMotors(Enums::MotorAxis ax)
{
    QString commandRow = "#m%1=S,200";

    switch (ax) {
    case Enums::X: commandRow = "#mx=S,200"; break;
    case Enums::Y: commandRow = "#my=S,200"; break;
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
