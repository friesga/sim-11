#ifndef _SECTIONROCESSOR_H_
#define _SECTIONROCESSOR_H_

class SectionProcessor
{
public:
	virtual void processSection (iniparser::Section* section) = 0;
};

#endif // !_SECTIONPROCESSOR_H_
