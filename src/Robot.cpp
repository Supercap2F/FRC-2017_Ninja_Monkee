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
	void drive(double x, double y, double z, double gyro=0) {
		if(gamepad.GetRawButton(1)){
			robotDrive.MecanumDrive_Cartesian(-x/.77,y/.77,z/.77,gyro);
		}else{
			switch(driveLevel){
				case(full):
					robotDrive.MecanumDrive_Cartesian(-x,y,z,gyro);
					break;
				default:
					robotDrive.MecanumDrive_Cartesian(-x/.66,y/.66,z/.66,gyro);
					break;
			}
		}
	}

	void run_shooter() {
		shooter->Set(0);


	}

	void pollControllers(){
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

		if(gamepad.GetRawButton(2)){
			driveLevel=full;
		}else{
			driveLevel=normal;
		}

	}

	void pollSensors() {
		shooterEncoderRate = shooterEncoder->GetRate();
	}

	void sendDataToDriverStation() {
		frc::SmartDashboard::PutNumber("shooter encoder rate",
				shooterEncoderRate);
		frc::SmartDashboard::PutNumber("forward backward",
						forwardBackward);
		frc::SmartDashboard::PutNumber("strafe",
						strafe);
		frc::SmartDashboard::PutNumber("turn ",
						turn);
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
		pollControllers();
		pollSensors();
		drive(strafe,forwardBackward,turn,0);
		run_shooter();
		sendDataToDriverStation();
	}

	void TestPeriodic() { // only runs if robot is in test mode
		lw->Run();
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

	static constexpr int kJoystickChannel = 2;
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
	//drive vars
	double forwardBackward;
	double turn;
	double strafe;
	bool shoot = false;

	frc::Joystick gamepad { kXboxChannel };

	Encoder *shooterEncoder;
	static const int shooterEncoderChannelA = 0;
	static const int shooterEncoderChannelB = 1;
	double shooterEncoderRate; // encoder rate value, is updated ONLY in pollSensors, and ONLY sent to dashboard in sendDataToDriverStation.

	TalonSRX *shooter;
	TalonSRX *agitator;
	TalonSRX *winch;

	enum speedLevels {slow,normal,full};
	speedLevels driveLevel = normal;
	speedLevels shooterLevel = normal;


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


/*
 * Layout: (No joystick only game pads)
 * Controler: Game Pad
 *  Forward/Backward  -> left stick verticle
 *  Strafe Left/Right -> Left stick horazontal
 *  Turn left/Right   -> Right stick verticle
 *
 * Controler: Button Pad
 *  Shooter speed: (when pressed)
 *   low    ->
 *   normal ->
 *   high   ->
 *  Drive speed: (when pressed)
 *   low             ->
 *   normal (toggle) ->
 *   high   (toggle) ->
 *  Winch: (when pressed) ->
 *  Drop: (one time toggle, default state false/up) ->
 */
