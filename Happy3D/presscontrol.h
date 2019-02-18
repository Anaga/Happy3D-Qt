#ifndef PRESSCONTROL_H
#define PRESSCONTROL_H

#include <QObject>

class PressControl : public QObject
{
    Q_OBJECT
public:
    explicit PressControl(QObject *parent = nullptr);

signals:

public slots:
};

#endif // PRESSCONTROL_H