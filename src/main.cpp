/* Copyright 2018 PARCOLLET Nicolas, see COPYING file */
#include "s9x.h"

int main(int argc, char ** argv)
{
	S9XLauncher launcher(argc, argv);
	return launcher.run(1280, 720);
}
