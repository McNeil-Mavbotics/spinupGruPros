#include "main.h"
#include "controls.h"
#include "ports.h"

// Global Variables
bool shooting = false;
const int defaultFlywheelSpeed = 450;
int flywheelSpeed = defaultFlywheelSpeed;
int flywheelRange = 10;
const int intakeSpeed = 200;
const int expansionSpeed = 200;
bool expanding = false;
int autoMode = 0;
bool shifted = false;

void startFlywheel()
{
	flywheel.moveVoltage(-12000 * flywheelSpeed / 600);
}

void indexIndexer()
{
	int distance = 30;
	int initialPos = indexer.getPosition();
	indexer.moveVelocity(100);
	while (indexer.getPosition() - initialPos < distance)
		pros::delay(20);
	indexer.moveVelocity(-200);
	while (indexer.getPosition() > initialPos)
		pros::delay(20);
	indexer.moveVelocity(-10);
}

void leftAuton()
{
	autoMode = -1;
	pros::lcd::set_text(0, "Left");
}

void rightAuton()
{
	autoMode = 1;
	pros::lcd::set_text(0, "Right");
}

void initialize()
{
	pros::lcd::initialize();

	pros::lcd::register_btn0_cb(leftAuton);
	pros::lcd::register_btn2_cb(rightAuton);

	indexer.moveVelocity(-10);
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
	intake.moveVelocity(intakeSpeed);
	std::initializer_list<PathfinderPoint> points = {PathfinderPoint{0_ft, 0.5_ft, 0_deg}, PathfinderPoint{0_ft, 1.5_ft, -90_deg}};
	trapezoidProfile.get()
		->generatePath(points, "A");
	trapezoidProfile.get()
		->setTarget("A");
	rapidFire();
}

void autonomous()
{
	while (autoMode == 0)
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
		pros::delay(20);
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

	int previousSpeed = 0;

	// Start flywheel
	startFlywheel();
	while (shooting)
	{
		// Vibrate if flywheel is at velocity
		if (flywheel.getActualVelocity() >= flywheelSpeed - flywheelRange && flywheel.getActualVelocity() <= flywheelSpeed + flywheelRange && previousSpeed >= flywheelSpeed - flywheelRange && previousSpeed <= flywheelSpeed + flywheelRange)
			master.rumble(".");

		// Shoot if L2 is pressed
		if (master.operator[](fireBtn).changedToReleased())
			indexIndexer();

		previousSpeed = flywheel.getActualVelocity();
	}

	// Stop flywheel
	flywheel.moveVoltage(0);

	shooting = false;
}

void rapidFire()
{
	shooting = true;

	// Start flywheel
	startFlywheel();

	int previousSpeed = 0;

	// Shoot 3 times
	for (int i = 0; i < 3; i++)
	{
		// Wait for flywheel to reach velocity
		while (!(flywheel.getActualVelocity() >= flywheelSpeed - flywheelRange && flywheel.getActualVelocity() <= flywheelSpeed + flywheelRange && previousSpeed >= flywheelSpeed - flywheelRange && previousSpeed <= flywheelSpeed + flywheelRange))
		{
			previousSpeed = flywheel.getActualVelocity();
			pros::delay(20);
		}

		// Shoot
		if (shooting)
			indexIndexer();
		else
			return;
	}

	// Stop flywheel
	flywheel.moveVoltage(0);

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
			intake.moveVelocity(intakeSpeed);
		else if (master.operator[](reverseIntakeBtn).isPressed())
			intake.moveVelocity(-intakeSpeed);
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
			flywheel.moveVoltage(0);
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
				expansion.moveVelocity(expanding ? 0 : expansionSpeed);
		}
		else if (master.operator[](shiftBtn).changedToReleased())
		{
			shifted = false;
			drive->getModel()->setMaxVelocity(60 / 100 * 200);
			drive->getModel()->setBrakeMode(AbstractMotor::brakeMode::coast);
		}

		pros::lcd::print(1, "Flywheel: %f", flywheel.getActualVelocity());

		pros::delay(20);
	}
}