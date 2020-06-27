#include "Password.h"

//construct object in memory, set all variables
Password::Password(String pass)
{
	set(pass);
	reset();
}

//set the password
void Password::set(String pass)
{
	target = pass;
}

//evaluate a string, is it equal to the password?
bool Password::is(String pass)
{
	guess = pass;
	return evaluate();
}

//append a char to the guessed password
bool Password::append(char character)
{
	guess += character;
	return true;
}

//reset the guessed password, one can guess again
void Password::reset()
{
	guess = String("");
}

//is the current guessed password equal to the target password?
bool Password::evaluate()
{
	if (target == guess)
	{
		return true;
	}
	return false;
}

size_t Password::length()
{
	return target.length();
}

size_t Password::guessLength()
{
	return guess.length();
}

String Password::getPassword()
{
	return target;
}

String Password::getGuess()
{
	return guess;
}

//set password using operator =
Password &Password::operator=(String pass)
{
	set(pass);
	return *this;
}

//test password using ==
bool Password::operator==(String pass)
{
	return is(pass);
}

//test password using !=
bool Password::operator!=(String pass)
{
	return !is(pass);
}

//append to currently guessed password using operator <<
Password &Password::operator<<(char character)
{
	append(character);
	return *this;
}
