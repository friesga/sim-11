#ifndef _DEVICETYPE_H_
#define _DEVICETYPE_H_

// Every PDP-11 has one principal bus. That bus is either a Unibus or a
// QBus. Other busses, like the Extended Unibus (EUB), are regarded as
// a Unibus.
enum class BusType
{
    Unibus,
    QBus
};

// Every PDP-11 device is either a Unibus or a QBus device. In principle
// - apart from the use of converters - a Unibus device can only be connected
// to a Unibus and a QBus device can only be connected to a QBus.
template <BusType type>
class DeviceType
{
public:
    DeviceType () { type_ = type; }
    bool isQbusDevice () { return type_ == BusType::QBus; }
    bool isUnibusDevice () { return type_ == BusType::Unibus; }

private:
    BusType type_;
};

#endif // _DEVICETYPE_H_