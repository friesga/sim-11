#ifndef _BA11NCONFIG_H_
#define _BA11NCONFIG_H_

struct BA11_NConfig
{
    // Definition of the available front cover logo's. The logo has no
    // part number and there are no separate BA11-N versions for PDP-11/03's
    // and PDP-11/23's, but the logo is separate item as defined in the
    // BA11-N parts list (EK-BA11N-UG-001).
    enum class Logo
    {
        PDP_1103L,
        PDP_1123,
        PDP_1123_PLUS
    };

    Logo logo {Logo::PDP_1103L};
};

#endif // !_BA11NCONFIG_H_
