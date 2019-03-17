#ifndef TYPES_H
#define TYPES_H

#include <QString>
#include <QObject>


class Enums : public QObject { 
public:
    enum class CommandStatus: int {
        notInQueue,
        inQueue,
        running,
        finishByCorrectResponce,
        finishByDelay,
        finishByTimeout,
    };
    Q_ENUM(CommandStatus)


    enum MotorAxis { X, Y };
    Q_ENUM(MotorAxis)


    enum MoveDirection:int { Left, Right, Up, Down };
    Q_ENUM(MoveDirection)

    enum CheckType:int { Integer, Double };
    Q_ENUM(CheckType)

    Q_GADGET ///< Он легче Q_OBJECT и вообще скромняшка!

    Enums() = delete; ///< std=c++11, обеспечивает запрет на создание любого экземпляра Enums
};
/*
enum CheckType:int { Integer, Double };
enum MoveDirection:int { Left, Right, Up, Down };
enum MotorAxis:int { X, Y };

*/

#endif // TYPES_H
