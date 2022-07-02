#ifndef _DEVICE_H_
#define _DEVICE_H_

#include "types.h"

#include <string>
#include <memory>

/* Generic Device CSR bits */

#define CSR_V_GO        0                               // Go 
#define CSR_V_IE        6                               // Interrupt Enable
#define CSR_V_DONE      7                               // Done 
#define CSR_V_BUSY      11                              // Busy
#define CSR_V_ERR       15                              // Error
#define CSR_GO          (1u << CSR_V_GO)
#define CSR_IE          (1u << CSR_V_IE)
#define CSR_DONE        (1u << CSR_V_DONE)
#define CSR_BUSY        (1u << CSR_V_BUSY)
#define CSR_ERR         (1u << CSR_V_ERR)

// Definition of an abstract base class for I/O devices.
class Device
{
protected:
    std::string name_;          // Device name
    u32 baseAddress_;           // Device registers base address
    u32 vector_;                // Device vector

public:
    virtual void reset () = 0;
    std::string name() { return name_; }
};

#if 0
struct sim_device {
    
    struct sim_unit     *units;                         /* units */
    struct sim_reg      *registers;                     /* registers */
    struct sim_mtab     *modifiers;                     /* modifiers */
    uint32              numunits;                       /* #units */
    uint32              aradix;                         /* address radix */
    uint32              awidth;                         /* address width */
    uint32              aincr;                          /* addr increment */
    uint32              dradix;                         /* data radix */
    uint32              dwidth;                         /* data width */
    t_stat              (*examine)(t_value *v, t_addr a, struct sim_unit *up,
                            int32 sw);                  /* examine routine */
    t_stat              (*deposit)(t_value v, t_addr a, struct sim_unit *up,
                            int32 sw);                  /* deposit routine */
    t_stat              (*reset)(struct sim_device *dp);/* reset routine */
    t_stat              (*boot)(int32 u, struct sim_device *dp);
                                                        /* boot routine */
    t_stat              (*attach)(struct sim_unit *up, char *cp);
                                                        /* attach routine */
    t_stat              (*detach)(struct sim_unit *up); /* detach routine */
    void                *ctxt;                          /* context */
    uint32              flags;                          /* flags */
    uint32              dctrl;                          /* debug control */
    struct sim_debtab   *debflags;                      /* debug flags */
    t_stat              (*msize)(struct sim_unit *up, int32 v, char *cp, void *dp);
                                                        /* mem size routine */
    char                *lname;                         /* logical name */
    void                *help;                          /* (4.0 dummy) help routine */
    void                *attach_help;                   /* (4.0 dummy) help attach routine*/
    void                *help_context;                  /* (4.0 dummy) help context */
    void                *description;                   /* (4.0 dummy) description */
    };
#endif // 0

#endif // !_DEVICE_H_
