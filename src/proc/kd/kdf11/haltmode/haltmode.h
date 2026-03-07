#ifndef _KDF11_HALTMODE_H_
#define _KDF11_HALTMODE_H_

class KDF11_HaltMode
{
public:
    KDF11_HaltMode () = default;
    void setHaltMode (bool haltMode);
    bool inHaltMode () const;

private:
    bool haltMode_ {false};
};


#endif // _KDF11_HALTMODE_H_