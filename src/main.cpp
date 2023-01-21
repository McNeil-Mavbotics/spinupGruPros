#include "main.h"
#include "ports.h"

// Global Variables
bool shooting = false;
int flywheelSpeed = 200;
int flywheelRange = 10;

/**
 * A callback function for LLEMU's center button.
 *
 * When this callback is fired, it will toggle line 2 of the LCD text between
 * "I was pressed!" and nothing.
 */
void on_center_button()
{
	// TODO: Print debug stats
}

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize()
{
	pros::lcd::initialize();
	pros::lcd::set_text(1, "Ready to shred!");

	pros::lcd::register_btn1_cb(on_center_button);

	indexer.moveRelative(150, 200);
	flywheel.setBrakeMode(AbstractMotor::brakeMode::coast);
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
void autonomous() {}

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
	// Start flywheel
	flywheel.moveVelocity(200);
	while (shooting)
	{
		// Vibrate if flywheel is at velocity
		if (flywheel.getActualVelocity() <= flywheelSpeed - flywheelRange || flywheel.getActualVelocity() >= flywheelSpeed + flywheelRange)
			master.rumble(".");

		// Shoot if R2 is pressed
		if (master.operator[](ControllerDigital::R2).changedToReleased())
			indexer.moveRelative(360, 200);
	}

	// Stop flywheel
	flywheel.moveVelocity(0);
}

void rapidFire()
{
	shooting = true;

	// Start flywheel
	flywheel.moveVelocity(200);

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
	// Set drive stopping to cost
	drive->getModel()->setBrakeMode(AbstractMotor::brakeMode::coast);

	while (true)
	{
		// Switch direction of drive if B is pressed
		if (master.operator[](ControllerDigital::B).changedToReleased())
			switched *= -1;

		// Drive
		drive->getModel()->arcade(master.getAnalog(ControllerAnalog::leftY) * switched, master.getAnalog(ControllerAnalog::rightX), (0.01));

		// Rapid Fire if Y is pressed
		if (master.operator[](ControllerDigital::Y).changedToReleased())
			rapidFire();

		// Cancel shooting if left is pressed
		if (master.operator[](ControllerDigital::left).changedToReleased())
		{
			flywheel.moveVelocity(0);
			shooting = false;
		}

		// Slow down drive if R1 is pressed and set flywheel speed to left x axis, but center is 200
		if (master.operator[](ControllerDigital::R1).changedToPressed())
		{
			drive->getModel()->setMaxVelocity(150);
			flywheelSpeed = std::min(master.getAnalog(ControllerAnalog::leftX) + 200, 400.0f);
		}
		else if (master.operator[](ControllerDigital::R1).changedToReleased())
			drive->getModel()->setMaxVelocity(200);
	}
}