#include "dlconfig.h"

DLConfig::DLConfig ()
    : 
    DeviceConfig (DeviceType::DLV11_J)
{
	createDLV11J_UARTsConfig ();
}

// This function creates the UARTConfig objects for the DLV11-J, using
// the configured base address and vector for the module plus the
// chConsoleEnabled option.
// This function might be called twice; first to create the default
// configuration and a second time after processing of the DLV11-J
// configuration options. The default UART configuration is created to make
// sure that a system with the default configuration is able to print at
// least the halting address.
void DLConfig::createDLV11J_UARTsConfig ()
{
	if (uarts.size () > 0)
		uarts.clear ();

    for (u16 channelNr = 0; channelNr < numChannels; ++channelNr)
	{
		if (channelNr == 3 && ch3ConsoleEnabled)
			uarts.push_back (UARTConfig
			{
				defaultCh3Address_, 
				defaultCh3Vector_
			});
		else
			uarts.push_back (UARTConfig
			{
				static_cast<u16> (baseAddress + 8 * channelNr),
				static_cast<u16> (baseVector + 8 * channelNr)}
			);
	}
}
