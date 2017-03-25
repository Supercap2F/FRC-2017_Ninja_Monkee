#include <iostream>
#include <memory>
#include <string>
#include <fstream>
#include <stdlib.h>

#include <IterativeRobot.h>
#include <LiveWindow/LiveWindow.h>
#include <SmartDashboard/SendableChooser.h>
#include <SmartDashboard/SmartDashboard.h>

#include <Joystick.h>
#include <RobotDrive.h>
#include <Timer.h>
#include <CameraServer.h>

#include <Encoder.h>
#include <TalonSRX.h>
#include <I2c.h>

#include <CANTalon.h>
#include <Timer.h>
#include <Servo.h>
#include <ctime>

//Saving stuff
#include "fleet.h"
#include "toolbox.h"
#include "tests/test.cpp"

class Robot: public frc::IterativeRobot {
public:
	fleet config_fleet;
	actions::driveX config_x;
	actions::driveY config_y;
	actions::driveZ config_z;
	actions::gyro config_gyro;
	actions::driveGear config_gear;
	actions::agitator config_agitator;
	actions::shoot config_shoot;
	string config_folder = "/home/lvuser/autoConfigs/";
	string autoRecordPath;bool autoRecord;
	unsigned int autoCustomLPos = 0;
	clock_t timestamp = 0;
	double autoTime = 0;

	class shooterConfig {
	public:
		double shooterSlow = 0;
		double shooterNormal = 0;
		double shooterFull = 0;
	};

	class config {
	public:
		shooterConfig config_BLUE;
		shooterConfig config_RED;
		void loadConfig() {
			std::string temp;
			std::ifstream ifile("/home/lvuser/NinjaConfig.txt");
			if (ifile.good()) {
				//shooter low configs
				getline(ifile, temp); // config one RED team
				config_RED.shooterSlow = atoi(temp.c_str());
				getline(ifile, temp); // config two BLUE team
				config_BLUE.shooterSlow = atoi(temp.c_str());

				//shooter normal configs
				getline(ifile, temp); // config one RED team
				config_RED.shooterNormal = atoi(temp.c_str());
				getline(ifile, temp); // config two BLUE team
				config_BLUE.shooterNormal = atoi(temp.c_str());

				//shooter full configs
				getline(ifile, temp); // config one RED team
				config_RED.shooterFull = atoi(temp.c_str());
				getline(ifile, temp); // config two BLUE team
				config_BLUE.shooterFull = atoi(temp.c_str());
			}
			ifile.close();
		}

		void saveConfig() {
			/*
			 std::ofstream ofile("/home/lvuser/NinjaConfig.txt");
			 config_RED.shooterSlow+"\n"+config_BLUE.shooterSlow+"\n"+
			 config_RED.shooterNormal+"\n"+config_BLUE.shooterNormal+"\n"+;
			 ofile.close();
			 */
		}
	};

	/* x = strafe
	 * y = forward/backward
	 * z = turn
	 * gyro = gyro
	 */
	void drive(double x, double y, double z, double gyro = 0) {
		switch (driveLevel) {
		case (slow):
			x = (x / 1) * strafeSlow;
			y = (y / 1) * driveSlow;
			z = (z / 1) * turnSlow;
			break;
		case (normal):
			x = (x / 1) * strafeNormal;
			y = (y / 1) * driveNormal;
			z = (z / 1) * turnNormal;
			break;
		case (full):
			x = (x / 1) * strafeFull;
			y = (y / 1) * driveFull;
			z = (z / 1) * turnFull;
			break;
		default:
			x = 0;
			y = 0;
			z = 0;
		}
		robotDrive.MecanumDrive_Cartesian(x, y, z, gyro);
		frc::SmartDashboard::PutNumber("x", x);
		frc::SmartDashboard::PutNumber("y", y);
		frc::SmartDashboard::PutNumber("z", z);
	}

	void updateDrive(double x, double y, double z, double gyro = 0,
	bool update = true) {
		if (update) {
			driveX = x, driveY = y, driveZ = z, driveGyro = gyro;
		}
		drive(driveX, driveY, driveZ, driveGyro);

	}

	void run_agitator() {
		switch (agitatorLevel) {
		case (normal):
			agitator->Set(-1);
			break;
		case (reverse):
			agitator->Set(1);
			break;
		default:
			agitator->Set(0);
			break;
		}
	}

	void run_shooter() {
		double shooterSpeed = 0;
		switch (shooterLevel) {
		case (slow):
			shooterSpeed = shooterSlow / 10000;
			break;
		case (normal):
			shooterSpeed = shooterNormal / 10000;
			break;
		case (full):
			shooterSpeed = shooterFull / 10000;
			break;
		case (reverse):
			shooterSpeed = -1;
			break;
		default:
			shooterSpeed = 0;
			break;
		}
		frc::SmartDashboard::PutNumber("shooter speed", shooterSpeed);
		shooter.Set(shooterSpeed);
	}

	void run_winch() {
		switch (winchLevel) {
		case (normal):
			winch->Set(winchNormal);
			break;
		default:
			winch->Set(0);
			break;
		}
	}

	void getShooterConfig() {
		int temp = frc::SmartDashboard::GetNumber("shooterFull", -1);
		if (temp >= 0 && temp != shooterFull) {
			shooterFull = temp;
			std::ofstream file(pathShooterFull);
			file << temp;
			file.close();
		}

		temp = frc::SmartDashboard::GetNumber("shooterNormal", -1);
		if (temp >= 0 && temp != shooterNormal) {
			shooterNormal = temp;
			std::ofstream file(pathShooterNormal);
			file << temp;
			file.close();
		}

		temp = frc::SmartDashboard::GetNumber("shooterSlow", -1);
		if (temp >= 0 && temp != shooterSlow) {
			shooterSlow = temp;
			std::ofstream file(pathShooterSlow);
			file << temp;
			file.close();
		}
	}

	void pollControllers() {
		//gamepad: Drive control polling
		double leftdead = .1;
		double rightdead = .1;
		double tmp = 0;
		tmp = gamepad.GetX();
		frc::SmartDashboard::PutNumber("X = ", tmp);
		frc::SmartDashboard::PutBoolean("X>.5", tmp > 0.5);
		if (tmp > leftdead) {
			strafe = ((tmp - leftdead) * (1 / (1 - leftdead)));
		} else if (tmp < -leftdead) {
			strafe = ((tmp + leftdead) * (-1 / (-1 + leftdead)));
		} else {
			strafe = 0;
		}
		tmp = gamepad.GetY();
		frc::SmartDashboard::PutNumber("Y = ", tmp);
		if (tmp > leftdead) {
			forwardBackward = ((tmp - leftdead) * (1 / (1 - leftdead)));
		} else if (tmp < -leftdead) {
			forwardBackward = ((tmp + leftdead) * (-1 / (-1 + leftdead)));
		} else {
			forwardBackward = 0;
		}
		tmp = gamepad.GetZ();
		if (tmp > rightdead) {
			turn = ((tmp - rightdead) * (1 / (1 - rightdead)));
		} else if (tmp < -rightdead) {
			turn = ((tmp + rightdead) * (-1 / (-1 + rightdead)));
		} else {
			turn = 0;
		}

		//Drive Levels 'Gears'
		if (gamepad.GetRawButton(7)) { // Drive override
			if (driveLevel != slow) {
				driveLevel = slow;
			}
		} else {
			if (gamepad.GetRawButton(6) && driveLevel != full) {
				driveLevel = full;
			} else if (gamepad.GetRawButton(8) && driveLevel != normal) {
				driveLevel = normal;
			}
		}

		//button panel: Shooter control polling
		bool bslow = panel.GetRawButton(3), bnormal = (panel.GetRawButton(2)
				|| gamepad.GetRawButton(1)), bfull = panel.GetRawButton(1),
				breverse = panel.GetRawButton(7);
		if (breverse == true && shooterLevel != reverse) {
			shooterLevel = reverse;
		} else if (bslow == true && shooterLevel != slow) {
			shooterLevel = slow;
		} else if (bnormal == true && shooterLevel != normal) {
			shooterLevel = normal;
		} else if (bfull == true && shooterLevel != full) {
			shooterLevel = full;
		} else if (!bslow && !bnormal && !bfull && shooterLevel != stop) {
			shooterLevel = stop;
		}

		// agitator control polling
		if (panel.GetRawButton(6) || gamepad.GetRawButton(2)) { // Run agitator forward
			agitatorLevel = normal;
		} else if (panel.GetRawButton(5)) { // Run agitator in reverse
			agitatorLevel = reverse;
		} else {
			agitatorLevel = stop;
		}
		frc::SmartDashboard::PutNumber("runAgitator", agitatorLevel);

		if (panel.GetRawButton(8)) {
			winchLevel = normal;
		} else {
			winchLevel = stop;
		}

		if (panel.GetRawButton(4)) {
			if (!doorDrop[0]) {
				doorDrop[0] = true;
				doorDrop[1] = !doorDrop[1];
			}
		} else {
			doorDrop[0] = false;
		}

		//Drive Direction
		if (gamepad.GetRawButton(5)) {
			driverDir = left;
		} else {
			driverDir = front;
		}

	}

	void pollSensors() {
		shooterEncoderRate = shooterEncoder->GetRate();
	}

	void sendDataToDriverStation() {
		frc::SmartDashboard::PutNumber("shooter encoder rate",
				shooterEncoderRate);
		frc::SmartDashboard::PutNumber("forward backward", forwardBackward);
		frc::SmartDashboard::PutNumber("strafe", strafe);
		frc::SmartDashboard::PutNumber("turn ", turn);
		frc::SmartDashboard::PutNumber("shooterLevel", shooterLevel);
		frc::SmartDashboard::PutBoolean("driveLevelFull", driveLevel == full);
		frc::SmartDashboard::PutBoolean("driveLevelNormal",
				driveLevel == normal);
		frc::SmartDashboard::PutBoolean("driveLevelSlow", driveLevel == slow);
		frc::SmartDashboard::PutNumber("shooter pos", shooter.GetEncPosition());
		frc::SmartDashboard::PutBoolean("shooter vel", shooter.GetEncVel());

		frc::SmartDashboard::PutNumber("visionChangeX", visionChangeX);
		frc::SmartDashboard::PutNumber("visionChangeY", visionChangeY);
		//frc::SmartDashboard::PutNumber("VR", table->GetNumber("cx"));
	}

	bool pollVision() {
		if (frc::SmartDashboard::GetNumber("pollVision", 1) == 0) {
			visionChangeX = frc::SmartDashboard::GetNumber("cX", 0);
			visionChangeY = frc::SmartDashboard::GetNumber("cY", 0);
			visionError = (frc::SmartDashboard::GetNumber("vrerror", 0) == 1);
			frc::SmartDashboard::PutNumber("pollVision", 1);
			return true;
		}
		return false;
	}

	/*
	 * This autonomous (along with the chooser code above) shows how to select
	 * between different autonomous modes using the dashboard. The sendable
	 * chooser code works with the Java SmartDashboard. If you prefer the
	 * LabVIEW Dashboard, remove all of the chooser code and uncomment the
	 * GetString line to get the auto name from the text box below the Gyro.
	 *
	 * You can add additional auto modes by adding additional comparisons to the
	 * if-else structure below with additional strings. If using the
	 * SendableChooser make sure to add them to the chooser code above as well.
	 */
	void AutonomousInit() override { //This method is called once each time the robot enters Autonomous
		//autoSelected = chooser.GetSelected();
		std::string str = SmartDashboard::GetString("Auto Selector", "default");
		//std::cout << "Auto selected: " << autoSelected << std::endl;

		//Reset values
		this->agitatorLevel = this->stop;
		this->shooterLevel = this->stop;
		this->driveX = 0;
		this->driveY = 0;
		this->driveZ = 0;
		this->driveGyro = 0;
		this->driveLevel = this->normal;
		this->driverDir = this->direction::front;

		// Parse/Detect auto mode
		if (str == "forward") {
			autoSelected = driveForward;
			frc::SmartDashboard::PutString("Auto Name",
					"Auto Dead reckoning (drive forward)");
		} else if (str == "vision") {
			autoSelected = vision;
			frc::SmartDashboard::PutString("Auto Name", "Auto Vision");
		} else if (str == "gearLeft") {
			autoSelected = gearLeft;
			frc::SmartDashboard::PutString("Auto Name", "Auto gearLeft");
		} else if (str == "none") {
			autoSelected = none;
		} else {
			//this->autoRecordPath = frc::SmartDashboard::GetString("Auto Record","");
			string data = "", path = "" + this->config_folder + str + ".txt";
			if (str.size() > 0) {
				if (toolbox::readFormatted(&path, &data)) {
					if (this->config_fleet.fromString(&data, true)) {
						autoSelected = custom;
						this->autoCustomLPos = 0;
						frc::SmartDashboard::PutNumber("auto amount",
								this->config_fleet.entries.size());
						frc::SmartDashboard::PutString("Auto Name",
								"Auto Custom (" + this->autoRecordPath + ")");
						frc::SmartDashboard::PutString("Auto Custom ERROR:",
								"");
					} else {
						this->config_fleet.entries.clear();
						frc::SmartDashboard::PutString("Auto Custom ERROR:",
								"unserilizing");
						autoSelected = none;
						frc::SmartDashboard::PutString("Auto Name",
								"Auto NONE (" + str + ")");
					}
				} else {
					frc::SmartDashboard::PutString("Auto Custom ERROR:",
							"file " + path);
					autoSelected = none;
					frc::SmartDashboard::PutString("Auto Name",
							"Auto NONE (" + str + ")");
				}

			} else {
				frc::SmartDashboard::PutString("Auto Custom ERROR:",
						"No Custom selected");
				autoSelected = none;
				frc::SmartDashboard::PutString("Auto Name",
						"Auto NONE (" + str + ")");
			}

		}

		//autoMode = 0;
		robotDrive.SetExpiration(0.3);

		// Invert the left side motors
		robotDrive.SetInvertedMotor(RobotDrive::kFrontLeftMotor, true);

		// You may need to change or remove this to match your robot
		robotDrive.SetInvertedMotor(RobotDrive::kRearLeftMotor, true);
		timer1.Reset();
		timer1.Start();
		this->timestamp = 0;
		this->setAutoMode(0);
	}

	void setAutoMode(int mode) {
		this->autoSubMode = mode;
		frc::SmartDashboard::PutNumber("Auto Sub Mode", this->autoSubMode);
	}

	void run_door() {
		if (doorDrop[1]) {
			drop->Set(70);
		} else {
			drop->Set(0);
		}
	}

	void DisabledInit() {
		if (this->autoRecord) {
			string data = "";
			frc::SmartDashboard::PutBoolean("Auto Record to String",
					this->config_fleet.toString(&data));
			string path = this->config_folder + this->autoRecordPath + ".txt";
			frc::SmartDashboard::PutBoolean("Auto Record to file",
					toolbox::formattedWrite(&path, &data));
			this->autoRecord = false;
		}
		//frc::SmartDashboard::PutString("DisabledPeriodic", frc::SmartDashboard::GetString("Auto Record", "NULL"));
	}

	int autoVisionHelper_getStrafe() {
		if (visionChangeX < 310 - visionXMarginOfError) {
			frc::SmartDashboard::PutString("Auto vision strafe", "left");
			return left;
		} else if (visionChangeX > 310 + visionXMarginOfError) {
			frc::SmartDashboard::PutString("Auto vision strafe", "right");
			return right;
		} else {
			frc::SmartDashboard::PutString("Auto vision strafe", "center");

			frc::SmartDashboard::PutBoolean("Auto Found Target",
			true);
			return back; // Center of target
		}
	}

	int autoVisionHelper_notAtTarget() {
		if (visionChangeY > 10) {
			frc::SmartDashboard::PutBoolean("Auto Vision Found Target",
			false);
			return true;
		} else {
			frc::SmartDashboard::PutBoolean("Auto Vision Found Target",
			true);
			return false;
		}
	}

	int autoVisionHelper_getTargetDistance() {
		if (visionChangeY > close) {
			frc::SmartDashboard::PutString("Auto target distance", "far");
			return far;
		} else if (visionChangeY > hit) {
			frc::SmartDashboard::PutString("Auto target distance", "close");
			return close;
		} else {
			frc::SmartDashboard::PutString("Auto target distance", "hit");
			return hit;
		}
	}

	int leftCenterRight = 0;bool notAtTarget = true;
	void autoVision() {
		driveLevel = slow;
		switch (autoSubMode) {
		case (0):
			setAutoMode(1);
			break;
			if (timer1.Get() >= 1) { // until one second has passed
				updateDrive(0, 0, 0, 0); // stop driving
				setAutoMode(1); // change the automode
				timer1.Reset();
			} else {
				updateDrive(0, -0.4, 0, 0); //drive forward at half speed
			}
			break;
		case (1):
			if (pollVision()) {
				double temp = timer1.Get();
				double x = 0;
				double y = 0;
				double z = 0;
				if (visionError) {
					if (autoVisionHelper_getTargetDistance() == close) {
						setAutoMode(2);
						break;
					}
					if (notAtTarget) { // FIX THIS
						switch (leftCenterRight) {
						case (-1): //left
							updateDrive(1, 0, 0, 0);
							break;
						case (1): //right
							updateDrive(-1, 0, 0, 0);
							break;
						}
					} else {
						updateDrive(0, 0, 0);
					}
				} else {
					notAtTarget = autoVisionHelper_notAtTarget();
					switch (autoVisionHelper_getStrafe()) {
					case (left):
						x = -1;
						leftCenterRight = -1;
						break;
					case (right):
						x = 1;
						leftCenterRight = 1;
						break;
					default:
						x = 0;
						leftCenterRight = 0;
						break;
					}
					switch (autoVisionHelper_getTargetDistance()) {
					case (far):
						notAtTarget = true;
						y = -0.6;
						break;
					case (close):
						notAtTarget = true;
						y = -0.6;
						break;
					default:
						notAtTarget = false;
						y = 0;
						break;
					}
					if (x != 0) {
						y = 0;
					}
					if (x == 0 && y == 0) {
						setAutoMode(6);
						frc::SmartDashboard::PutString("Auto status",
								"Stopped; Hit Gear");
					}
					updateDrive(x, y, z);
				}
				frc::SmartDashboard::PutNumber("visionSpeed", temp);
				timer1.Reset();
			}
			break;
		case (2):
			timer1.Reset();
			updateDrive(0, -0.6, 0);
			setAutoMode(3);
			break;
		case (3):
			if (timer1.Get() > 2) {
				updateDrive(0, 0, 0);
				setAutoMode(4);
			}
			break;
		default:
			break;
		}
	}

	void autoGearLeft() {
		driveLevel = full;
		switch (this->autoSubMode) {
		case (0): // forward
			updateDrive(0, -0.40, 0, 0);
			setAutoMode(1);
			break;
		case (1):
			if (this->autoTime > 2) { // stop and turn right
				updateDrive(0, 0, .3, 0);
				setAutoMode(2);
			}
			break;
		case (2):
			if (this->autoTime > 4) { // forward
				updateDrive(0, -4.4, 0, 0);
				setAutoMode(3);
			}
			break;
		case(3):
			if (this->autoTime > 6) { // stop
				updateDrive(0, 0, 0, 0);
				setAutoMode(4);
			}
			break;
		case(4):break;
		}
	}

	void autoDriveForward() {
		driveLevel = full;
		switch (autoSubMode) {
		default:
			updateDrive(0, -0.40, 0, 0);
			setAutoMode(1);
			break;
		}
	}
	/*
	 * @brief get balls and shoot
	 */
	void autoDriveAndShoot() {
		/*
		 * drive forward 50% for 7.75 sec
		 * strafe right 60% for 3 sec
		 * strafe left 60% for .75 sec
		 * reverse 50% for 3.75 sec
		 * reverse 30% for 6 sec
		 * turn right 30% for .75 sec
		 * stop
		 * agitate 100%, leave on
		 * wait 1 sec
		 * shoot until end of auto
		 */
		switch (autoSubMode) {
		case (0): // drive forward
			updateDrive(0, 0.5, 0, 0);
			if (timer1.Get() > 7.75) {
				setAutoMode(autoSubMode + 1);
				timer1.Reset();
			}
			break;
		case (1): // strafe right
			updateDrive(0.6, 0, 0, 0);
			if (timer1.Get() > 3) {
				setAutoMode(autoSubMode + 1);
				timer1.Reset();
			}
			break;
		case (2): // strafe left
			updateDrive(-0.6, 0, 0, 0);
			if (timer1.Get() > 0.75) {
				setAutoMode(autoSubMode + 1);
				timer1.Reset();
			}
			break;
		case (3): // reverse
			updateDrive(0, -0.5, 0, 0);
			if (timer1.Get() > 3.75) {
				setAutoMode(autoSubMode + 1);
				timer1.Reset();
			}
			break;
		case (4): // turn right
			updateDrive(0, 0, 0.3, 0);
			if (timer1.Get() > 0.75) {
				setAutoMode(autoSubMode + 1);
				timer1.Reset();
			}
			break;
		case (5): // run agitator, leave on
			updateDrive(0, 0, 0, 0);
			agitatorLevel = normal;
			run_agitator();
			if (timer1.Get() > 1) {
				setAutoMode(autoSubMode + 1);
				timer1.Reset();
			}
			break;
		case (6): // shooter
			shooterLevel = full;
			run_agitator();
			run_shooter();
			break;
		default:
			break;
		}
	}

	void autoCustom() {
		if (this->autoCustomLPos >= this->config_fleet.entries.size()) {
			return;
		}
		for (unsigned int x = this->autoCustomLPos;
				x < this->config_fleet.entries.size() /*Need time thing*/;
				x++) {
			dataBoat* b = this->config_fleet.entries[x].data;
			if (b != NULL && this->autoTime >= b->time) {
				this->autoCustomLPos = x;
				if (b->className == this->config_x.className) {
					this->driveX = ((actions::driveX*) b)->x;
				} else if (b->className == this->config_y.className) {
					driveY = ((actions::driveY*) b)->y;
				} else if (b->className == this->config_z.className) {
					driveZ = ((actions::driveZ*) b)->z;
				} else if (b->className == this->config_gyro.className) {
					driveGyro = ((actions::gyro*) b)->value;
				} else if (b->className == this->config_gear.className) {
					actions::driveGear* o = (actions::driveGear*) b;
					switch (o->gear) {
					case (actions::driveGear::slow):
						driveLevel = this->slow;
						break;
					case (actions::driveGear::full):
						driveLevel = this->full;
						break;
					default:
						driveLevel = this->normal;
						break;
					}
				} else if (b->className == this->config_shoot.className) {
					actions::shoot* o = (actions::shoot*) b;
					switch (o->gear) {
					case (actions::shoot::slow):
						shooterLevel = this->slow;
						break;
					case (actions::shoot::normal):
						shooterLevel = this->normal;
						break;
					case (actions::shoot::full):
						shooterLevel = this->full;
						break;
					default:
						shooterLevel = this->stop;
						break;
					}
				} else if (b->className == this->config_agitator.className) {
					actions::agitator* o = (actions::agitator*) b;
					if (o->onOff) {
						this->agitatorLevel = this->speedLevels::normal;
					} else {
						this->agitatorLevel = this->speedLevels::stop;
					}
				}
				this->config_fleet.entries[x].clear();
			}
		}
		frc::SmartDashboard::PutNumber("auto Custom L pos",this->autoCustomLPos);
		frc::SmartDashboard::PutNumber("Auto Custom L time",this->autoTime);
	}

	void AutonomousPeriodic() { // TODO: FIX THIS
		/* This method is called each time the robot receives a
		 * packet instructing the robot to be in Autonomous Enabled mode (approximately every 20ms)
		 * This means that code in this method should return in 20 ms or less (no delays or loops)
		 */

		if (this->timestamp == 0) {
			//Setup timers for Auto
			this->timestamp = clock();
		}
		this->autoTime = ((clock()-this->timestamp)/(double)CLOCKS_PER_SEC);
		frc::SmartDashboard::PutNumber("auto times", this->autoTime);
		string err;
		//autoVision();
		switch (autoSelected) {
		case (driveForward):
			autoDriveForward();
		err = "";
			break;
		case (vision):
			autoVision();
		err = "";
			break;
		case (custom):
			autoCustom();
		err = "";
			break;
		case(gearLeft):
			autoGearLeft();
		err = "";
			break;
		default:
			driveX = 0;
			driveY = 0;
			driveZ = 0;
			driveGyro = 0;
			err = "Unknown value";
			break;
		}
		frc::SmartDashboard::PutString("AutonomousPeriodic-> autoSelected: ERROR", err);
		updateDrive(0, 0, 0, 0, false); // Just send previouse signals to motors to prevent watchdog.
		run_agitator();
		run_shooter();
		//frc::SmartDashboard::PutNumber("NUM", ++xcount);
		sendDataToDriverStation();
		//frc::SmartDashboard::PutString("!!!",frc::SmartDashboard::GetString("Auto Selector", "-1"));
	}

	void TeleopInit() {
		/* The TeleopInit method is called once each time the robot enters teleoperated mode
		 *  This is where you would put presets for the teleoperated mode.
		 */
		robotDrive.SetExpiration(0.3);

		// Invert the left side motors
		robotDrive.SetInvertedMotor(RobotDrive::kFrontLeftMotor, true);

		// You may need to change or remove this to match your robot
		robotDrive.SetInvertedMotor(RobotDrive::kRearLeftMotor, true);
		setAutoMode(0);
	}

	void TeleopPeriodic() {
		/* The TeleopPeriodic method is called each time the robot recieves a packet instructing it
		 * to be in teleoperated mode
		 */
		pollVision();
		pollControllers();
		pollSensors();
		switch (driverDir) {
		case (left):
			drive(-forwardBackward, strafe, turn, 0);
			break;
		default:
			drive(strafe, forwardBackward, turn, 0);
			break;
		}
		run_shooter();
		run_agitator();
		run_winch();
		run_door();
		sendDataToDriverStation();
		getShooterConfig();
	}

	void TestInit() {
		// Invert the left side motors
		robotDrive.SetInvertedMotor(RobotDrive::kFrontLeftMotor, true);

		// You may need to change or remove this to match your robot
		robotDrive.SetInvertedMotor(RobotDrive::kRearLeftMotor, true);

		//TimeCh1.Reset();
		//TimeCh1.Start();
		this->autoRecordPath = frc::SmartDashboard::GetString("Auto Record",
				"");
		this->autoRecord = (this->autoRecordPath.size() > 0);

		//Reset values
		this->config_fleet.entries.clear();
		this->last_driveX = 0;
		this->last_driveY = 0;
		this->last_driveZ = 0;
		this->last_driveGyro = 0;
		this->agitatorLevel = this->speedLevels::stop;
		this->shooterLevel = this->speedLevels::stop;
		this->timestamp = 0;
	}

	void TestPeriodic() {
		if (this->timestamp == 0) {
			//Setup timers for Auto
			this->timestamp = clock();
		}

		// only runs if robot is in test mode
		//lw->Run();
		// get encoder values and send them to the dashboard
		if (this->autoRecord) {		//Record drive, for auto mode
			pollControllers();
			pollSensors();
			switch (driverDir) {
			case (left):
				drive(-forwardBackward, strafe, turn, 0);
				break;
			default:
				drive(strafe, forwardBackward, turn, 0);
				break;
			}
			run_shooter();
			run_agitator();
			run_winch();
			run_door();
			getShooterConfig();

			u_int32_t id;
			this->autoTime = ((clock()-this->timestamp)/(double)CLOCKS_PER_SEC);
			frc::SmartDashboard::PutNumber("time", this->autoTime);

			if (true) { // NEEDS TESTING
				if (abs(abs(this->last_driveX) - abs(this->forwardBackward))
						> 0.01) {
					this->last_driveX = this->forwardBackward;
					this->config_x.time = this->autoTime;
					this->config_x.x = this->last_driveX;
					this->config_fleet.addEntry(&this->config_x, &id);
				}
				if (abs(abs(this->last_driveY) - abs(this->strafe)) > 0.01) {
					this->last_driveY = this->strafe;
					this->config_y.time = this->autoTime;
					this->config_y.y = this->last_driveY;
					this->config_fleet.addEntry(&this->config_y, &id);
				}
				if (abs(abs(this->last_driveZ) - abs(this->turn)) > 0.01) {
					this->last_driveZ = this->turn;
					this->config_z.time = this->autoTime;
					this->config_z.z = this->last_driveZ;
					this->config_fleet.addEntry(&this->config_z, &id);
				}
				if (abs(abs(this->last_driveGyro) - abs(this->driveGyro))
						> 0.01) {
					this->last_driveGyro = this->driveGyro;
					this->config_gyro.time = this->autoTime;
					this->config_gyro.value = this->last_driveGyro;
					this->config_fleet.addEntry(&this->config_gyro, &id);
				}
				if (this->last_shooterLevel != this->shooterLevel) {
					this->last_shooterLevel = this->shooterLevel;
					this->config_shoot.time = this->autoTime;
					switch (this->shooterLevel) {
					case (this->speedLevels::slow):
						this->config_shoot.gear = this->config_shoot.slow;
						break;
					case (this->speedLevels::normal):
						this->config_shoot.gear = this->config_shoot.normal;
						break;
					case (this->speedLevels::full):
						this->config_shoot.gear = this->config_shoot.full;
						break;
					default:
						this->config_shoot.gear = this->config_shoot.off;
						break;
					}
					this->config_fleet.addEntry(&this->config_shoot, &id);

				}
				if (this->last_driveLevel != this->driveLevel) {
					this->last_driveLevel = this->driveLevel;
					this->config_gear.time = this->autoTime;
					switch (this->driveLevel) {
					case (this->speedLevels::slow):
						this->config_gear.gear = this->config_gear.slow;
						break;
					case (this->speedLevels::full):
						this->config_gear.gear = this->config_gear.full;
						break;
					default:
						this->config_gear.gear = this->config_gear.normal;
						break;
					}
					this->config_fleet.addEntry(&this->config_gear, &id);
				}
				if (this->last_agitatorLevel != this->agitatorLevel) {
					this->last_agitatorLevel = this->agitatorLevel;
					this->config_agitator.time = this->autoTime;
					this->config_agitator.onOff = (this->last_agitatorLevel
							== this->normal);
					this->config_fleet.addEntry(&this->config_agitator, &id);
				}
			} else { // WORKS

				this->config_x.time = this->autoTime;
				frc::SmartDashboard::PutNumber("auto x", this->config_x.time);
				this->config_y.time = this->autoTime;
				this->config_z.time = this->autoTime;
				this->config_gyro.time = this->autoTime;
				this->config_gear.time = this->autoTime;
				this->config_shoot.time = this->autoTime;
				this->config_agitator.time = this->autoTime;

				this->config_x.x = strafe;
				this->config_y.y = forwardBackward;
				this->config_z.z = turn;
				this->config_gyro.value = 0;
				this->config_agitator.onOff = (this->agitatorLevel
						== this->speedLevels::normal);
				switch (this->driveLevel) {
				case (slow):
					this->config_gear.gear = actions::driveGear::slow;
					break;
				case (full):
					this->config_gear.gear = actions::driveGear::full;
					break;
				default:
					this->config_gear.gear = actions::driveGear::normal;
					break;
				}

				switch (this->shooterLevel) {
				case (slow):
					this->config_shoot.gear = actions::shoot::slow;
					break;
				case (normal):
					this->config_shoot.gear = actions::shoot::normal;
					break;
				case (full):
					this->config_shoot.gear = actions::shoot::full;
					break;
				default:
					this->config_shoot.gear = actions::shoot::off;
					break;
				}

				this->config_fleet.addEntry(&this->config_x, &id);
				this->config_fleet.addEntry(&this->config_y, &id);
				this->config_fleet.addEntry(&this->config_z, &id);
				this->config_fleet.addEntry(&this->config_gyro, &id);
				this->config_fleet.addEntry(&this->config_gear, &id);
				this->config_fleet.addEntry(&this->config_shoot, &id);
				this->config_fleet.addEntry(&this->config_agitator, &id);
			}

			sendDataToDriverStation();
		} else { // just vision polling
			pollVision();
			frc::SmartDashboard::PutNumber("visionChangeX", visionChangeX);
		}
	}

	void RobotInit() {
		//chooser.AddDefault(autoNameDefault, autoNameDefault);
		//chooser.AddObject(autoNameCustom, autoNameCustom);
		frc::SmartDashboard::PutData("Auto Modes", &chooser);
		//CameraServer::GetInstance()->StartAutomaticCapture();

		//Declare Varibles
		winch = new TalonSRX(4);
		agitator = new TalonSRX(5);
		drop = new frc::Servo(6);

		//Init Varibles

		shooterEncoder = new Encoder(shooterEncoderChannelA,
				shooterEncoderChannelB,
				false, Encoder::EncodingType::k4X);
		shooter.SetControlMode(
				frc::CANSpeedController::ControlMode::kPercentVbus);
		shooterEncoder->SetMaxPeriod(.1);
		shooterEncoder->SetMinRate(10);
		shooterEncoder->SetDistancePerPulse(.0001);
		shooterEncoder->SetReverseDirection(true);
		shooterEncoder->SetSamplesToAverage(7);
		table = NetworkTable::GetTable("SmartDashboard");

		shooter.SetFeedbackDevice(
				CANTalon::FeedbackDevice::CtreMagEncoder_Relative);
		shooter.ConfigNominalOutputVoltage(+0.0f, -0.0f);
		shooter.ConfigPeakOutputVoltage(+12.0f, -12.0f);
		shooter.SetEncPosition(0);
		NetworkTable::GlobalDeleteAll();
		sendDataToDriverStation();
		{
			std::ifstream file(pathShooterFull);
			std::string temp;
			getline(file, temp);
			file.close();
			shooterFull = atoi(temp.c_str());
		}
		{
			std::ifstream file(pathShooterNormal);
			std::string temp;
			getline(file, temp);
			file.close();
			shooterNormal = atoi(temp.c_str());
		}
		{
			std::ifstream file(pathShooterSlow);
			std::string temp;
			getline(file, temp);
			file.close();
			shooterSlow = atoi(temp.c_str());
		}
		frc::SmartDashboard::PutNumber("shooterFull", shooterFull);
		frc::SmartDashboard::PutNumber("shooterNormal", shooterNormal);
		frc::SmartDashboard::PutNumber("shooterSlow", shooterSlow);
		frc::SmartDashboard::PutNumber("pollVision", 1);
		frc::SmartDashboard::PutString("Auto vision strafe", "init");
		frc::SmartDashboard::PutString("Auto target distance", "init");
		frc::SmartDashboard::PutString("Auto Name", "init");
		frc::SmartDashboard::PutNumber("Auto vision margin",
				visionXMarginOfError);
		//frc::SmartDashboard::PutNumber("!-!",frc::SmartDashboard::GetNumber("Shooter full",-1)); //TODO: FIX THIS

		//Configure auto recording feature
		this->config_fleet = fleet();
		this->config_x = actions::driveX();
		this->config_y = actions::driveY();
		this->config_z = actions::driveZ();
		this->config_gyro = actions::gyro();
		this->config_gear = actions::driveGear();
		this->config_shoot = actions::shoot();
		this->config_agitator = actions::agitator();

		u_int32_t id;
		this->config_fleet.addTemplate(&this->config_x, &id);
		this->config_fleet.addTemplate(&this->config_y, &id);
		this->config_fleet.addTemplate(&this->config_z, &id);
		this->config_fleet.addTemplate(&this->config_gyro, &id);
		this->config_fleet.addTemplate(&this->config_gear, &id);
		this->config_fleet.addTemplate(&this->config_shoot, &id);
		this->config_fleet.addTemplate(&this->config_agitator, &id);
	}

private:
	frc::LiveWindow* lw = LiveWindow::GetInstance(); // this is for testing
	frc::SendableChooser<std::string> chooser;
	enum autoModes {
		none = -1, driveForward = 0, vision = 1, custom = 2, gearLeft = 3
	};
	autoModes autoSelected;

// Channels for the wheels
	static constexpr int kFrontLeftChannel = 0;
	static constexpr int kRearLeftChannel = 2;
	static constexpr int kFrontRightChannel = 1;
	static constexpr int kRearRightChannel = 3;
	/*
	 static constexpr int kJoystickChannel = 2;
	 static constexpr int kJoystickChannel2 = 1;
	 static constexpr int kXboxChannel = 0;
	 */

// Robot drive system
	frc::RobotDrive robotDrive { kFrontLeftChannel, kRearLeftChannel,
			kFrontRightChannel, kRearRightChannel };

	/********************************************************
	 * Joystick setup
	 * NOTE: this code is not needed atm. Uncomment (and comment
	 * the xbox code) for use. Do not leave both sections uncommented.
	 *
	 ********************************************************/
//drive vars
	double forwardBackward, last_forwardBackward;
	double turn, last_turn;
	double strafe, last_strafe;

	frc::Joystick gamepad { 0 };
	frc::Joystick panel { 1 };

	Encoder *shooterEncoder;
	static const int shooterEncoderChannelA = 0;
	static const int shooterEncoderChannelB = 1;
	double shooterEncoderRate; // encoder rate value, is updated ONLY in pollSensors, and ONLY sent to dashboard in sendDataToDriverStation.
	TalonSRX *agitator;
	TalonSRX *winch;

	CANTalon shooter { 1 }; // id of the device is zero

	Servo* drop;

	double driveSlow = 0.3, driveNormal = 0.6, driveFull = 1;
	double strafeSlow = 0.45, strafeNormal = 0.8, strafeFull = driveFull;
	double turnSlow = 0.25, turnNormal = driveNormal, turnFull = driveFull;

	enum speedLevels {
		reverse = 0, stop = 1, slow = 2, normal = 3, full = 4
	};
	speedLevels last_driveLevel = normal, last_shooterLevel = normal;
	speedLevels driveLevel = normal, shooterLevel = normal;

//shooter speeds
	double shooterSlow = 2000, shooterNormal = 4000, shooterFull = 8000;

//Gamepad
	bool btn_driveToggle = false, btn_shooterToggle = false;

	speedLevels last_agitatorLevel = stop;
	speedLevels agitatorLevel = stop;
	double agitatorNormal = 1, agitatorReverse = -1;

	speedLevels winchLevel = stop;
	double winchNormal = 1, winchReverse = -1;

// Panel
	bool runWinch = false;

	std::shared_ptr<NetworkTable> table;

	std::string pathShooterFull = "/home/lvuser/shooterFull.txt",
			pathShooterNormal = "/home/lvuser/shooterNormal.txt",
			pathShooterSlow = "/home/lvuser/shooterSlow.txt";

	int autoSubMode = 0;

	double driveX, driveY, driveZ, driveGyro;
	double last_driveX, last_driveY, last_driveZ, last_driveGyro;
	int visionChangeX = 0, visionChangeY = 0, visionXMarginOfError = 30;bool visionError =
	true;

	bool doorDrop[2] = { false, false };

	enum direction {
		front = 0, right = 90, back = 180, left = 270
	};

	direction driverDir = front;

	enum autoVision_YTargetDistance {
		hit = 10, close = 100, far = 500
	};

	Timer timer1;
};

START_ROBOT_CLASS(Robot);

/* IMPORTANT NOTES
 * If the build fails, try re-indexing (re-building) the code.
 * That will be an issue when pulling updates from the repo.
 *
 *
 * WHEN COMMITING CODE, CLEAN PROJECT FIRST
 *
 *GITHUB IMPORTANT!
 * Index files you want to sync (one time) (Right click file -> Team -> Add to Index) (Mostly for new files)
 *
 * !!-Steps to Git Push from eclipse-!!
 * Commit (Right click project -> Team -> Commit) or (Ctr + Shift + 3(#)) Make sure to select file to commit(staging) !! DO NOT SELECT "COMMIT AND PUSH"
 * To Push (Right click project -> Team -> Push Branch master -> Make sure to check "Force overwrite of branch on remote if it exists and has diverged" -> Then continue with submission.
 *
 * */

/*
 * Controls
 * 	Drive System:
 * 	 forward/backward  -> gamepad joystick 1 Y-axis
 * 	 strafe left/right -> gamepad joystick 1 X-axis
 * 	 turn left/right   -> gamepad joystick 2 X-axis
 * 	 stop -> no above input
 * 	 ~Note: there is deadzones on joysticks
 *
 * 	Drive direction:
 * 	 left -> (when pressed) gamepad btn 5 (left top bumper)
 * 	 front -> when no above input
 *
 *  Shooter speed: (when pressed)
 *   low    -> panel btn 1 (label: HI)
 *   normal -> panel btn 2 (label: mid) or gamepad btn 1
 *   high   -> panel btn 3 (label: dwn)
 *   reverse -> panel btn 7
 *   stop   -> no above input
 *
 *  Drive 'gears': (toggle)
 *   low    (toggle) -> gamepad btn 7 (left bumper)
 *   normal (toggle) -> gamepad btn 8 (right bumper)
 *   high   (toggle) -> gamepad btn 6 (right bumper)
 *
 *  Winch:
 *   Forward: (pressed) -> panel btn 4
 *   Stop: No above button pressed
 *
 *  Agitator:
 *   Forward: (pressed) -> panel btn 6 or gamepad btn 2
 *   Reverse: (pressed) -> panel btn 5
 *   Stop: no above button pressed
 *
 *  Drop: (one time toggle, default state false/up) ->
 *
 *	!-COMPLETED-!
 *  Drive speeds done (gamepad)
 *  Shooter speeds done. (panel)
 *  Shooter encoder done.
 */
