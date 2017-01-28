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

class Robot: public frc::IterativeRobot {
public:
	void RobotInit() {
		chooser.AddDefault(autoNameDefault, autoNameDefault);
		chooser.AddObject(autoNameCustom, autoNameCustom);
		frc::SmartDashboard::PutData("Auto Modes", &chooser);
		//CameraServer::GetInstance()->StartAutomaticCapture();
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
		robotDrive.MecanumDrive_Cartesian((joystick.GetZ()), (joystick.GetY()),
							(joystick.GetX()));


	}

	void TestPeriodic() { // only runs if robot is in test mode
		lw->Run();
	}

private:
	frc::LiveWindow* lw = LiveWindow::GetInstance(); // this is for testing
	frc::SendableChooser<std::string> chooser;
	const std::string autoNameDefault = "Default";
	const std::string autoNameCustom = "My Auto";
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

};

START_ROBOT_CLASS(Robot)
