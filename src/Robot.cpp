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

class Robot: public frc::IterativeRobot {
public:
	void drive() {
		robotDrive.MecanumDrive_Cartesian((jX), (jY), (j2X));
	}

	void run_shooter() {
		switch(shooterLevel){
		case sl_slow:
			shooter->Set(j2Z);
		case sl_normal:;
		case sl_full:;
		}

	}

	void pollSensors() {
		if (joystick.GetX() > jDead || joystick.GetX() < -jDead) {
			jX = joystick.GetX();
		} else {
			jX = 0;
		}

		if (joystick.GetY() > jDead || joystick.GetY() < -jDead) {
			jY = joystick.GetY();
		} else {
			jY = 0;
		}
		if (joystick.GetZ() > jDeadZ || joystick.GetZ() < -jDeadZ) {
			jZ = joystick.GetZ();
		} else {
			jZ = 0;
		}

		if (joystick2.GetX() > j2Dead || joystick2.GetX() < -j2Dead) {
			j2X = joystick2.GetX();
		} else {
			j2X = 0;
		}

		if (joystick2.GetY() > j2Dead || joystick2.GetY() < -j2Dead) {
			j2Y = joystick2.GetY();
		} else {
			j2Y = 0;
		}

		if (joystick2.GetZ() > j2DeadZ || joystick2.GetZ() < -j2DeadZ) {
			j2Z = joystick2.GetZ();
		} else {
			j2Z = 0;
		}
		shooterEncoderRate = shooterEncoder->GetRate();
	}

	void sendDataToDriverStation() {
		frc::SmartDashboard::PutNumber("shooter encoder rate",
				shooterEncoderRate);
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
		// std::string autoSelected = SmartDashboard::GetString("Auto Selector", autoNameDefault);
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
		robotDrive.SetExpiration(0.1);

		// Invert the left side motors
		robotDrive.SetInvertedMotor(RobotDrive::kFrontLeftMotor, true);

		// You may need to change or remove this to match your robot
		robotDrive.SetInvertedMotor(RobotDrive::kRearLeftMotor, true);

	}

	void TeleopPeriodic() {
		/* The TeleopPeriodic method is called each time the robot recieves a packet instructing it
		 * to be in teleoperated mode
		 */
		pollSensors();
		drive();
		sendDataToDriverStation();
	}

	void TestPeriodic() { // only runs if robot is in test mode
		lw->Run();
		pollSensors();
		drive();
		run_shooter();
		sendDataToDriverStation();
		// get encoder values and send them to the dashboard
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

	static constexpr int kJoystickChannel = 0;
	static constexpr int kJoystickChannel2 = 1;
	static constexpr int kXboxChannel = 0;

	// Robot drive system
	frc::RobotDrive robotDrive { kFrontLeftChannel, kRearLeftChannel,
			kFrontRightChannel, kRearRightChannel };

	/********************************************************
	 * Joystick setup
	 * NOTE: this code is not needed atm. Uncomment (and comment
	 * the xbox code) for use. Do not leave both sections uncommented.
	 *
	 ********************************************************/
	frc::Joystick joystick { kJoystickChannel };
	double jX;
	double jY;
	double jZ;
	double jDead = 0.1;
	double jDeadZ = 0.04;
	frc::Joystick joystick2 { kJoystickChannel2 };
	double j2X;
	double j2Y;
	double j2Z;
	double j2Dead = 0.1;
	double j2DeadZ = 0.04;

	Encoder *shooterEncoder;
	static const int shooterEncoderChannelA = 0;
	static const int shooterEncoderChannelB = 1;
	double shooterEncoderRate; // encoder rate value, is updated ONLY in pollSensors, and ONLY sent to dashboard in sendDataToDriverStation.

	TalonSRX *shooter;
	TalonSRX *agitator;
	TalonSRX *winch;

	enum shooterLevel {sl_slow,sl_normal,sl_full};

};

START_ROBOT_CLASS(Robot);

/* IMPORTANT NOTES
 * If the build fails, try re-indexing (re-building) the code.
 * That will be an issue when pulling updates from the repo.
 *
 *
 * WHEN COMMITING CODE, CLEAN PROJECT FIRST
 *
 * */
