#include "bus/qbus/qbus.h"

#include <gtest/gtest.h>
#include <array>
#include <vector>

using std::array;
using std::vector;

class DummyBusDevice : public BusDevice
{
public:
	DummyBusDevice (int id) : id_ {id} {}
	StatusCode read (BusAddress address, u16* destination)
	{
		*destination = id_;
		return StatusCode::Success;
	}
	StatusCode writeWord (BusAddress address, u16 value) { return StatusCode::Success; }
	StatusCode writeByte (BusAddress address, u8 value) { return StatusCode::Success; }
	bool responsible (BusAddress address) { return true; }
	void reset () {}

private:
	int id_;
};

TEST (QbusInstallTest, devicesCanBeInstalled)
{
	vector<DummyBusDevice> devices =
	{
		DummyBusDevice (0),
		DummyBusDevice (1),
		DummyBusDevice (2),
		DummyBusDevice (3),
		DummyBusDevice (4),
		DummyBusDevice (5),
		DummyBusDevice (6),
		DummyBusDevice (7),
		DummyBusDevice (8)
	};
	Qbus testBus;

	// Install a device in every slot
	u16 slotNumber {0};
	for (BusDevice& device : devices)
    {
        EXPECT_TRUE (testBus.installModule (&devices[slotNumber]));
		slotNumber++;
    }

	// Verify all devices are installed in their expected slot
	int slotId {0};
	u16 deviceId;
	for (auto slot : testBus)
	{
		EXPECT_EQ (slot->read (0, &deviceId), StatusCode::Success);
		EXPECT_EQ (deviceId, slotId++);
	}

	auto module = *testBus[0];
	EXPECT_EQ (module->read (0, &deviceId), StatusCode::Success);
	EXPECT_EQ (deviceId, 0);
}

TEST (QbusInstallTest, cannotInstallMoreThanNumberOfSlots)
{
	vector<DummyBusDevice> devices =
	{
		DummyBusDevice (0),
		DummyBusDevice (1),
		DummyBusDevice (2),
		DummyBusDevice (3),
		DummyBusDevice (4),
		DummyBusDevice (5),
		DummyBusDevice (6),
		DummyBusDevice (7),
		DummyBusDevice (8)
	};
	Qbus testBus;


	// Install a device in every slot
	u16 slotNumber {0};
	for (BusDevice& device : devices)
	{
		EXPECT_TRUE (testBus.installModule (&devices[slotNumber]));
		slotNumber++;
	}

	// Verify no more devices can be installed
	EXPECT_FALSE (testBus.installModule (&devices[0]));
	EXPECT_FALSE (testBus.installModuleAtPosition (&devices[0], 0));
}

TEST (QbusInstallTest, cannotInstallNullptr)
{
    Qbus testBus;
    EXPECT_FALSE (testBus.installModule (nullptr));
	EXPECT_FALSE (testBus.installModuleAtPosition (nullptr, 0));
}

TEST (QbusInstallTest, cannotInstallAtPositionOutOfRange)
{
    Qbus testBus;
	DummyBusDevice dummyBusDevice (0);
    EXPECT_FALSE (testBus.installModuleAtPosition (&dummyBusDevice,
		testBus.capacity () + 1));
}

// Verify that - while there are unoccupied slots - a device can be installed
// at a position that is occupied and that, as a result of this operation, the
// already installed devices are shifted one position.
TEST (QbusInstallTest, deviceCanBeInstalledAtOccupiedPosition)
{
	array<DummyBusDevice, 5> devices =
	{
		DummyBusDevice (0),
		DummyBusDevice (1),
		DummyBusDevice (2),
		DummyBusDevice (3),
		DummyBusDevice (4)
	};
	Qbus testBus;

	// First install a device in the first five slots
	u16 slotNumber {0};
	for (BusDevice& device : devices)
	{
		EXPECT_TRUE (testBus.installModule (&devices[slotNumber]));
		slotNumber++;
	}

    // Install a device in the last slot
	DummyBusDevice additionalBusDevice (10);
    EXPECT_TRUE (testBus.installModuleAtPosition (&additionalBusDevice, 0));

	// Verify all devices are installed in their expected slot
	array<int, 6> expectedDeviceIds {10, 0, 1, 2, 3, 4};
	int slotId {0};
	u16 deviceId;
	for (auto device : testBus)
	{
		EXPECT_EQ (device->read (0, &deviceId), StatusCode::Success);
		EXPECT_EQ (deviceId, expectedDeviceIds[slotId++]);
	}
}

TEST (QbusInstallTest, slotsMustBeOccupiedConsecutively)
{
    Qbus testBus;
	DummyBusDevice busDevice0 (0);
	DummyBusDevice busDevice1 (1);
	EXPECT_TRUE (testBus.installModule (&busDevice0));
    EXPECT_FALSE (testBus.installModuleAtPosition (&busDevice1, 2));
}

TEST (QbusInstallTest, deviceCanBeInstalledInFirstSlot)
{
	Qbus testBus;
	DummyBusDevice busDevice0 (0);
	EXPECT_TRUE (testBus.installModuleAtPosition (&busDevice0, 0));
}