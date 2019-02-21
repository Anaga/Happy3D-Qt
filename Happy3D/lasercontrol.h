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
    QString movePlate(MoveDirection direction, long distance, long speed);
    QString moveWiper(MoveDirection direction, long distance, long speed);
    QString stopLaser();
    QString stopMotor();

};

#endif // LASERCONTROL_H
