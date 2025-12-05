#ifndef _DUMMY_HALTMODE_H_
#define _DUMMY_HALTMODE_H_

class DummyHaltMode
{
public:
    DummyHaltMode () = default;
    void setHaltMode (bool haltMode);
    bool inHaltMode () const;
};


#endif // _DUMMY_HALTMODE_H_