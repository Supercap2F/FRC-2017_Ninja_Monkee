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

class Robot: public frc::IterativeRobot {
public:
	/* x = strafe
	 * y = forward/backward
	 * z = turn
	 * gyro = gyro
	*/
	void drive(double x, double y, double z, double gyro = 0) {
		switch (driveLevel) {
		case (full):
			break;
		case (slow):
			x = (x / 1) * driveSlow;
			y = (y / 1) * driveSlow;
			z = (z / 1) * driveSlow;
			break;
		case (normal):
			x = (x / 1) * driveNormal;
			y = (y / 1) * driveNormal;
			z = (z / 1) * driveNormal;
			break;
		default:
			x=0;
			y=0;
			z=0;
		}
		robotDrive.MecanumDrive_Cartesian(x, y, z, gyro);
		frc::SmartDashboard::PutNumber("x", x);
		frc::SmartDashboard::PutNumber("y", y);
		frc::SmartDashboard::PutNumber("z", z);
	}

	void run_agitator() {
		if (runAgitator) {
			agitator->Set(1);
		} else {
			agitator->Set(0);
		}
	}

	void run_shooter() {
		double shooterSpeed = 0;
		switch (shooterLevel) {
		case (slow):
			shooterSpeed = shooterSlow;
			break;
		case (normal):
			shooterSpeed = shooterNormal;
			break;
		case (full):
			shooterSpeed = shooterFull;
			break;
		default:
			shooterSpeed = 0;
			break;
		}
		//frc::SmartDashboard::PutNumber("shooter speed", shooterSpeed);
		shooter.Set(shooterSpeed);
	}

	void run_winch() {
		if (runWinch) {
			winch->Set(1);
		} else {
			winch->Set(0);
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
		if (gamepad.GetX() > leftdead || gamepad.GetX() < -leftdead) {
			strafe = gamepad.GetX();
		} else {
			strafe = 0;
		}
		if (gamepad.GetY() > leftdead || gamepad.GetY() < -leftdead) {
			forwardBackward = gamepad.GetY();
		} else {
			forwardBackward = 0;
		}
		if (gamepad.GetZ() > rightdead || gamepad.GetZ() < -rightdead) {
			turn = gamepad.GetZ();
		} else {
			turn = 0;
		}
		if (gamepad.GetRawButton(7)) { // Drive override
			if(driveLevel != slow){
				driveLevel = slow;
			}
		} else {
			if (gamepad.GetRawButton(6) && driveLevel != full) {
				driveLevel = full;
			} else if (gamepad.GetRawButton(8) && driveLevel != normal) {
				driveLevel = normal;
			}
		}
		/*
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
		 */

		//button panel: Shooter control polling
		bool bslow = panel.GetRawButton(3), bnormal = panel.GetRawButton(2),
				bfull = panel.GetRawButton(1);
		if (bslow == true && shooterLevel != slow) {
			shooterLevel = slow;
		} else if (bnormal == true && shooterLevel != normal) {
			shooterLevel = normal;
		} else if (bfull == true && shooterLevel != full) {
			shooterLevel = full;
		} else if (!bslow && !bnormal && !bfull && shooterLevel != stop) {
			shooterLevel = stop;
		}

		// agitator control polling
		runAgitator = panel.GetRawButton(6);
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
		frc::SmartDashboard::PutBoolean("driveLevelNormal", driveLevel == normal);
		frc::SmartDashboard::PutBoolean("driveLevelSlow", driveLevel == slow);
		frc::SmartDashboard::PutBoolean("Button 0", driveLevel == slow);
		frc::SmartDashboard::PutNumber("shooter pos", shooter.GetEncPosition());
		frc::SmartDashboard::PutBoolean("shooter vel", shooter.GetEncVel());
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
			getShooterConfig();
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
		agitator = new TalonSRX(5);
		winch = new TalonSRX(4);

		//Init Varibles

		shooterEncoder = new Encoder(shooterEncoderChannelA,
				shooterEncoderChannelB, false, Encoder::EncodingType::k4X);
		shooter.SetControlMode(frc::CANSpeedController::ControlMode::kSpeed);
		shooterEncoder->SetMaxPeriod(.1);
		shooterEncoder->SetMinRate(10);
		shooterEncoder->SetDistancePerPulse(.0001);
		shooterEncoder->SetReverseDirection(true);
		shooterEncoder->SetSamplesToAverage(7);
		table = NetworkTable::GetTable("SmartDashboard");
		NetworkTable::GlobalDeleteAll();

		shooter.SetFeedbackDevice(
				CANTalon::FeedbackDevice::CtreMagEncoder_Relative);
		shooter.ConfigNominalOutputVoltage(+0.0f, -0.0f);
		shooter.ConfigPeakOutputVoltage(+12.0f, -12.0f);
		shooter.SetEncPosition(0);
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
		//frc::SmartDashboard::PutNumber("!-!",frc::SmartDashboard::GetNumber("Shooter full",-1)); //TODO: FIX THIS
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
	TalonSRX *agitator;
	TalonSRX *winch;

	CANTalon shooter { 1 }; // id of the device is zero

	double driveSlow = 0.3, driveNormal = 0.6, driveFull = 1;

	enum speedLevels {
		stop, slow, normal, full
	};
	speedLevels driveLevel = normal, shooterLevel = normal;

	//shooter speeds
	int shooterSlow = 2000, shooterNormal = 4000, shooterFull = 8000;

	//Gamepad
	bool btn_driveToggle = false, btn_shooterToggle = false;
	uint8_t teleopTicks;

	// Panel
	bool runWinch = false, runAgitator = false;

	std::shared_ptr<NetworkTable> table;

	std::string pathShooterFull = "/home/lvuser/shooterFull.txt",
			pathShooterNormal = "/home/lvuser/shooterNormal.txt",
			pathShooterSlow = "/home/lvuser/shooterSlow.txt";

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
 *   low    -> panel btn 1 (label: HI)
 *   normal -> panel btn 2 (label: mid)
 *   high   -> panel btn 3 (label: dwn)
 *  Drive speed: (toggle)
 *   low    (toggle) -> gamepad btn 7 (left bumper)
 *   normal (toggle) -> gamepad btn 8 (right bumper)
 *   high   (toggle) -> gamepad btn 6 (right bumper)
 *  Winch: (when pressed) -> panel btn 4
 *  Drop: (one time toggle, default state false/up) ->
 *
 *	!-COMPLETED-!
 *  Drive speeds done (gamepad)
 *  Shooter speeds done. (panel)
 *  Shooter encoder done.
 */
