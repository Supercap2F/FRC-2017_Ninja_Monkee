/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   robots.cpp
 * Author: majorChaos
 *
 * Created on March 20, 2017, 4:26 PM
 */

#include <stdlib.h>
#include <iostream>
#include <string>
#include <time.h>
#include <ctime>

#include "fleet.h"
#include "toolbox.h"

using namespace std;

class action: public dataBoat {
public:

	action() :
			dataBoat() {
		this->time = 0;
		this->className = "action";
	}

	action(const action& other) :
			dataBoat(other) {
		this->time = other.time;
	}

	virtual bool copy(dataBoat** to) {
		if (to == NULL) {
			return false;
		}
		if (*to == NULL) {
			*to = new action();
		}
		if (!dataBoat::copy(to)) {
			return false;
		}
		action* a = (action*) *to;
		a->time = this->time;
		return true;
	}

	virtual bool toVars(
			std::vector<std::pair<std::string, std::string> >* vars) {
		pair<string, string> p;
		string tmp;
		p.first = "time";
		if (toolbox::doubleToString(this->time, &tmp)
				!= toolbox::toolboxError_noError) {
			return false;
		}
		p.second = tmp;
		vars->push_back(p);

		//Parent serilize
		if (!dataBoat::toVars(vars)) {
			return false;
		}

		return true;
	}

	virtual bool fromVars(
			std::vector<std::pair<std::string, std::string> >* vars, int pos) {
		if (vars == NULL || vars->size() < (unsigned) pos + 1) {
			return false;
		}
		double tmp;

		//time
		if (toolbox::toDouble(&((*vars)[pos++].second), &tmp) != true) {
			return false;
		}
		this->time = tmp;

		//Parent unserilize
		if (!dataBoat::fromVars(vars, pos)) {
			return false;
		}
		return true;
	}
};

namespace actions {
class driveX: public action {
public:
	double x;

	driveX() :
			action(), x(0) {
		this->className = "driveX";
	}

	virtual bool copy(dataBoat** to) {
		if (to == NULL) {
			return false;
		}
		if (*to == NULL) {
			*to = new driveX();
		}
		if (!action::copy(to)) {
			return false;
		}
		driveX* o = (driveX*) *to;
		o->x = this->x;
		return true;
	}

	virtual bool toVars(
			std::vector<std::pair<std::string, std::string> >* vars) {
		pair<string, string> p;
		string tmp;
		p.first = "dX";
		if (toolbox::toString(this->x * 100, &tmp)
				!= toolbox::toolboxError_noError) {
			return false;
		}
		p.second = tmp;
		vars->push_back(p);

		//Parent serilize
		if (!action::toVars(vars)) {
			return false;
		}

		return true;
	}

	virtual bool fromVars(
			std::vector<std::pair<std::string, std::string> >* vars, int pos) {
		if (vars == NULL || vars->size() < (unsigned) pos + 1) {
			return false;
		}
		int tmp;

		//time
		if (toolbox::toInt(&((*vars)[pos++].second), &tmp) != true) {
			return false;
		}
		this->x = tmp / (double) 100;

		//Parent unserilize
		if (!action::fromVars(vars, pos)) {
			return false;
		}
		return true;
	}

};

class driveY: public action {
public:
	double y;

	driveY() :
			action(), y(0) {
		this->className = "driveY";
	}

	virtual bool copy(dataBoat** to) {
		if (to == NULL) {
			return false;
		}
		if (*to == NULL) {
			*to = new driveY();
		}
		if (!action::copy(to)) {
			return false;
		}
		driveY* o = (driveY*) *to;
		o->y = this->y;
		return true;
	}

	virtual bool toVars(
			std::vector<std::pair<std::string, std::string> >* vars) {
		pair<string, string> p;
		string tmp;
		p.first = "dY";
		if (toolbox::toString(this->y * 100, &tmp)
				!= toolbox::toolboxError_noError) {
			return false;
		}
		p.second = tmp;
		vars->push_back(p);

		//Parent serilize
		if (!action::toVars(vars)) {
			return false;
		}

		return true;
	}

	virtual bool fromVars(
			std::vector<std::pair<std::string, std::string> >* vars, int pos) {
		if (vars == NULL || vars->size() < (unsigned) pos + 1) {
			return false;
		}
		int tmp;

		//time
		if (toolbox::toInt(&((*vars)[pos++].second), &tmp) != true) {
			return false;
		}
		this->y = tmp / (double) 100;

		//Parent unserilize
		if (!action::fromVars(vars, pos)) {
			return false;
		}
		return true;
	}

};

class driveZ: public action {
public:
	double z;

	driveZ() :
			action(), z(0) {
		this->className = "driveZ";
	}

	virtual bool copy(dataBoat** to) {
		if (to == NULL) {
			return false;
		}
		if (*to == NULL) {
			*to = new driveZ();
		}
		if (!action::copy(to)) {
			return false;
		}
		driveZ* o = (driveZ*) *to;
		o->z = this->z;
		return true;
	}

	virtual bool toVars(
			std::vector<std::pair<std::string, std::string> >* vars) {
		pair<string, string> p;
		string tmp;
		p.first = "dZ";
		if (toolbox::toString(this->z * 100, &tmp)
				!= toolbox::toolboxError_noError) {
			return false;
		}
		p.second = tmp;
		vars->push_back(p);

		//Parent serilize
		if (!action::toVars(vars)) {
			return false;
		}

		return true;
	}

	virtual bool fromVars(
			std::vector<std::pair<std::string, std::string> >* vars, int pos) {
		if (vars == NULL || vars->size() < (unsigned) pos + 1) {
			return false;
		}
		int tmp;

		//time
		if (toolbox::toInt(&((*vars)[pos++].second), &tmp) != true) {
			return false;
		}
		this->z = tmp / (double) 100;

		//Parent unserilize
		if (!action::fromVars(vars, pos)) {
			return false;
		}
		return true;
	}

};

class driveGear: public action {
public:
	enum gears {
		slow = 0, normal = 1, full = 2
	};
	gears gear;

	driveGear() :
			action(), gear(slow) {
		this->className = "driveGear";
	}

	virtual bool copy(dataBoat** to) {
		if (to == NULL) {
			return false;
		}
		if (*to == NULL) {
			*to = new driveGear();
		}
		if (!action::copy(to)) {
			return false;
		}
		driveGear* o = (driveGear*) *to;
		o->gear = this->gear;
		return true;
	}

	virtual bool toVars(
			std::vector<std::pair<std::string, std::string> >* vars) {
		pair<string, string> p;
		string tmp;
		p.first = "dGear";
		if (toolbox::toString(this->gear, &tmp)
				!= toolbox::toolboxError_noError) {
			return false;
		}
		p.second = tmp;
		vars->push_back(p);

		//Parent serilize
		if (!action::toVars(vars)) {
			return false;
		}

		return true;
	}

	virtual bool fromVars(
			std::vector<std::pair<std::string, std::string> >* vars, int pos) {
		if (vars == NULL || vars->size() < (unsigned) pos + 1) {
			return false;
		}
		int tmp;

		//time
		if (toolbox::toInt(&((*vars)[pos++].second), &tmp) != true) {
			return false;
		}
		if (tmp != slow && tmp != normal && tmp != full) {
			return false;
		}
		switch (tmp) {
		case (slow):
			this->gear = slow;
			break;
		case (normal):
			this->gear = normal;
			break;
		case (full):
			this->gear = full;
			break;
		default:
			return false;
		}

		//Parent unserilize
		if (!action::fromVars(vars, pos)) {
			return false;
		}
		return true;
	}

};

class gyro: public action {
public:
	int value;

	gyro() :
			action(), value(0) {
		this->className = "gyro";
	}

	virtual bool copy(dataBoat** to) {
		if (to == NULL) {
			return false;
		}
		if (*to == NULL) {
			*to = new gyro();
		}
		if (!action::copy(to)) {
			return false;
		}
		gyro* o = (gyro*) *to;
		o->value = this->value;
		return true;
	}

	virtual bool toVars(
			std::vector<std::pair<std::string, std::string> >* vars) {
		pair<string, string> p;
		string tmp;
		p.first = "gy";
		if (toolbox::toString(this->value, &tmp)
				!= toolbox::toolboxError_noError) {
			return false;
		}
		p.second = tmp;
		vars->push_back(p);

		//Parent serilize
		if (!action::toVars(vars)) {
			return false;
		}

		return true;
	}

	virtual bool fromVars(
			std::vector<std::pair<std::string, std::string> >* vars, int pos) {
		if (vars == NULL || vars->size() < (unsigned) pos + 1) {
			return false;
		}
		int tmp;

		//time
		if (toolbox::toInt(&((*vars)[pos++].second), &tmp) != true) {
			return false;
		}
		this->value = tmp;

		//Parent unserilize
		if (!action::fromVars(vars, pos)) {
			return false;
		}
		return true;
	}

};

class shoot: public action {
public:
	enum gears {
		off = 0, slow = 1, normal = 2, full = 3
	};
	gears gear;

	shoot() :
			action(), gear(slow) {
		this->className = "shoot";
	}

	virtual bool copy(dataBoat** to) {
		if (to == NULL) {
			return false;
		}
		if (*to == NULL) {
			*to = new shoot();
		}
		if (!action::copy(to)) {
			return false;
		}
		shoot* o = (shoot*) *to;
		o->gear = this->gear;
		return true;
	}

	virtual bool toVars(
			std::vector<std::pair<std::string, std::string> >* vars) {
		pair<string, string> p;
		string tmp;
		p.first = "value";
		if (toolbox::toString(this->gear, &tmp)
				!= toolbox::toolboxError_noError) {
			return false;
		}
		p.second = tmp;
		vars->push_back(p);

		//Parent serilize
		if (!action::toVars(vars)) {
			return false;
		}

		return true;
	}

	virtual bool fromVars(
			std::vector<std::pair<std::string, std::string> >* vars, int pos) {
		if (vars == NULL || vars->size() < (unsigned) pos + 1) {
			return false;
		}
		int tmp;

		//time
		if (toolbox::toInt(&((*vars)[pos++].second), &tmp) != true) {
			return false;
		}
		if (tmp != off && tmp != slow && tmp != normal && tmp != full) {
			return false;
		}
		switch (tmp) {
		case (off):
			this->gear = off;
			break;
		case (slow):
			this->gear = slow;
			break;
		case (normal):
			this->gear = normal;
			break;
		case (full):
			this->gear = full;
			break;
		default:
			return false;
		}

		//Parent unserilize
		if (!action::fromVars(vars, pos)) {
			return false;
		}
		return true;
	}
};

class agitator: public action {
public:
	bool onOff;
	agitator() :
			action(), onOff(false) {
	}
	virtual bool copy(dataBoat** to) {
		if (to == NULL) {
			return false;
		}
		if (*to == NULL) {
			*to = new agitator();
		}
		if (!action::copy(to)) {
			return false;
		}
		agitator* o = (agitator*) *to;
		o->onOff = this->onOff;
		return true;
	}
	virtual bool toVars(
			std::vector<std::pair<std::string, std::string> >* vars) {
		pair<string, string> p;
		string tmp;
		p.first = "onOff";
		if (toolbox::boolToString(this->onOff, &tmp)
				!= toolbox::toolboxError_noError) {
			return false;
		}
		p.second = tmp;
		vars->push_back(p);

		//Parent serilize
		if (!action::toVars(vars)) {
			return false;
		}

		return true;
	}
	virtual bool fromVars(
			std::vector<std::pair<std::string, std::string> >* vars, int pos) {
		if (vars == NULL || vars->size() < (unsigned) pos + 1) {
			return false;
		}

		//time
		if (toolbox::toBool(&((*vars)[pos++].second), &this->onOff) != true) {
			return false;
		}

		//Parent unserilize
		if (!action::fromVars(vars, pos)) {
			return false;
		}
		return true;
	}
};

}

/*
 * Simple C++ Test Suite
 */
/*
 void test1() {
 return;
 std::cout << "robots test 1" << std::endl;
 fleet f = fleet();
 using namespace actions;
 driveX x = driveX();
 driveY y = driveY();
 driveZ z = driveZ();
 gyro g = gyro();
 driveGear gear = driveGear();
 shoot s = shoot();
 u_int32_t id;

 //Add templates
 f.addTemplate(&x,&id);
 f.addTemplate(&y,&id);
 f.addTemplate(&z,&id);
 f.addTemplate(&g,&id);
 f.addTemplate(&gear,&id);
 f.addTemplate(&s,&id);

 //Add Entries (actions)
 cout << "Making entries" << endl;
 f.entries.resize(45000*10);
 srand(time(NULL));
 clock_t begin = 0;
 int last_i = 0;
 for(int i=0;i<45000;i++){
 if((double(clock() - begin) / CLOCKS_PER_SEC)>10){
 cout << "to go = " << 45000-i << "; per 10 sec = " << (i-last_i) << endl;
 last_i=i;
 begin = clock();
 }
 x.time = i*20;
 x.x = (rand() % 100 - 100)/(double)100;
 f.addEntry(&x,&id);

 y.time = i*20;
 y.y = (rand() % 100 - 100)/(double)100;
 f.addEntry(&y,&id);
 z.time = i*20;
 z.z = (rand() % 100 - 100)/(double)100;
 f.addEntry(&z,&id);
 g.time = i*20;
 g.value = rand() % 360 + 0;
 f.addEntry(&g,&id);
 gear.time = i*20;
 switch(rand() % 100 + 0){
 case(0):gear.gear=driveGear::slow; break;
 case(1):gear.gear=driveGear::normal; break;
 case(2):gear.gear=driveGear::full; break;
 }
 f.addEntry(&gear,&id);
 s.time = i*20;
 switch(rand() % 3 + 0){
 case(0):s.gear=shoot::off; break;
 case(1):s.gear=shoot::slow; break;
 case(2):s.gear=shoot::normal; break;
 case(3):s.gear=shoot::full; break;
 }
 f.addEntry(&s,&id);
 }

 //serilize
 string tmp = "";
 f.toString(&tmp);
 //cout << "size of cfg = " << tmp.size() << endl;
 //cout << tmp << endl;

 cout << "writing to file" << endl;
 string path = "robo.cfg";
 if(toolbox::formattedWrite(&path,&tmp)){
 cout << "wrote to file" << endl;
 }else{
 std::cout << "%TEST_FAILED% time=0 testname=test1 (robots) message=can't write to file" << std::endl;
 return;
 }

 string tmp2;
 cout << "Reading file" << endl;
 if(toolbox::readFormatted(&path,&tmp2)){
 cout << "read file" << endl;
 }else{
 std::cout << "%TEST_FAILED% time=0 testname=test1 (robots) message=can't read file" << std::endl;
 return;
 }
 //cout << tmp2.size() << endl;

 //unserilize
 cout << "unserilizing" << endl;
 if(f.fromString(&tmp2,true)){
 cout << "serilized data" << endl;
 }else{
 std::cout << "%TEST_FAILED% time=0 testname=test1 (robots) message=can't unserilize data" << std::endl;
 return;
 }

 cout << "deconstructing" << endl;
 //std::cout << "%TEST_FAILED% time=0 testname=test1 (robots) message=error message sample" << std::endl;
 }
 */
/*
 int main(int argc, char** argv) {
 std::cout << "%SUITE_STARTING% robots" << std::endl;
 std::cout << "%SUITE_STARTED%" << std::endl;

 std::cout << "%TEST_STARTED% test1 (robots)\n" << std::endl;
 test1();
 std::cout << "%TEST_FINISHED% time=0 test1 (robots)" << std::endl;

 std::cout << "%SUITE_FINISHED% time=0" << std::endl;
 cout << "the EXIT" << endl;

 return (EXIT_SUCCESS);
 }
 */

