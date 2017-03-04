#include <string>
#include <stdlib.h>

using namespace std;

class base{
public:
	const static char divider = ',';
	string baseName;
	virtual string toString(){return "";};
	virtual bool fromString(string* str, base* o){
		base tmp;
		tmp.baseName = this->baseName;
		(*o) = tmp;
		return true;
	};
	base(){this->baseName = "base";};
	virtual ~base(){};

	const static unsigned int splitString(string* s,int start, int end, string* result){
		string tmp = "";
		int x;
		for(x=start;x<(signed)s->size() && x<end;x++){
			tmp+=(*s)[x];
		}
		(*result) = tmp;
		return x;
	}

	const static unsigned int splitString(string* s, int start, int end, char stopAt, string* result){
		string tmp = "";
		char c;
		int x;
		for(x=start;x<(signed)s->size() && x<end;x++){
			c=(*s)[x];
			if(c==stopAt){
				break;
			}else{
				tmp+=c;
			}
		}
		(*result) = tmp;
		return x;
	}
};

class config : public base{
public:
	string name;
	int x;
	int y;
	int z;
	config(){
		this->baseName = "config";
		this->name = "configy";
		this->x = 0;
		this->y = 0;
		this->z = 0;
	}
	string toString(){
		// "x divider y divider z divider name"
		string t="";
		t += x;
		t += base::divider;
		t += y;
		t += base::divider;
		t += z;
		t += base::divider;
		t += this->name;
		return t;
	}

	bool fromString(string* str, base* o){
		int len = str->size();
		bool b;
		string tmp="";
		char c;
		int pos = 0;
		for(int x=0;x<len;x++){
			c = str->at(x);
			if((c==base::divider && pos!=3) || x+1==len){
				switch(pos){
				case(0):
						this->x = atoi(tmp.c_str());
						break;
				case(1):
						this->y = atoi(tmp.c_str());
						break;
				case(2):
						this->z = atoi(tmp.c_str());
						break;
				case(3):
						this->name = tmp;
						b=true;
						break;
				}
				tmp="";
				pos++;
			}else{
				tmp+=c;
			}
		}
		return b;
	}
};

class mem{
private:
	base* templates;
	int templatesLen;
	base* bases;
	bool* basesState;
	int basesLen;
	void clearBases(){
		delete[] this->bases;
		delete[] this->basesState;
		this->basesLen = 0;
	}
	void setBases(int num){
		this->bases = new base[num];
		this->basesState = new bool[num];
		this->basesLen = num;
	}
	void expandBases(int expandBy){
		base* o;
		bool* b;
		o = new base[this->basesLen];
		b = new bool[this->basesLen];
		for(int x=0;x<this->basesLen;x++){
			o[x] = this->bases[x];
			b[x] = this->basesState[x];
		}
		int tmp = this->basesLen;
		clearBases();
		setBases(tmp+expandBy);
		for(int x=0;x<tmp;x++){
			o[x] = this->bases[x];
			b[x] = this->basesState[x];
		}
		delete[] o;
		delete[] b;
	}

	int getFreeSpot_bases(){
		for(int x=0;x<this->basesLen;x++){
			if(this->basesState[x]==false){
				return x;
			}
		}
		return -1;
	}

	int getTemplateByName(string* name){
		for(int x=0;x<this->templatesLen;x++){
			if(this->templates[x].baseName == *name){
				return x;
			}
		}
		return -1;
	}

public:
	const static char divider = ',';
	mem(){
		this->basesLen = 0;
		setBases(6);
	}
	~mem(){
		this->clearBases();
	}
	int add(base b){
		int spot = this->getFreeSpot_bases();
		if(spot<0){
			spot = this->basesLen;
			this->expandBases(1);
		}
		this->bases[spot] = b;
		this->basesState[spot] = true;
		return spot;
	}
	void remove(int spot){
		this->basesState[spot] = false;
	}
	string toString(){
		// "numberOfBases divider baseBaseNameSize divider baseName baseSize divider base"
		string s = "";
		string tmp;
		s+=this->basesLen;
		s+=mem::divider;
		for(int x=0;x<this->basesLen;x++){
			if(this->basesState[x]){
				tmp = this->bases[x].baseName;
				s+=tmp.size();
				s+=mem::divider;
				s+=tmp;
				tmp = this->bases[x].toString();
				s+=tmp.size();
				s+=mem::divider;
				s+=tmp;
			}
		}
		return s;
	}

	bool fromString(string* s){
		bool b;
		string tmp;
		int pos = base::splitString(s, 0, s->size(), mem::divider, &tmp);
		this->clearBases();
		this->setBases(atoi(tmp.c_str()));

		string name;
		unsigned int nameLen, baseSize;
		int lastEntry = 0;
		int u;

		//Get name length
		pos = base::splitString(s, pos, s->size(), mem::divider, &tmp);
		nameLen = atoi(tmp.c_str());
		//Get name
		pos = base::splitString(s, pos, pos+nameLen, &tmp);
		name = tmp;
		//Get obj length
		pos = base::splitString(s, pos, s->size(), mem::divider, &tmp);
		baseSize = atoi(tmp.c_str());
		//Get obj
		pos = base::splitString(s, pos, pos+baseSize, &tmp);
		u = this->getTemplateByName(&name);
		//this->templates[u].fromString(&tmp);
		this->add(this->templates[u]);
		return b;
	}
};

