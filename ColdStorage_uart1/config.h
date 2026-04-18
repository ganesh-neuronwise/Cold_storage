#pragma once
#include <Arduino.h>

// ---------- ANALOG ----------
#define TEMP_CH PA_2 //or we can also use PA_3 
#define HUM_CH  PA_0 //or we can also use  PA_1

// ---------- RELAYS ----------
#define R_COMP1  PC_10
#define R_COMP2  PC_11
#define R_HF1    PC_12
#define R_HF2    PD_2
#define R_FAN1   PB_3
#define R_FAN2   PB_4
#define R_HT1    PB_5
#define R_HT2    PB_8

// ---------- CONTROL TIMINGS ----------
#define FAN_PRERUN_MS    (2UL * 60UL * 1000UL)   // 2 minutes
//#define COMP_MIN_ON_MS   (1UL * 60UL * 1000UL)   // 1 minute
//#define COMP_MIN_OFF_MS  (3UL * 60UL * 1000UL)   // 3 minutes  ✅ ADD THIS

// ---------- DIGITAL INPUT POLARITY ----------
// 1 = ACTIVE HIGH
// 0 = ACTIVE LOW

#define POL_C1_P     0
#define POL_C1_HP    0
#define POL_C1_LP    0
#define POL_C1_OL    0

#define POL_C2_P     0
#define POL_C2_HP    0
#define POL_C2_LP    0
#define POL_C2_OL    0

#define POL_HF1_P    0
#define POL_HF2_P    0

#define POL_FAN1_P   0
#define POL_FAN2_P   0

#define POL_HT1_P    0
#define POL_HT1_OL   0
#define POL_HT2_P    0
#define POL_HT2_OL   0

#define POL_DOOR     0
#define POL_PHASE    0



const uint8_t relay_pins[] = {
  R_COMP1,R_COMP2,R_HF1,R_HF2,R_FAN1,R_FAN2,R_HT1,R_HT2
};
#define RELAY_COUNT 8

// ---------- DIGITAL INPUTS ----------
#define IN_C1     PC_13
#define IN_C1_HP  PC_14
#define IN_C1_LP  PC_15
#define IN_C1_OL  PC_0

#define IN_C2     PC_2
#define IN_C2_HP  PC_3
#define IN_C2_LP  PA_6
#define IN_C2_OL  PA_7

#define IN_HT1_OL   PB_12   // real heater 1 overload input
#define IN_HT2_OL   PB_14   // real heater 2 overload input

#define IN_DOOR   PB_15
#define IN_PHASE  PC_6

// ---------- DEVICE ON/OFF FEEDBACK ("P") ----------
#define IN_FAN1_P   PB_0
#define IN_FAN2_P   PB_1

#define IN_HT1_P    PB_2
#define IN_HT2_P    PB_13

#define IN_HF1_P    PC_4
#define IN_HF2_P    PC_5

#define SHUNT_OHM 150.0   // ohms


// ---------- CONTROL TIMINGS ----------
#define FAN_PRERUN_MS   (2UL * 60UL * 1000UL)
//#define COMP_MIN_ON_MS  (1UL * 60UL * 1000UL)

// ---------- DEFAULT SETPOINT ----------
#define DEF_TEMP_SP  5.0
#define DEF_HUM_SP   70.0
