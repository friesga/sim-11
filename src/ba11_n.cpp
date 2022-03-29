//
// Support for the BA11-N Mounting Box
//
#include <iostream>
#include <thread>

#include "lsi11.h"

using namespace std;

BA11_N::BA11_N ()
{
    this->ba11_nThread = thread (&BA11_N::bezel, this);
}

void BA11_N::bezel ()
{
    cout << "Started bezel" << endl;
    while (1)
    {
        this_thread::sleep_for (chrono::milliseconds (5000));
        cout << "Bezel active" << endl;
    }
}
