/* Copyright 2018 PARCOLLET Nicolas, see COPYING file */
#include "ether.h"

Event::Event(const std::string & sender, const std::string & type, const std::vector<Variant> & args)
:
	_sender(sender),
	_type(type),
	_args(args)
{
}

Event::~Event()
{
}

std::string Event::sender() const
{
	return _sender;
}

std::string Event::type() const
{
	return _type;
}

std::string Event::string(int index) const
{
	return _args[index].asString();
}

double Event::number(int index) const
{
	return _args[index].asNumber();
}
