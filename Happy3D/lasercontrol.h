#ifndef LASERCONTROL_H
#define LASERCONTROL_H

#include <QObject>
#include "communicaton.h"
#include <QFloat16>
#include "types.h"

class LaserControl : public QObject
{
    Q_OBJECT
public:
    explicit LaserControl(QObject *parent = nullptr);

signals:

public slots:
    QString moveMotors(MoveDirection direction, long distance, long speed);
    QString initMotors(MotorAxis ax);
    QString stopLaser();
    QString stopMotor();

};

#endif // LASERCONTROL_H
