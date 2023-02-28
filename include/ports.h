#include "main.h"

// Controller
Controller master;

// Motors
Motor indexer(1, false, AbstractMotor::gearset::green, AbstractMotor::encoderUnits::degrees);
Motor flywheel(2, true, AbstractMotor::gearset::blue, AbstractMotor::encoderUnits::degrees);
Motor intake(4, false, AbstractMotor::gearset::green, AbstractMotor::encoderUnits::degrees);
Motor expansion(9, false, AbstractMotor::gearset::green, AbstractMotor::encoderUnits::degrees);

// Drivetrain Motors
Motor left_front_mtr(5, true, AbstractMotor::gearset::green, AbstractMotor::encoderUnits::degrees);
Motor right_front_mtr(8, false, AbstractMotor::gearset::green, AbstractMotor::encoderUnits::degrees);
Motor left_back_mtr(3, false, AbstractMotor::gearset::green, AbstractMotor::encoderUnits::degrees);
Motor right_back_mtr(10, true, AbstractMotor::gearset::green, AbstractMotor::encoderUnits::degrees);

// Drivetrain
std::shared_ptr<ChassisController> drive =
    ChassisControllerBuilder()
        .withMotors({left_front_mtr, left_back_mtr}, {right_front_mtr,
                                                      right_back_mtr})
        .withDimensions(AbstractMotor::gearset::green, {{4_in, 12.5_in}, imev5GreenTPR})
        // .withGains({0, 0, 0}, {0, 0, 0}, {0, 0, 0})
        .build();

std::shared_ptr<AsyncMotionProfileController> trapezoidProfile =
    AsyncMotionProfileControllerBuilder()
        .withLimits({
            1.0, // Maximum linear velocity of the Chassis in m/s
            2.0, // Maximum linear acceleration of the Chassis in m/s/s
            10.0 // Maximum linear jerk of the Chassis in m/s/s/s
        })
        .withOutput(drive)
        .buildMotionProfileController();