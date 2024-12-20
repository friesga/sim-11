#include "qbus/qbus.h"

#include <gtest/gtest.h>
#include <array>

using std::array;

class DummyBusDevice : public BusDevice
{
public:
	DummyBusDevice (int id) : id_ {id} {}
	StatusCode read (BusAddress address, u16* destination)
	{
		*destination = id_;
		return StatusCode::OK;
	}
	StatusCode writeWord (BusAddress address, u16 value) { return StatusCode::OK; }
	StatusCode writeByte (BusAddress address, u8 value) { return StatusCode::OK; }
	bool responsible (BusAddress address) { return true; }
	void reset () {}

private:
	int id_;
};

TEST (QbusInstallTest, devicesCanBeInstalled)
{
	array<DummyBusDevice, 10> devices =
	{
		DummyBusDevice (0),
		DummyBusDevice (1),
		DummyBusDevice (2),
		DummyBusDevice (3),
		DummyBusDevice (4),
		DummyBusDevice (5),
		DummyBusDevice (6),
		DummyBusDevice (7),
		DummyBusDevice (8),
		DummyBusDevice (9)
	};
	Qbus testBus;

	// Install a device in every slot
	u16 slotNumber {0};
	for (BusDevice& device : devices)
    {
        testBus.installModule (slotNumber, &devices[slotNumber]);
		slotNumber++;
    }

	// Verify all devices are installed in their expected slot
	int slotId {0};
	u16 deviceId;
	for (auto slot : testBus)
	{
		EXPECT_EQ (slot->read (0, &deviceId), StatusCode::OK);
		EXPECT_EQ (deviceId, slotId++);
	}

	auto module = *testBus[0];
	EXPECT_EQ (module->read (0, &deviceId), StatusCode::OK);
	EXPECT_EQ (deviceId, 0);
}