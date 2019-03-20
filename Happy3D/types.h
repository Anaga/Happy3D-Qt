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
    static QString print(Enums::CommandStatus stat){
        switch (stat) {
        case Enums::CommandStatus::inQueue                  : return "in Queue";
        case Enums::CommandStatus::running                  : return "running";
        case Enums::CommandStatus::notInQueue               : return "not In Queue";
        case Enums::CommandStatus::finishByTimeout          : return "finish By Timeout";
        case Enums::CommandStatus::finishByDelay            : return "finish By Delay";
        case Enums::CommandStatus::finishByCorrectResponce  : return "finish By Correct Responce";
        }
        return "CODE ERROR";
    }


    enum class SendTo:int { notSet, toLaser, toPress };
    Q_ENUM(SendTo)

    static QString print(Enums::SendTo s){
        if (s==SendTo::toLaser) return "to lazer";
        if (s==SendTo::toPress) return "to press";
        return "not set";
    }


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
