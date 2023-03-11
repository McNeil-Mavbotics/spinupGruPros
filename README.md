# Code Explanation for Rubberbandit v2/v3 Robot

The following code is built by the McNeil MiNions robotics team and is for the Rubberbandit v2/v3 robot. Here is a brief explanation of the code:

```
.
├── include/
│   ├── api.h
│   ├── controls.h
│   ├── main.h
│   ├── ports.h
│   └── ...
└── src/
    └── main.cpp
```

## Global Variables

`shooting` is a boolean variable to determine if the robot is currently shooting.<br>
`defaultFlywheelSpeed` is an integer constant for the default speed of the flywheel.<br>
`flywheelSpeed` is an integer variable for the current speed of the flywheel, which is initially set to the default speed.<br>
`flywheelRange` is an integer variable for the allowed range of the flywheel speed.<br>
`intakeSpeed` is an integer constant for the speed of the intake.<br>
`expansionSpeed` is an integer constant for the speed of the expansion.<br>
`expanding` is a boolean variable to determine if the robot is currently expanding.<br>
`autoMode` is an integer variable to determine the autonomous mode of the robot.<br>
`shifted` is a boolean variable to determine if the robot is currently shifted.<br>

## Functions

`startFlywheel()` is a function to start the flywheel and control its speed using the flywheelSpeed variable.<br>
`indexIndexer()` is a function to move the indexer to index a ball into the shooter.<br>
`leftAuton()` and `rightAuton()` are functions to set the autoMode variable to -1 or 1, respectively, and display the corresponding text on the LCD.<br>
`autonomousLeft()` and `autonomousRight()` are functions to set the autonomous program for the left and right side of the field, respectively.<br>
`autonomous()` is a function to run the autonomous program based on the autoMode variable.<br>
`manualShoot()` is a function to manually shoot the ball and control the flywheel speed, and vibration of the controller.<br>
`rapidFire()` is a function to rapidly shoot the ball three times by waiting for the flywheel speed to reach a certain velocity and then calling the `indexIndexer()` function.<br>
Note: The code contains various other functions that have not been explained here as they do not have a significant impact on the overall functionality of the robot.
