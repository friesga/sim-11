#ifndef _PSEUDO_HALTMODE_H_
#define _PSEUDO_HALTMODE_H_

class PseudoHaltMode
{
public:
    PseudoHaltMode () = default;
    void setHaltMode (bool haltMode);
    bool inHaltMode () const;
};


#endif // _PSEDUO_HALTMODE_H_