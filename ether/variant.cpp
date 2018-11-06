#include "ether.h"

Variant::Variant()
:
	_ptr(nullptr),
	_type(TypeNull)
{
}

Variant::Variant(const Variant & other)
:
	_ptr(nullptr),
	_type(other._type)
{
	if (_type == TypeNumber) {
		_ptr = new double(other.asNumber());
	} else if (_type == TypeBoolean) {
		_ptr = new bool(other.asBoolean());
	} else if (_type == TypeString) {
		_ptr = new std::string(other.asString());
	} else if (_type == TypeArray) {
		_ptr = new std::list<Variant>(other.asArray());
	} else if (_type == TypeObject) {
		_ptr = new std::map<std::string, Variant>(other.asObject());
	} else if (_type == TypeNull) {
		_ptr = nullptr;
	}
}

Variant & Variant::operator=(const Variant & other)
{
	if (this != &other) {
		clear();
		_type = other._type;
		if (_type == TypeNumber) {
			_ptr = new double(other.asNumber());
		} else if (_type == TypeBoolean) {
			_ptr = new bool(other.asBoolean());
		} else if (_type == TypeString) {
			_ptr = new std::string(other.asString());
		} else if (_type == TypeArray) {
			_ptr = new std::list<Variant>(other.asArray());
		} else if (_type == TypeObject) {
			_ptr = new std::map<std::string, Variant>(other.asObject());
		} else if (_type == TypeNull) {
			_ptr = nullptr;
		}
	}
	return *this;
}

Variant::Variant(bool value)
:
	_ptr(new bool(value)),
	_type(TypeBoolean)
{
}

Variant::Variant(int value)
:
	_ptr(new double(value)),
	_type(TypeNumber)
{
}

Variant::Variant(float value)
:
	_ptr(new float(value)),
	_type(TypeNumber)
{
}

Variant::Variant(double value)
:
	_ptr(new double(value)),
	_type(TypeNumber)
{
}

Variant::Variant(const std::string & value)
:
	_ptr(new std::string(value)),
	_type(TypeString)
{
}

Variant::Variant(const char * value)
:
	_ptr(new std::string(value)),
	_type(TypeString)
{
}

Variant::Variant(const std::list<Variant> & value)
:
	_ptr(new std::list<Variant>(value)),
	_type(TypeArray)
{
}

Variant::Variant(const std::map<std::string, Variant> & value)
:
	_ptr(new std::map<std::string, Variant>(value)),
	_type(TypeObject)
{
}

Variant::~Variant()
{
	clear();
}

int Variant::type() const
{
	return _type;
}

double Variant::asNumber() const
{
	return *((double*)_ptr);
}

bool Variant::asBoolean() const
{
	return *((double*)_ptr);
}

std::list<Variant> Variant::asArray() const
{
	return *((std::list<Variant>*)_ptr);
}

std::map<std::string, Variant> Variant::asObject() const
{
	return *((std::map<std::string, Variant>*)_ptr);
}

std::string	Variant::asString()	const
{
	return *((std::string*)_ptr);
}

void Variant::set(bool value)
{
	clear();
	_type = TypeBoolean;
	_ptr = new bool(value);
}

void Variant::set(int value)
{
	clear();
	_type = TypeNumber;
	_ptr = new double(value);

}

void Variant::set(double value)
{
	clear();
	_type = TypeNumber;
	_ptr = new double(value);
}

void Variant::set(const std::string value)
{
	clear();
	_type = TypeString;
	_ptr = new std::string(value);
}

void Variant::set(const std::list<Variant> & value)
{
	clear();
	_type = TypeArray;
	_ptr = new std::list<Variant>(value);
}

void Variant::set(const std::map<std::string, Variant> & value)
{
	clear();
	_type = TypeObject;
	_ptr = new std::map<std::string, Variant>(value);
}

void Variant::clear()
{
	if (_ptr) {
		if (_type == TypeNumber) {
			delete (double*)_ptr;
		} else if (_type == TypeBoolean) {
			delete (bool*)_ptr;
		} else if (_type == TypeString) {
			delete (std::string *)_ptr;
		} else if (_type == TypeArray) {
			delete (std::list<Variant>*)_ptr;
		} else if (_type == TypeObject) {
			delete (std::map<std::string, Variant>*)_ptr;
		}
		_ptr	= nullptr;
		_type	= TypeNull;
	}
}
