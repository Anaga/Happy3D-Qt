#ifndef LASERCONTROL_H
#define LASERCONTROL_H

#include <QObject>
#include "communicaton.h"
#include <QFloat16>

class LaserControl : public QObject
{
    Q_OBJECT
public:
    explicit LaserControl(QObject *parent = nullptr);

signals:

public slots:
    void movePlate(bool direction, qreal distance, qreal speed);
    void moveWiper(bool direction, qreal distance, qreal speed);
};

#endif // LASERCONTROL_H
