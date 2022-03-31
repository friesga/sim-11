#ifndef _BA11_N_H_
#define _BA11_N_H_

#include <thread>

class BA11_N
{
public:
	BA11_N ();

private:
	void bezel ();

	std::thread ba11_nThread;
};

#endif // _BA11_N_H_
