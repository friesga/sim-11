#include "kd11odt.h"
#include "trace/trace.h"

// Constructor
KD11ODT::KD11ODT (KD11CPU& cpu)
    :
    cpu_{ cpu },
    state{ ODT_STATE_INIT }
{}

void KD11ODT::reset ()
{
    state = ODT_STATE_INIT;
}

void KD11ODT::clear ()
{
    buf_sz = 0;
}

void KD11ODT::write (u8 c)
{
    buf[buf_sz++] = c;
    buf_r = 0;
}

void KD11ODT::writeOctal (u16 val)
{
    int i;
    buf[buf_sz++] = ((val >> 15) & 0x7) + '0';
    for (i = 0; i < 5; i++)
    {
        buf[buf_sz++] = ((val >> 12) & 7) + '0';
        val <<= 3;
    }
}

void KD11ODT::inputError ()
{
    state = ODT_STATE_WR;
    next = ODT_STATE_WAIT;
    buf[0] = '?';
    buf[1] = '\r';
    buf[2] = '\n';
    buf[3] = '@';
    buf_r = 0;
    buf_sz = 4;
}

// ToDo: Use bus->writeByte() as characters are written?
void KD11ODT::step (QBUS* bus)
{
    /* odt */
    // KD11ODT* odt = &odt;
    switch (state)
    {
        case ODT_STATE_INIT:
            clear ();
            write ('\r');
            write ('\n');
            writeOctal (cpu_.r[7]);
            write ('\r');
            write ('\n');
            write ('@');
            next = ODT_STATE_WAIT;
            state = ODT_STATE_WR;
            break;

        case ODT_STATE_WAIT:
            if (bus->read (0177560) & 0x80)
            {
                /* ch available */
                u16 c = bus->read (0177562);
                bus->writeWord (0177566, (u8)c);
                switch ((u8)c)
                {
                    case '$':
                    case 'R':
                        state = ODT_STATE_REG;
                        break;
                    case '0':
                    case '1':
                    case '2':
                    case '3':
                    case '4':
                    case '5':
                    case '6':
                    case '7':
                        state = ODT_STATE_ADDR;
                        addr = ((u8)c) - '0';
                        break;

                    case 'G':
                        clear ();
                        write ('\r');
                        write ('\n');
                        write ('@');
                        next = ODT_STATE_WAIT;
                        state = ODT_STATE_WR;
                        break;

                    case 'P':
                        state = ODT_STATE_INIT;
                        cpu_.runState = STATE_RUN;
                        cpu_.bus_->setProcessorRunning (true);
                        trc.TRCCPUEvent (CpuEventRecordType::CPU_ODT_P, cpu_.r[7]);
                        break;

                    default:
                        inputError ();
                        break;
                }
            }
            break;

        case ODT_STATE_WR:
            if (bus->read (0177564) & 0x80)
            {
                bus->writeWord (0177566, buf[buf_r++]);
                if (buf_r == buf_sz)
                    state = next;
            }
            break;

        case ODT_STATE_ADDR:
            if (bus->read (0177560) & 0x80)
            {
                /* ch available */
                u16 ch = bus->read (0177562);
                u8 c = (u8)ch;
                bus->writeWord (0177566, c);
                if ((u8)c == '/')
                {
                    /* delimit */
                    // ToDo: The address might be invalid and in that case
                    // ODT should respond with a '?' instead of a zero.
                    u16 val = bus->read (addr).valueOr (0);
                    clear ();
                    writeOctal (val);
                    write (' ');
                    next = ODT_STATE_VAL;
                    state = ODT_STATE_WR;
                    input = 0;
                }
                else if (c >= '0' && c <= '7')
                {
                    addr <<= 3;
                    addr |= c - '0';
                }
                else if (c == 'G')
                {
                    state = ODT_STATE_INIT;
                    cpu_.r[7] = addr;
                    cpu_.runState = STATE_RUN;
                    cpu_.bus_->setProcessorRunning (true);
                    trc.TRCCPUEvent (CpuEventRecordType::CPU_ODT_G, addr);
                }
                else
                    inputError ();
            }
            break;

        case ODT_STATE_REG:
            if (bus->read (0177560) & 0x80)
            {
                /* ch available */
                u16 ch = bus->read (0177562);
                u8 c = (u8)ch;
                bus->writeWord (0177566, c);
                state = ODT_STATE_REG_WAIT;
                if (c >= '0' && c <= '7')
                    addr = c - '0';
                else if (c == 'S')
                    addr = 8;
                else
                    inputError ();
            }
            break;

        case ODT_STATE_VAL:
            if (bus->read (0177560) & 0x80)
            {
                /* ch available */
                u16 ch = bus->read (0177562);
                u8 c = (u8)ch;
                bus->writeWord (0177566, c);
                if (c == '\r' || c == '\n')
                {
                    if (input)
                        bus->writeWord (addr, val);
                }
                else if (c >= '0' && c <= '7')
                {
                    val <<= 3;
                    val |= c - '0';
                    input = 1;
                }
                else
                    inputError ();

                if (c == '\r')
                {
                    clear ();
                    write ('\r');
                    write ('\n');
                    write ('@');
                    state = ODT_STATE_WR;
                    next = ODT_STATE_WAIT;
                }
                else if (c == '\n')
                {
                    u16 tmpValue;

                    addr += 2;
                    val = 0;
                    tmpValue = bus->read (addr);

                    clear ();
                    write ('\r');
                    write ('\n');
                    writeOctal (addr);
                    write ('/');
                    writeOctal (tmpValue);
                    write (' ');

                    next = ODT_STATE_VAL;
                    state = ODT_STATE_WR;
                    input = 0;
                }
            }
            break;

        case ODT_STATE_REG_WAIT:
            if (bus->read (0177560) & 0x80)
            {
                /* ch available */
                u16 ch = bus->read (0177562);
                u8 c = (u8)ch;
                bus->writeWord (0177566, c);
                if (c == '/')
                {
                    u16 val;
                    if (addr < 8)
                        val = cpu_.r[addr];
                    else
                        val = cpu_.psw;

                    clear ();
                    writeOctal (val);
                    write (' ');
                    val = 0;
                    next = ODT_STATE_REG_VAL;
                    state = ODT_STATE_WR;
                    input = 0;
                }
                else
                    inputError ();
            }
            break;

        case ODT_STATE_REG_VAL:
            if (bus->read (0177560) & 0x80)
            {
                /* ch available */
                u16 ch = bus->read (0177562);
                u8 c = (u8)ch;
                bus->writeWord (0177566, c);
                if (c == '\r' || c == '\n')
                {
                    if (input)
                    {
                        if (addr == 8)
                            cpu_.psw = val;
                        else
                            cpu_.r[addr] = val;
                    }
                }
                else if (c >= '0' && c <= '7')
                {
                    val <<= 3;
                    val |= c - '0';
                    input = 1;
                }
                else
                    inputError ();

                if (c == '\r' || (c == '\n' && addr == 7))
                {
                    clear ();
                    write ('\r');
                    write ('\n');
                    write ('@');
                    state = ODT_STATE_WR;
                    next = ODT_STATE_WAIT;
                }
                else if (c == '\n')
                {
                    u16 tmpValue;

                    addr++;
                    val = 0;
                    tmpValue = cpu_.r[addr];

                    clear ();
                    write ('\r');
                    write ('R');
                    write (addr + '0');
                    write ('/');
                    writeOctal (tmpValue);
                    write (' ');

                    next = ODT_STATE_REG_VAL;
                    state = ODT_STATE_WR;
                    input = 0;
                }
            }
            break;
    }
}