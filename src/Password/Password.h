#ifndef PASSWORD_H
#define PASSWORD_H

// Arduino versioning.
#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"	// for digitalRead, digitalWrite, etc
#else
#include "WProgram.h"
#endif

class Password {
public:
	Password(String pass);
	
	void set(String pass);
	bool is(String pass);
	bool append(char character);
	char pop();
	void reset();
	bool enoughGuess();
	bool evaluate();
	size_t length();
	size_t guessLength();
	String getPassword();
	String getGuess();
	
	//operators
	Password &operator=(String pass);
	bool operator==(String pass);
	bool operator!=(String pass);
	Password &operator<<(char character);
	
private:
	String target;
	String guess;
};

#endif
