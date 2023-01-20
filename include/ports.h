#include "main.h"

// Controller
Controller master;

// Motors
Motor indexer(1, false, AbstractMotor::gearset::green, AbstractMotor::encoderUnits::degrees);
Motor flywheel(2, true, AbstractMotor::gearset::green, AbstractMotor::encoderUnits::degrees);

// Drivetrain Motors
Motor left_front_mtr(3);
Motor right_front_mtr(4);
Motor left_back_mtr(5);
Motor right_back_mtr(6);

// Drivetrain
std::shared_ptr<ChassisController> drive =
    ChassisControllerBuilder()
        .withMotors({left_front_mtr, left_back_mtr}, {right_front_mtr, right_back_mtr})
        .withDimensions(AbstractMotor::gearset::green, {{4_in, 12.5_in}, imev5GreenTPR})
        .withGains({0.001, 0, 0.0001}, {0.001, 0, 0.0001}, {0.001, 0, 0.0001})
        .build();