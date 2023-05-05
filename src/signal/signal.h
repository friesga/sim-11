#ifndef _SIGNAL_H_
#define _SIGNAL_H_

#include <vector>
#include <functional>
#include <mutex>

using std::function;
using std::vector;
using std::mutex;
using std::lock_guard;

class Signal
{
public:
    using Subscriber = function<void(bool)>;
	using SubscriberKey = ptrdiff_t;

    Signal (bool initialValue);
    Signal ();
    ~Signal () = default;
    void set (bool value, SubscriberKey sender = 0);
    operator bool ();
    void cycle (SubscriberKey sender = 0);
    SubscriberKey subscribe (Subscriber);

private:
    bool value_;
    vector<Subscriber> subscribers_;
    mutex signalMutex_;

    void notifyObservers (SubscriberKey sender);
};


#endif // _SIGNAL_H_