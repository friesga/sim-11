#ifndef _NOSTACKOVERFLOWPOLICY_H_
#define _NOSTACKOVERFLOWPOLICY_H_

class NoStackOverflowPolicy
{
public:
    bool stackOverflow() const
    {
        return false;
    }
};


#endif // _NOSTACKOVERFLOWPOLICY_H_
