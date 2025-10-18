#ifndef _KY11_APROCESSOR_H_
#define _KY11_APROCESSOR_H_

#include "configdata/ka11/ky11_aconfig/ky11_aconfig.h"

class KY11_AProcessor 
{
public:
	KY11_AConfig getConfig ();

private:
	KY11_AConfig ky11_aConfig_ {};
};

#endif // _KY11_APROCESSOR_H_