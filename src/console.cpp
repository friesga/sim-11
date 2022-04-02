#include "dlv11j/dlv11j.h"

#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <termios.h>
#include <signal.h>

#ifdef _WIN32
// #include <fcntl.h>
#include <termio.h>
// #include <clock_gettime.h>
#endif

#ifdef _WIN32
#define tcsetattr(fd, act, tp)		setattr_stdin (hStdin)
#define tcgetattr(fd, tp)			getattr_stdin (hStdin)
#else
#define	select_stdin(x)				select(1, &fds, NULL, NULL, &tv)
#define read_stdin(x,buf,count)		read(0, buf, count)
#endif

struct termios tio;
#ifdef _WIN32
	HANDLE hStdin = open_stdin();
#else
	struct sigaction sa;
#endif // _WIN32

struct termios original_tio;
fd_set fds;
struct timeval tv;

extern volatile int running;

void sigint_handler (int signum)
{
	running = 0;
}

void readStdin(DLV11J &dlv11)
{
#ifndef DEBUG
	if (tcgetattr (0, &original_tio) == -1)
	{
		// printf("Failed to retrieve TTY configuration\n");
		// return 1;
		throw ("Failed to retrieve TTY configuration\n");
	}
#endif

#ifndef DEBUG
#ifdef _WIN32
	signal (SIGINT, SIG_DFL);
#else
	sa.sa_handler = sigint_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if(sigaction(SIGINT, &sa, NULL) == -1) {
		printf("WARNING: failed to set SIGINT signal handler\n");
	}
#endif

	/* set raw mode */
	tio = original_tio;
	tio.c_iflag &= ~(ICRNL | INPCK | ISTRIP);
	tio.c_oflag &= ~OPOST;
	tio.c_cflag |= CS8;
	tio.c_lflag &= ~(ECHO | ICANON | IEXTEN);
	tcsetattr (0, TCSANOW, &tio);
#endif

	FD_ZERO (&fds);
	FD_SET (0, &fds);

	// No timeout on select
	tv.tv_sec = 0;
	tv.tv_usec = 0;

	while (running)
	{
		char c;

		if (select_stdin (hStdin))
		{		
			(void) !read_stdin (hStdin, &c, 1);
#ifdef DEBUG
			if(c == '\n')
				c = '\r';
#endif
			dlv11.Send (3, c);
		}
	}

	tcsetattr (0, TCSANOW, &original_tio);
}