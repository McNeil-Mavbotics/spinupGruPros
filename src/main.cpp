#include "main.h"
#include "controls.h"
#include "ports.h"

// Global Variables
bool shooting = false;
const int defaultFlywheelSpeedDesired = 450;
const int defaultFlywheelSpeedActual = 390;
const int thirdDiskReduction = 0;
int flywheelRange = 10;
const int intakeSpeed = 200;
const int expansionSpeed = 200;
bool expanding = false;
int autoMode = 0;

void startFlywheel()
{
	flywheel.moveVoltage(-12000 * defaultFlywheelSpeedDesired / 600);
}

void indexIndexer()
{
	int distance = 10;
	int initialPos = indexer.getPosition();
	indexer.moveVelocity(100);
	while (indexer.getPosition() - initialPos < distance)
		pros::delay(20);
	indexer.moveVelocity(-200);
	while (indexer.getPosition() > initialPos)
		pros::delay(20);
	indexer.moveVelocity(0);
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
	left_front_mtr.setBrakeMode(AbstractMotor::brakeMode::coast);
	right_front_mtr.setBrakeMode(AbstractMotor::brakeMode::coast);
	left_back_mtr.setBrakeMode(AbstractMotor::brakeMode::coast);
	right_back_mtr.setBrakeMode(AbstractMotor::brakeMode::coast);
	flywheel.setBrakeMode(AbstractMotor::brakeMode::coast);
	drive->setMaxVelocity(150);
	// Set drive stopping to coast
	drive->getModel()->setBrakeMode(AbstractMotor::brakeMode::coast);
}

void disabled() {}

void competition_initialize() {}

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
	intake.moveVelocity(0);
}

void autonomous()
{
	do
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
	} while (autoMode == 0);
}

void rapidFire()
{
	shooting = true;

	// Start flywheel
	startFlywheel();

	pros::delay(2000);

	// Shoot 3 times
	for (int i = 0; i < 3; i++)
	{
		pros::delay(1000);

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

void manualShoot()
{
	shooting = true;

	// Start flywheel
	startFlywheel();
	while (shooting)
		// Shoot if L2 is pressed
		if (master.operator[](fireBtn).changedToReleased())
			indexIndexer();

	// Stop flywheel
	flywheel.moveVoltage(0);

	shooting = false;
}

void opcontrol()
{
	while (true)
	{
		// Drive
		drive->getModel()->arcade(master.getAnalog(ControllerAnalog::leftY), master.getAnalog(ControllerAnalog::rightX), 0.01);

		// Indexer Retreat
		if (master.operator[](indexRetreatBtn).isPressed())
			indexer.moveVelocity(-100);
		else if (!shooting)
			indexer.moveVelocity(0);

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
			// Slow down drive
			drive->getModel()->setMaxVelocity(30 / 100 * 200);
			drive->getModel()->setBrakeMode(AbstractMotor::brakeMode::hold);
			// Expansion
			if (master.operator[](expansionBtn).isPressed())
				expansion.moveVelocity(expanding ? 0 : expansionSpeed);
		}
		else if (master.operator[](shiftBtn).changedToReleased())
		{
			drive->getModel()->setMaxVelocity(60 / 100 * 200);
			drive->getModel()->setBrakeMode(AbstractMotor::brakeMode::coast);
		}

		master.setText(1, 1, std::to_string(flywheel.getActualVelocity()));

		pros::delay(20);
	}
}