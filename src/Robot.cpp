#include <iostream>
#include <memory>
#include <string>

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

class Robot: public frc::IterativeRobot {
public:
	void drive(double x, double y, double z, double gyro = 0) {
		if (driveOverride) { //Override
			x = (x / 1) * driveSlow;
			y = (y / 1) * driveSlow;
			z = (z / 1) * driveSlow;
		} else {
			switch (driveLevel) {
			case (full):
				break;
			default:
				x = (x / 1) * driveNormal;
				y = (y / 1) * driveNormal;
				z = (z / 1) * driveNormal;
				break;
			}
		}
		robotDrive.MecanumDrive_Cartesian(-x, y, z, gyro);
		frc::SmartDashboard::PutNumber("x", x);
		frc::SmartDashboard::PutNumber("y", y);
		frc::SmartDashboard::PutNumber("z", z);
	}

	void run_shooter() {
		switch (shooterLevel) {
		case (slow):
			shooter->Set(shooterSlow);
			break;
		case (normal):
			shooter->Set(shooterNormal);
			break;
		case (full):
			shooter->Set(shooterFull);
			break;
		default:
			shooter->Set(0);
			break;
		}

	}

	void run_winch() {
		if (runWinch) {
			winch->Set(0.5);
		} else {
			winch->Set(0);
		}
	}

	void pollControllers() {
		//gamepad: Drive control polling
		double leftdead = .4;
		double rightdead = .4;
		if (gamepad.GetX() > leftdead || gamepad.GetX() < -leftdead) {
			strafe = gamepad.GetX();
		} else {
			strafe = gamepad.GetX();
		}
		if (gamepad.GetY() > leftdead || gamepad.GetY() < -leftdead) {
			forwardBackward = gamepad.GetY();
		} else {
			forwardBackward = gamepad.GetY();
		}
		if (gamepad.GetZ() > rightdead || gamepad.GetZ() < -rightdead) {
			turn = gamepad.GetZ();
		} else {
			turn = gamepad.GetZ();
		}

		if (gamepad.GetRawButton(6) != btn_driveToggle) {
			btn_driveToggle = !btn_driveToggle;
			if (btn_driveToggle == true) {
				if (driveLevel == full) {
					driveLevel = normal;
				} else {
					driveLevel = full;
				}
			}
		}
		if (gamepad.GetRawButton(5)) {
			driveOverride = true;
		} else {
			driveOverride = false;
		}

		//button panel: Shooter control polling
		bool bslow = panel.GetRawButton(1), bnormal = panel.GetRawButton(2),
				bfull = panel.GetRawButton(3);
		if (bslow == true && shooterLevel != slow) {
			shooterLevel = slow;
		} else if (bnormal == true && shooterLevel != normal) {
			shooterLevel = normal;
		} else if (bfull == true && shooterLevel != full) {
			shooterLevel = full;
		} else if (!bslow && !bnormal && !bfull && shooterLevel != stop) {
			shooterLevel = stop;
		}

		// winch control polling
		if (panel.GetRawButton(4)) {
			runWinch = true;
		} else {
			runWinch = false;
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
		frc::SmartDashboard::PutNumber("driveLevel", driveLevel);
		//frc::SmartDashboard::PutNumber("VR", table->GetNumber("cx"));
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
		autoSelected = chooser.GetSelected();
		//std::string autoSelected = SmartDashboard::GetString("Auto Selector", autoNameDefault);
		std::cout << "Auto selected: " << autoSelected << std::endl;

		if (autoSelected == autoNameCustom) {
			// Custom Auto goes here
		} else {
			// Default Auto goes here
		}
	}

	void AutonomousPeriodic() {
		/* This method is called each time the robot receives a
		 * packet instructing the robot to be in Autonomous Enabled mode (approximately every 20ms)
		 * This means that code in this method should return in 20 ms or less (no delays or loops)
		 */
		if (autoSelected == autoNameCustom) {
			// Custom Auto goes here
		} else {
			// Default Auto goes here
		}
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

	}

	void TeleopPeriodic() {
		/* The TeleopPeriodic method is called each time the robot recieves a packet instructing it
		 * to be in teleoperated mode
		 */
		uint8_t ticks = ++teleopTicks;
		if (ticks > 0) {
			pollControllers();
			pollSensors();
			drive(strafe, forwardBackward, turn, 0);
			run_shooter();
			run_winch();
			sendDataToDriverStation();
			ticks = 0;
		}
	}

	void TestPeriodic() { // only runs if robot is in test mode
		lw->Run();

		// get encoder values and send them to the dashboard
	}

	void RobotInit() {
		chooser.AddDefault(autoNameDefault, autoNameDefault);
		chooser.AddObject(autoNameCustom, autoNameCustom);
		frc::SmartDashboard::PutData("Auto Modes", &chooser);
		//CameraServer::GetInstance()->StartAutomaticCapture();

		//Declare Varibles
		shooter = new TalonSRX(9);
		agitator = new TalonSRX(7);
		winch = new TalonSRX(4);

		//Init Varibles

		shooterEncoder = new Encoder(shooterEncoderChannelA,
				shooterEncoderChannelB, false, Encoder::EncodingType::k4X);
		shooterEncoder->SetMaxPeriod(.1);
		shooterEncoder->SetMinRate(10);
		shooterEncoder->SetDistancePerPulse(.001);
		shooterEncoder->SetReverseDirection(true);
		shooterEncoder->SetSamplesToAverage(7);
		table = NetworkTable::GetTable("SmartDashboard");
		NetworkTable::GlobalDeleteAll();
		table->PutBoolean("I AM ALIVE",true);
	}

private:
	frc::LiveWindow* lw = LiveWindow::GetInstance(); // this is for testing
	frc::SendableChooser<std::string> chooser;
	const std::string autoNameDefault = "auto 1";
	const std::string autoNameCustom = "auto 2";
	std::string autoSelected;

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
	double forwardBackward;
	double turn;
	double strafe;bool shoot = false;

	frc::Joystick gamepad { 0 };
	frc::Joystick panel { 1 };

	Encoder *shooterEncoder;
	static const int shooterEncoderChannelA = 0;
	static const int shooterEncoderChannelB = 1;
	double shooterEncoderRate; // encoder rate value, is updated ONLY in pollSensors, and ONLY sent to dashboard in sendDataToDriverStation.

	TalonSRX *shooter;
	TalonSRX *agitator;
	TalonSRX *winch;

	double driveSlow = .3, driveNormal = .6, driveFull = 1;

	enum speedLevels {
		stop, slow, normal, full
	};
	speedLevels driveLevel = normal, shooterLevel = normal;

	//shooter speeds
	uint8_t shooterSlow = .3, shooterNormal = .5, shooterFull = 1;

	//Gamepad
	bool btn_driveToggle = false, btn_shooterToggle = false, driveOverride =
			false;
	uint8_t teleopTicks;

	// Panel
	bool runWinch = false;

	std::shared_ptr<NetworkTable> table;
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
 * Layout: (No joystick only game pads)
 * Controler: Game Pad
 *  Forward/Backward  -> left stick verticle
 *  Strafe Left/Right -> Left stick horazontal
 *  Turn left/Right   -> Right stick verticle
 *
 * Controls
 *  Shooter speed: (when pressed)
 *   stop   -> no below input
 *   low    -> panel btn 1 (label: up)
 *   normal -> panel btn 2 (label: mid)
 *   high   -> panel btn 3 (label: dwn)
 *  Drive speed: (when pressed)
 *   low             -> gamepad btn 5 (left bumper)
 *   normal (toggle) -> gamepad btn 6 (right bumper)
 *   high   (toggle) -> gamepad btn 6 (right bumper)
 *  Winch: (when pressed) -> panel btn 4
 *  Drop: (one time toggle, default state false/up) ->
 *
 *	!-COMPLETED-!
 *  Drive speeds done (gamepad)
 *  Shooter speeds done. (panel)
 */
