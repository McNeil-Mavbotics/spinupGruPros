#include "main.h"
#include "controls.h"
#include "ports.h"

// Global Variables
bool shooting = false;
const int defaultFlywheelSpeed = 360;
int flywheelSpeed = defaultFlywheelSpeed;
int flywheelRange = 10;
int autoMode = 0;
bool shifted = false;

void on_left_button()
{
	autoMode = -1;
	pros::lcd::set_text(1, "Left");
}

void on_right_button()
{
	autoMode = 1;
	pros::lcd::set_text(1, "Right");
}

void initialize()
{
	pros::lcd::initialize();

	pros::lcd::register_btn0_cb(on_left_button);
	pros::lcd::register_btn2_cb(on_right_button);

	indexer.moveRelative(150, 200);
	left_front_mtr.setBrakeMode(AbstractMotor::brakeMode::coast);
	right_front_mtr.setBrakeMode(AbstractMotor::brakeMode::coast);
	flywheel.setBrakeMode(AbstractMotor::brakeMode::coast);
	drive->setMaxVelocity(150);
	// Set drive stopping to coast
	drive->getModel()->setBrakeMode(AbstractMotor::brakeMode::coast);
}

/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled() {}

/**
 * Runs after initialize(), and before autonomous when connected to the Field
 * Management System or the VEX Competition Switch. This is intended for
 * competition-specific initialization routines, such as an autonomous selector
 * on the LCD.
 *
 * This task will exit when the robot is enabled and autonomous or opcontrol
 * starts.
 */
void competition_initialize() {}

/**
 * Runs the user autonomous code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the autonomous
 * mode. Alternatively, this function may be called in initialize or opcontrol
 * for non-competition testing purposes.
 *
 * If the robot is disabled or communications is lost, the autonomous task
 * will be stopped. Re-enabling the robot will restart the task, not re-start it
 * from where it left off.
 */

void autonomousLeft()
{
}

void autonomousRight()
{
}

void autonomous()
{
	switch (autoMode)
	{
	case -1:
		autonomousLeft();
		break;
	case 1:
		autonomousRight();
		break;
	default:
		break;
	}
}

/**
 * Runs the operator control code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the operator
 * control mode.
 *
 * If no competition control is connected, this function will run immediately
 * following initialize().
 *
 * If the robot is disabled or communications is lost, the
 * operator control task will be stopped. Re-enabling the robot will restart the
 * task, not resume it from where it left off.
 */

void manualShoot()
{
	shooting = true;

	// Start flywheel
	flywheel.moveVelocity(flywheelSpeed);
	while (shooting)
	{
		// Vibrate if flywheel is at velocity
		if (flywheel.getActualVelocity() >= flywheelSpeed - flywheelRange && flywheel.getActualVelocity() <= flywheelSpeed + flywheelRange)
			master.rumble(".");

		// Shoot if L2 is pressed
		if (master.operator[](fireBtn).changedToReleased())
			indexer.moveRelative(360, 200);
	}

	// Stop flywheel
	flywheel.moveVelocity(0);

	shooting = false;
}

void rapidFire()
{
	shooting = true;

	// Start flywheel
	flywheel.moveVelocity(flywheelSpeed);

	// Shoot 3 times
	for (int i = 0; i < 3; i++)
	{
		// Wait for flywheel to reach velocity
		while (flywheel.getActualVelocity() <= flywheelSpeed - flywheelRange || flywheel.getActualVelocity() >= flywheelSpeed + flywheelRange)
		{
			pros::delay(20);
		}

		// Shoot
		if (shooting)
			indexer.moveRelative(360, 200);
		else
			return;
	}

	// Stop flywheel
	flywheel.moveVelocity(0);

	shooting = false;
}

void opcontrol()
{
	int switched = 1;

	while (true)
	{
		// Switch direction of drive
		if (master.operator[](switchDriveBtn).changedToReleased())
			switched *= -1;

		// Drive
		if (shifted)
			drive->getModel()->arcade(master.getAnalog(ControllerAnalog::leftY) * switched, master.getAnalog(ControllerAnalog::rightX), 0);
		else
			drive->getModel()->curvature(master.getAnalog(ControllerAnalog::leftY) * switched, master.getAnalog(ControllerAnalog::rightX), 0.01);

		// Start intake
		if (master.operator[](intakeBtn).isPressed())
			intake.moveVelocity(175);
		else if (master.operator[](reverseIntakeBtn).isPressed())
			intake.moveVelocity(-175);
		else
			intake.moveVelocity(0);

		// Rapid Fire
		if (master.operator[](rapidFireBtn).changedToReleased() && !shooting)
			pros::Task rapid(rapidFire);

		// Manually Shoot
		if (master.operator[](manualShootBtn).changedToReleased() && !shooting)
			pros::Task manual(manualShoot);

		// Cancel shooting
		if (master.operator[](cancelShootingBtn).changedToReleased())
		{
			flywheel.moveVelocity(0);
			shooting = false;
		}

		// Shift Controls
		if (master.operator[](shiftBtn).changedToPressed())
		{
			shifted = true;
			// Slow down drive
			drive->getModel()->setMaxVelocity(30 / 100 * 200);
			drive->getModel()->setBrakeMode(AbstractMotor::brakeMode::hold);
			// Expansion
			if (master.operator[](expansionBtn).isPressed())
				expansion.moveRelative(360, 200);
		}
		else if (master.operator[](shiftBtn).changedToReleased())
		{
			shifted = false;
			drive->getModel()->setMaxVelocity(60 / 100 * 200);
			drive->getModel()->setBrakeMode(AbstractMotor::brakeMode::coast);
		}

		pros::delay(20);
	}
}