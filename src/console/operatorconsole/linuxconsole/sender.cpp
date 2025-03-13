#include "linuxconsole.h"

#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <termios.h>
#include <iostream>

using std::runtime_error;
using std::cerr;

void LinuxConsole::sender ()
try
{
    struct termios original_tio;
	struct termios tio;
	fd_set fds;
	struct timeval timeout;

    if (tcgetattr (0, &original_tio) == -1)
		throw runtime_error ("Failed to retrieve TTY configuration");

	consoleRunning_ = true;

    // Set console to raw mode
	tio = original_tio;
	tio.c_iflag &= ~(ICRNL | INPCK | ISTRIP);
	tio.c_oflag &= ~OPOST;
	tio.c_cflag |= CS8;
	tio.c_lflag &= ~(ECHO | ICANON | IEXTEN);
	tcsetattr (0, TCSANOW, &tio);

	while (consoleRunning_)
	{
		char c;

		// Clear the set of filedescriptors and add stdin to the set. The
		// set has to be initialized at every select call.
		FD_ZERO (&fds);
		FD_SET (STDIN_FILENO, &fds);

		// Define a time out value of 50 msecs. This value also has to be set
		// for every select call as Linux modifies timeout to reflect the
		// amount of time not slept.
		timeout.tv_sec = 0;
		timeout.tv_usec = 50;

		static const int highestFileDescriptor = STDIN_FILENO + 1;

		// Check if stdin (fd 0) is ready for reading
		// The select() will time-out regurarly to check the value of
		// consoleRunning_. 
		int numFileDescriptors = 
			select (highestFileDescriptor, &fds, NULL, NULL, &timeout);
		if (numFileDescriptors == -1)
			// An error occurred
			throw runtime_error ("select error");
		else if (numFileDescriptors == 0)
			// A time-out occurred
			continue;
		else
		{
			// There is data to read; read one character from stdin
			if (read (0, &c, 1) != 1)
				throw runtime_error ("Read console error");

			send (c);
		}
	}

	tcsetattr (0, TCSANOW, &original_tio);
}
catch (const std::exception& ex)
{
	cerr << "LinuxConsole::sender exception: " << ex.what () << '\n';
}
