/**
 * \file
 *
 * \brief USB Device Human Interface Device (HID) keyboard interface.
 *
 * Copyright (c) 2009-2015 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include "samd51j18a.h"
#include "d51_util.h"
#include "conf_usb.h"
#include "usb_protocol.h"
#include "udd.h"
#include "udc.h"
#include "udi_device_conf.h"
#include "udi_hid.h"
#include "udi_hid_kbd.h"
#include <string.h>
#include "report.h"

//***************************************************************************
// KBD
//***************************************************************************
#ifdef KBD

bool    udi_hid_kbd_enable(void);
void    udi_hid_kbd_disable(void);
bool    udi_hid_kbd_setup(void);
uint8_t udi_hid_kbd_getsetting(void);

UDC_DESC_STORAGE udi_api_t udi_api_hid_kbd = {
    .enable     = (bool (*)(void))udi_hid_kbd_enable,
    .disable    = (void (*)(void))udi_hid_kbd_disable,
    .setup      = (bool (*)(void))udi_hid_kbd_setup,
    .getsetting = (uint8_t(*)(void))udi_hid_kbd_getsetting,
    .sof_notify = NULL,
};

COMPILER_WORD_ALIGNED
static uint8_t udi_hid_kbd_rate;

COMPILER_WORD_ALIGNED
static uint8_t udi_hid_kbd_protocol;

COMPILER_WORD_ALIGNED
uint8_t udi_hid_kbd_report_set;

bool udi_hid_kbd_b_report_valid;

COMPILER_WORD_ALIGNED
uint8_t udi_hid_kbd_report[UDI_HID_KBD_REPORT_SIZE];

volatile bool udi_hid_kbd_b_report_trans_ongoing;

COMPILER_WORD_ALIGNED
static uint8_t udi_hid_kbd_report_trans[UDI_HID_KBD_REPORT_SIZE];

COMPILER_WORD_ALIGNED
UDC_DESC_STORAGE udi_hid_kbd_report_desc_t udi_hid_kbd_report_desc = {{
    0x05, 0x01,  // Usage Page (Generic Desktop)
    0x09, 0x06,  // Usage (Keyboard)
    0xA1, 0x01,  // Collection (Application)
    0x05, 0x07,  //   Usage Page (Keyboard)
    0x19, 0xE0,  //   Usage Minimum (224)
    0x29, 0xE7,  //   Usage Maximum (231)
    0x15, 0x00,  //   Logical Minimum (0)
    0x25, 0x01,  //   Logical Maximum (1)
    0x75, 0x01,  //   Report Size (1)
    0x95, 0x08,  //   Report Count (8)
    0x81, 0x02,  //   Input (Data, Variable, Absolute)
    0x81, 0x01,  //   Input (Constant)
    0x19, 0x00,  //   Usage Minimum (0)
    0x29, 0xFF,  //   Usage Maximum (255)
    0x15, 0x00,  //   Logical Minimum (0)
    0x25, 0xFF,  //   Logical Maximum (255)
    0x75, 0x08,  //   Report Size (8)
    0x95, 0x06,  //   Report Count (6)
    0x81, 0x00,  //   Input (Data, Array)
    0x05, 0x08,  //   Usage Page (LED)
    0x19, 0x01,  //   Usage Minimum (1)
    0x29, 0x05,  //   Usage Maximum (5)
    0x15, 0x00,  //   Logical Minimum (0)
    0x25, 0x01,  //   Logical Maximum (1)
    0x75, 0x01,  //   Report Size (1)
    0x95, 0x05,  //   Report Count (5)
    0x91, 0x02,  //   Output (Data, Variable, Absolute)
    0x95, 0x03,  //   Report Count (3)
    0x91, 0x01,  //   Output (Constant)
    0xC0         // End Collection
}};

static bool udi_hid_kbd_setreport(void);

static void udi_hid_kbd_report_sent(udd_ep_status_t status, iram_size_t nb_sent, udd_ep_id_t ep);

static void udi_hid_kbd_setreport_valid(void);

bool udi_hid_kbd_enable(void) {
    // Initialize internal values
    udi_hid_kbd_rate                   = 0;
    udi_hid_kbd_protocol               = 0;
    udi_hid_kbd_b_report_trans_ongoing = false;
    memset(udi_hid_kbd_report, 0, UDI_HID_KBD_REPORT_SIZE);
    udi_hid_kbd_b_report_valid = false;
    return UDI_HID_KBD_ENABLE_EXT();
}

void udi_hid_kbd_disable(void) { UDI_HID_KBD_DISABLE_EXT(); }

bool udi_hid_kbd_setup(void) { return udi_hid_setup(&udi_hid_kbd_rate, &udi_hid_kbd_protocol, (uint8_t *)&udi_hid_kbd_report_desc, udi_hid_kbd_setreport); }

uint8_t udi_hid_kbd_getsetting(void) { return 0; }

static bool udi_hid_kbd_setreport(void) {
    if ((USB_HID_REPORT_TYPE_OUTPUT == (udd_g_ctrlreq.req.wValue >> 8)) && (0 == (0xFF & udd_g_ctrlreq.req.wValue)) && (1 == udd_g_ctrlreq.req.wLength)) {
        // Report OUT type on report ID 0 from USB Host
        udd_g_ctrlreq.payload      = &udi_hid_kbd_report_set;
        udd_g_ctrlreq.callback     = udi_hid_kbd_setreport_valid;
        udd_g_ctrlreq.payload_size = 1;
        return true;
    }
    return false;
}

bool udi_hid_kbd_send_report(void) {
    if (!main_b_kbd_enable) {
        return false;
    }

    if (udi_hid_kbd_b_report_trans_ongoing) {
        return false;
    }

    memcpy(udi_hid_kbd_report_trans, udi_hid_kbd_report, UDI_HID_KBD_REPORT_SIZE);
    udi_hid_kbd_b_report_valid         = false;
    udi_hid_kbd_b_report_trans_ongoing = udd_ep_run(UDI_HID_KBD_EP_IN | USB_EP_DIR_IN, false, udi_hid_kbd_report_trans, UDI_HID_KBD_REPORT_SIZE, udi_hid_kbd_report_sent);

    return udi_hid_kbd_b_report_trans_ongoing;
}

static void udi_hid_kbd_report_sent(udd_ep_status_t status, iram_size_t nb_sent, udd_ep_id_t ep) {
    UNUSED(status);
    UNUSED(nb_sent);
    UNUSED(ep);
    udi_hid_kbd_b_report_trans_ongoing = false;
    if (udi_hid_kbd_b_report_valid) {
        udi_hid_kbd_send_report();
    }
}

static void udi_hid_kbd_setreport_valid(void) {
    // UDI_HID_KBD_CHANGE_LED(udi_hid_kbd_report_set);
}

#endif  // KBD

//********************************************************************************************
// NKRO Keyboard
//********************************************************************************************
#ifdef NKRO

bool    udi_hid_nkro_enable(void);
void    udi_hid_nkro_disable(void);
bool    udi_hid_nkro_setup(void);
uint8_t udi_hid_nkro_getsetting(void);

UDC_DESC_STORAGE udi_api_t udi_api_hid_nkro = {
    .enable     = (bool (*)(void))udi_hid_nkro_enable,
    .disable    = (void (*)(void))udi_hid_nkro_disable,
    .setup      = (bool (*)(void))udi_hid_nkro_setup,
    .getsetting = (uint8_t(*)(void))udi_hid_nkro_getsetting,
    .sof_notify = NULL,
};

COMPILER_WORD_ALIGNED
static uint8_t udi_hid_nkro_rate;

COMPILER_WORD_ALIGNED
static uint8_t udi_hid_nkro_protocol;

COMPILER_WORD_ALIGNED
uint8_t udi_hid_nkro_report_set;

bool udi_hid_nkro_b_report_valid;

COMPILER_WORD_ALIGNED
uint8_t udi_hid_nkro_report[UDI_HID_NKRO_REPORT_SIZE];

volatile bool udi_hid_nkro_b_report_trans_ongoing;

COMPILER_WORD_ALIGNED
static uint8_t udi_hid_nkro_report_trans[UDI_HID_NKRO_REPORT_SIZE];

COMPILER_WORD_ALIGNED
UDC_DESC_STORAGE udi_hid_nkro_report_desc_t udi_hid_nkro_report_desc = {{
    0x05, 0x01,  // Usage Page (Generic Desktop),
    0x09, 0x06,  // Usage (Keyboard),
    0xA1, 0x01,  // Collection (Application) - Keyboard,

    // Mods
    0x75, 0x01,  //   Report Size (1),
    0x95, 0x08,  //   Report Count (8),
    0x15, 0x00,  //   Logical Minimum (0),
    0x25, 0x01,  //   Logical Maximum (1),
    0x05, 0x07,  //   Usage Page (Key Codes),
    0x19, 0xE0,  //   Usage Minimum (224),
    0x29, 0xE7,  //   Usage Maximum (231),
    0x81, 0x02,  //   Input (Data, Variable, Absolute),

    // LED Report
    0x75, 0x01,  //   Report Size (1),
    0x95, 0x05,  //   Report Count (5),
    0x05, 0x08,  //   Usage Page (LEDs),
    0x19, 0x01,  //   Usage Minimum (1),
    0x29, 0x05,  //   Usage Maximum (5),
    0x91, 0x02,  //   Output (Data, Variable, Absolute),

    // LED Report Padding
    0x75, 0x03,  //   Report Size (3),
    0x95, 0x01,  //   Report Count (1),
    0x91, 0x03,  //   Output (Constant),

    // Main keys
    0x75, 0x01,  //   Report Size (1),
    0x95, 0xF8,  //   Report Count (248),
    0x15, 0x00,  //   Logical Minimum (0),
    0x25, 0x01,  //   Logical Maximum (1),
    0x05, 0x07,  //   Usage Page (Key Codes),
    0x19, 0x00,  //   Usage Minimum (0),
    0x29, 0xF7,  //   Usage Maximum (247),
    0x81, 0x02,  //   Input (Data, Variable, Absolute, Bitfield),
    0xc0,        // End Collection - Keyboard
}};

static bool udi_hid_nkro_setreport(void);
static void udi_hid_nkro_setreport_valid(void);
static void udi_hid_nkro_report_sent(udd_ep_status_t status, iram_size_t nb_sent, udd_ep_id_t ep);

bool udi_hid_nkro_enable(void) {
    // Initialize internal values
    udi_hid_nkro_rate                   = 0;
    udi_hid_nkro_protocol               = 0;
    udi_hid_nkro_b_report_trans_ongoing = false;
    memset(udi_hid_nkro_report, 0, UDI_HID_NKRO_REPORT_SIZE);
    udi_hid_nkro_b_report_valid = false;
    return UDI_HID_NKRO_ENABLE_EXT();
}

void udi_hid_nkro_disable(void) { UDI_HID_NKRO_DISABLE_EXT(); }

bool udi_hid_nkro_setup(void) { return udi_hid_setup(&udi_hid_nkro_rate, &udi_hid_nkro_protocol, (uint8_t *)&udi_hid_nkro_report_desc, udi_hid_nkro_setreport); }

uint8_t udi_hid_nkro_getsetting(void) { return 0; }

// keyboard receives LED report here
static bool udi_hid_nkro_setreport(void) {
    if ((USB_HID_REPORT_TYPE_OUTPUT == (udd_g_ctrlreq.req.wValue >> 8)) && (0 == (0xFF & udd_g_ctrlreq.req.wValue)) && (1 == udd_g_ctrlreq.req.wLength)) {
        // Report OUT type on report ID 0 from USB Host
        udd_g_ctrlreq.payload      = &udi_hid_nkro_report_set;
        udd_g_ctrlreq.callback     = udi_hid_nkro_setreport_valid;  // must call routine to transform setreport to LED state
        udd_g_ctrlreq.payload_size = 1;
        return true;
    }
    return false;
}

bool udi_hid_nkro_send_report(void) {
    if (!main_b_nkro_enable) {
        return false;
    }

    if (udi_hid_nkro_b_report_trans_ongoing) {
        return false;
    }

    memcpy(udi_hid_nkro_report_trans, udi_hid_nkro_report, UDI_HID_NKRO_REPORT_SIZE);
    udi_hid_nkro_b_report_valid         = false;
    udi_hid_nkro_b_report_trans_ongoing = udd_ep_run(UDI_HID_NKRO_EP_IN | USB_EP_DIR_IN, false, udi_hid_nkro_report_trans, UDI_HID_NKRO_REPORT_SIZE, udi_hid_nkro_report_sent);

    return udi_hid_nkro_b_report_trans_ongoing;
}

static void udi_hid_nkro_report_sent(udd_ep_status_t status, iram_size_t nb_sent, udd_ep_id_t ep) {
    UNUSED(status);
    UNUSED(nb_sent);
    UNUSED(ep);
    udi_hid_nkro_b_report_trans_ongoing = false;
    if (udi_hid_nkro_b_report_valid) {
        udi_hid_nkro_send_report();
    }
}

static void udi_hid_nkro_setreport_valid(void) {
    // UDI_HID_NKRO_CHANGE_LED(udi_hid_nkro_report_set);
}

#endif  // NKRO

//********************************************************************************************
// EXK (extra-keys) SYS-CTRL  Keyboard
//********************************************************************************************
#ifdef EXK

bool    udi_hid_exk_enable(void);
void    udi_hid_exk_disable(void);
bool    udi_hid_exk_setup(void);
uint8_t udi_hid_exk_getsetting(void);

UDC_DESC_STORAGE udi_api_t udi_api_hid_exk = {
    .enable     = (bool (*)(void))udi_hid_exk_enable,
    .disable    = (void (*)(void))udi_hid_exk_disable,
    .setup      = (bool (*)(void))udi_hid_exk_setup,
    .getsetting = (uint8_t(*)(void))udi_hid_exk_getsetting,
    .sof_notify = NULL,
};

COMPILER_WORD_ALIGNED
static uint8_t udi_hid_exk_rate;

COMPILER_WORD_ALIGNED
static uint8_t udi_hid_exk_protocol;

COMPILER_WORD_ALIGNED
uint8_t udi_hid_exk_report_set;

bool udi_hid_exk_b_report_valid;

COMPILER_WORD_ALIGNED
udi_hid_exk_report_t udi_hid_exk_report;

static bool udi_hid_exk_b_report_trans_ongoing;

COMPILER_WORD_ALIGNED
static uint8_t udi_hid_exk_report_trans[UDI_HID_EXK_REPORT_SIZE];

COMPILER_WORD_ALIGNED
UDC_DESC_STORAGE udi_hid_exk_report_desc_t udi_hid_exk_report_desc = {{
    // System Control Collection (8 bits)

    0x05, 0x01,              // Usage Page (Generic Desktop),
    0x09, 0x80,              // Usage (System Control),
    0xA1, 0x01,              // Collection (Application),
    0x85, REPORT_ID_SYSTEM,  //   Report ID (2) (System),
    0x16, 0x01, 0x00,        //   Logical Minimum (1),
    0x26, 0x03, 0x00,        //   Logical Maximum (3),
    0x1A, 0x81, 0x00,        //   Usage Minimum (81) (System Power Down),
    0x2A, 0x83, 0x00,        //   Usage Maximum (83) (System Wake Up),
    0x75, 0x10,              //   Report Size (16),
    0x95, 0x01,              //   Report Count (1),
    0x81, 0x00,              //   Input (Data, Array),
    0xC0,                    // End Collection - System Control

    // Consumer Control Collection - Media Keys (16 bits)

    0x05, 0x0C,                // Usage Page (Consumer),
    0x09, 0x01,                // Usage (Consumer Control),
    0xA1, 0x01,                // Collection (Application),
    0x85, REPORT_ID_CONSUMER,  //   Report ID (3) (Consumer),
    0x16, 0x01, 0x00,          //   Logical Minimum (1),
    0x26, 0x9C, 0x02,          //   Logical Maximum (668),
    0x1A, 0x01, 0x00,          //   Usage Minimum (1),
    0x2A, 0x9C, 0x02,          //   Usage Maximum (668),
    0x75, 0x10,                //   Report Size (16),
    0x95, 0x01,                //   Report Count (1),
    0x81, 0x00,                //   Input (Data, Array),
    0xC0,                      // End Collection - Consumer Control
}};

static bool udi_hid_exk_setreport(void);

static void udi_hid_exk_report_sent(udd_ep_status_t status, iram_size_t nb_sent, udd_ep_id_t ep);

static void udi_hid_exk_setreport_valid(void);

bool udi_hid_exk_enable(void) {
    // Initialize internal values
    udi_hid_exk_rate                   = 0;
    udi_hid_exk_protocol               = 0;
    udi_hid_exk_b_report_trans_ongoing = false;
    memset(udi_hid_exk_report.raw, 0, UDI_HID_EXK_REPORT_SIZE);
    udi_hid_exk_b_report_valid = false;
    return UDI_HID_EXK_ENABLE_EXT();
}

void udi_hid_exk_disable(void) { UDI_HID_EXK_DISABLE_EXT(); }

bool udi_hid_exk_setup(void) { return udi_hid_setup(&udi_hid_exk_rate, &udi_hid_exk_protocol, (uint8_t *)&udi_hid_exk_report_desc, udi_hid_exk_setreport); }

uint8_t udi_hid_exk_getsetting(void) { return 0; }

static bool udi_hid_exk_setreport(void) {
    if ((USB_HID_REPORT_TYPE_OUTPUT == (udd_g_ctrlreq.req.wValue >> 8)) && (0 == (0xFF & udd_g_ctrlreq.req.wValue)) && (1 == udd_g_ctrlreq.req.wLength)) {
        // Report OUT type on report ID 0 from USB Host
        udd_g_ctrlreq.payload      = &udi_hid_exk_report_set;
        udd_g_ctrlreq.callback     = udi_hid_exk_setreport_valid;
        udd_g_ctrlreq.payload_size = 1;
        return true;
    }
    return false;
}

bool udi_hid_exk_send_report(void) {
    if (!main_b_exk_enable) {
        return false;
    }

    if (udi_hid_exk_b_report_trans_ongoing) {
        return false;
    }

    memcpy(udi_hid_exk_report_trans, udi_hid_exk_report.raw, UDI_HID_EXK_REPORT_SIZE);
    udi_hid_exk_b_report_valid         = false;
    udi_hid_exk_b_report_trans_ongoing = udd_ep_run(UDI_HID_EXK_EP_IN | USB_EP_DIR_IN, false, udi_hid_exk_report_trans, UDI_HID_EXK_REPORT_SIZE, udi_hid_exk_report_sent);

    return udi_hid_exk_b_report_trans_ongoing;
}

static void udi_hid_exk_report_sent(udd_ep_status_t status, iram_size_t nb_sent, udd_ep_id_t ep) {
    UNUSED(status);
    UNUSED(nb_sent);
    UNUSED(ep);
    udi_hid_exk_b_report_trans_ongoing = false;
    if (udi_hid_exk_b_report_valid) {
        udi_hid_exk_send_report();
    }
}

static void udi_hid_exk_setreport_valid(void) {}

#endif  // EXK

//********************************************************************************************
// MOU Mouse
//********************************************************************************************
#ifdef MOU

bool    udi_hid_mou_enable(void);
void    udi_hid_mou_disable(void);
bool    udi_hid_mou_setup(void);
uint8_t udi_hid_mou_getsetting(void);

UDC_DESC_STORAGE udi_api_t udi_api_hid_mou = {
    .enable     = (bool (*)(void))udi_hid_mou_enable,
    .disable    = (void (*)(void))udi_hid_mou_disable,
    .setup      = (bool (*)(void))udi_hid_mou_setup,
    .getsetting = (uint8_t(*)(void))udi_hid_mou_getsetting,
    .sof_notify = NULL,
};

COMPILER_WORD_ALIGNED
static uint8_t udi_hid_mou_rate;

COMPILER_WORD_ALIGNED
static uint8_t udi_hid_mou_protocol;

// COMPILER_WORD_ALIGNED
// uint8_t udi_hid_mou_report_set; //No set report

bool udi_hid_mou_b_report_valid;

COMPILER_WORD_ALIGNED
uint8_t udi_hid_mou_report[UDI_HID_MOU_REPORT_SIZE];

static bool udi_hid_mou_b_report_trans_ongoing;

COMPILER_WORD_ALIGNED
static uint8_t udi_hid_mou_report_trans[UDI_HID_MOU_REPORT_SIZE];

COMPILER_WORD_ALIGNED
UDC_DESC_STORAGE udi_hid_mou_report_desc_t udi_hid_mou_report_desc = {{
    0x05, 0x01,  // Usage Page (Generic Desktop),
    0x09, 0x02,  // Usage (Mouse),
    0xA1, 0x01,  // Collection (Application),
    0x09, 0x01,  //   Usage (Pointer),
    0xA1, 0x00,  //   Collection (Physical),
    0x05, 0x09,  //     Usage Page (Buttons),
    0x19, 0x01,  //     Usage Minimum (01),
    0x29, 0x05,  //     Usage Maximun (05),
    0x15, 0x00,  //     Logical Minimum (0),
    0x25, 0x01,  //     Logical Maximum (1),
    0x95, 0x05,  //     Report Count (5),
    0x75, 0x01,  //     Report Size (1),
    0x81, 0x02,  //     Input (Data, Variable, Absolute), ;5 button bits
    0x95, 0x01,  //     Report Count (1),
    0x75, 0x03,  //     Report Size (3),
    0x81, 0x01,  //     Input (Constant), ;3 bit padding,

    0x05, 0x01,  //     Usage Page (Generic Desktop),
    0x09, 0x30,  //     Usage (X),
    0x09, 0x31,  //     Usage (Y),
    0x15, 0x81,  //     Logical Minimum (-127),
    0x25, 0x7F,  //     Logical Maximum (127),
    0x95, 0x02,  //     Report Count (2),
    0x75, 0x08,  //     Report Size (8),
    0x81, 0x06,  //     Input (Data, Variable, Relative), ;2 position bytes (X & Y),

    0x09, 0x38,  //     Usage (Wheel),
    0x15, 0x81,  //     Logical Minimum (-127),
    0x25, 0x7F,  //     Logical Maximum (127),
    0x95, 0x01,  //     Report Count (1),
    0x75, 0x08,  //     Report Size (8),
    0x81, 0x06,  //     Input (Data, Variable, Relative),

    0x05, 0x0C,        //     Usage Page (Consumer),
    0x0A, 0x38, 0x02,  //     Usage (AC Pan (Horizontal wheel)),
    0x15, 0x81,        //     Logical Minimum (-127),
    0x25, 0x7F,        //     Logical Maximum (127),
    0x95, 0x01,        //     Report Count (1),
    0x75, 0x08,        //     Report Size (8),
    0x81, 0x06,        //     Input (Data, Variable, Relative),

    0xC0,  //   End Collection,
    0xC0,  // End Collection
}};

static void udi_hid_mou_report_sent(udd_ep_status_t status, iram_size_t nb_sent, udd_ep_id_t ep);

bool udi_hid_mou_enable(void) {
    // Initialize internal values
    udi_hid_mou_rate                   = 0;
    udi_hid_mou_protocol               = 0;
    udi_hid_mou_b_report_trans_ongoing = false;
    memset(udi_hid_mou_report, 0, UDI_HID_MOU_REPORT_SIZE);
    udi_hid_mou_b_report_valid = false;
    return UDI_HID_MOU_ENABLE_EXT();
}

void udi_hid_mou_disable(void) { UDI_HID_MOU_DISABLE_EXT(); }

bool udi_hid_mou_setup(void) { return udi_hid_setup(&udi_hid_mou_rate, &udi_hid_mou_protocol, (uint8_t *)&udi_hid_mou_report_desc, NULL); }

uint8_t udi_hid_mou_getsetting(void) { return 0; }

bool udi_hid_mou_send_report(void) {
    if (!main_b_mou_enable) {
        return false;
    }

    if (udi_hid_mou_b_report_trans_ongoing) {
        return false;
    }

    memcpy(udi_hid_mou_report_trans, udi_hid_mou_report, UDI_HID_MOU_REPORT_SIZE);
    udi_hid_mou_b_report_valid         = false;
    udi_hid_mou_b_report_trans_ongoing = udd_ep_run(UDI_HID_MOU_EP_IN | USB_EP_DIR_IN, false, udi_hid_mou_report_trans, UDI_HID_MOU_REPORT_SIZE, udi_hid_mou_report_sent);

    return udi_hid_mou_b_report_trans_ongoing;
}

static void udi_hid_mou_report_sent(udd_ep_status_t status, iram_size_t nb_sent, udd_ep_id_t ep) {
    UNUSED(status);
    UNUSED(nb_sent);
    UNUSED(ep);
    udi_hid_mou_b_report_trans_ongoing = false;
    if (udi_hid_mou_b_report_valid) {
        udi_hid_mou_send_report();
    }
}

#endif  // MOU

//********************************************************************************************
// RAW
//********************************************************************************************
#ifdef RAW

bool    udi_hid_raw_enable(void);
void    udi_hid_raw_disable(void);
bool    udi_hid_raw_setup(void);
uint8_t udi_hid_raw_getsetting(void);

UDC_DESC_STORAGE udi_api_t udi_api_hid_raw = {
    .enable     = (bool (*)(void))udi_hid_raw_enable,
    .disable    = (void (*)(void))udi_hid_raw_disable,
    .setup      = (bool (*)(void))udi_hid_raw_setup,
    .getsetting = (uint8_t(*)(void))udi_hid_raw_getsetting,
    .sof_notify = NULL,
};

COMPILER_WORD_ALIGNED
static uint8_t udi_hid_raw_rate;

COMPILER_WORD_ALIGNED
static uint8_t udi_hid_raw_protocol;

COMPILER_WORD_ALIGNED
uint8_t udi_hid_raw_report_set[UDI_HID_RAW_REPORT_SIZE];

//! Report to send
COMPILER_WORD_ALIGNED
uint8_t udi_hid_raw_report[UDI_HID_RAW_REPORT_SIZE];

//! Report to receive
COMPILER_WORD_ALIGNED
static uint8_t udi_hid_raw_report_out[UDI_HID_RAW_REPORT_SIZE];

//! Report to receive, copy for App
COMPILER_WORD_ALIGNED
static uint8_t udi_hid_raw_report_out_cp[UDI_HID_RAW_REPORT_SIZE];

volatile bool udi_hid_raw_b_report_trans_ongoing;

COMPILER_WORD_ALIGNED
static uint8_t udi_hid_raw_report_trans[UDI_HID_RAW_REPORT_SIZE];

COMPILER_WORD_ALIGNED
UDC_DESC_STORAGE udi_hid_raw_report_desc_t udi_hid_raw_report_desc = {{
    0x06, 0xBC, 0xFF,		// Usage Page (Drop mfgr specific)
    0x0A, 0x34, 0x02,		// Usage  (Drop mfgr specific)
	0xA1, 0x01,  			// Collection (Application)
    0x75, 0x08,              //   Report Size (8)
    0x15, 0x00,              //   Logical Minimum (0)
    0x26, 0xFF, 0x00,        //   Logical Maximum (0x00FF) = 255
    0x95, 0x40,              //     Report Count
    0x09, 0x01,              //     Usage (Input)
    0x81, 0x02,              //     Input (Data)
    0x95, 0x40,              //     Report Count
    0x09, 0x02,              //     Usage (Output)
    0x91, 0x02,              //     Output (Data)
    0xC0,                    // End Collection - Consumer Control
}};

static bool udi_hid_raw_setreport(void);
static void udi_hid_raw_setreport_valid(void);

static void udi_hid_raw_report_sent(udd_ep_status_t status, iram_size_t nb_sent, udd_ep_id_t ep);

__attribute__((weak)) void raw_hid_receive(uint8_t *data, uint8_t length) {
    // Users should #include "raw_hid.h" in their own code
    // and implement this function there. Leave this as weak linkage
    // so users can opt to not handle data coming in.
}


/**
 * \brief Enable reception of out report
 *
 * \return \c 1 if function was successfully done, otherwise \c 0.
 */
static bool udi_hid_raw_report_out_enable(void);

bool udi_hid_raw_enable(void) {
    // Initialize internal values
    udi_hid_raw_rate                   = 0;
    udi_hid_raw_protocol               = 0;
    udi_hid_raw_b_report_trans_ongoing = false;
    memset(udi_hid_raw_report, 0, UDI_HID_RAW_REPORT_SIZE);
	if (!udi_hid_raw_report_out_enable())		// PS120919
		return false;
    return UDI_HID_RAW_ENABLE_EXT();
}

void udi_hid_raw_disable(void) { UDI_HID_RAW_DISABLE_EXT(); }

bool udi_hid_raw_setup(void) {
	return udi_hid_setup(&udi_hid_raw_rate, &udi_hid_raw_protocol, (uint8_t *)&udi_hid_raw_report_desc, udi_hid_raw_setreport);
}

uint8_t udi_hid_raw_getsetting(void) {
	return 0;
}

static bool udi_hid_raw_setreport(void) {
    if ((USB_HID_REPORT_TYPE_OUTPUT == (udd_g_ctrlreq.req.wValue >> 8)) && (0 == (0xFF & udd_g_ctrlreq.req.wValue)) && (UDI_HID_RAW_REPORT_SIZE == udd_g_ctrlreq.req.wLength)) {
        // Report OUT type on report ID 0 from USB Host (Should it be type FEATURE instead? ***TBD)
        udd_g_ctrlreq.payload      = udi_hid_raw_report_set;
        udd_g_ctrlreq.callback     = udi_hid_raw_setreport_valid;  // must call routine to transform setreport to LED state
        udd_g_ctrlreq.payload_size = UDI_HID_RAW_REPORT_SIZE;
        return true;
    }
    return false;
}

// Send IN Report
bool udi_hid_raw_send_report(uint8_t length) {
    if (!main_b_raw_enable) {
        return false;
    }

    if (udi_hid_raw_b_report_trans_ongoing) {
        return false;
    }

    memset(udi_hid_raw_report_trans, 0, UDI_HID_RAW_REPORT_SIZE);
    memcpy(udi_hid_raw_report_trans, udi_hid_raw_report, length);
    udi_hid_raw_b_report_trans_ongoing = udd_ep_run(UDI_HID_RAW_EP_IN | USB_EP_DIR_IN, false, udi_hid_raw_report_trans, UDI_HID_RAW_REPORT_SIZE, udi_hid_raw_report_sent);

    return udi_hid_raw_b_report_trans_ongoing;
}

// Send IN Report Callback
static void udi_hid_raw_report_sent(udd_ep_status_t status, iram_size_t nb_sent, udd_ep_id_t ep) {
    UNUSED(status);
    UNUSED(nb_sent);
    UNUSED(ep);
    udi_hid_raw_b_report_trans_ongoing = false;
    //if (udi_hid_raw_b_report_valid) {
    //    udi_hid_raw_send_report();		// retrigger if more data
    //}
}

static void udi_hid_raw_setreport_valid(void) {}


static void udi_hid_raw_report_out_received(udd_ep_status_t status,
		iram_size_t nb_received, udd_ep_id_t ep)
{
	UNUSED(ep);
	if (UDD_EP_TRANSFER_OK != status)
		return;	// Abort reception

	if (sizeof(udi_hid_raw_report_out) == nb_received) {
		// need to turn off IRQs?
		memcpy( udi_hid_raw_report_out_cp, udi_hid_raw_report_out, sizeof(udi_hid_raw_report_out ));	// Make a copy of report (necessary?)
		raw_hid_receive( udi_hid_raw_report_out_cp, RAW_EPSIZE );
	}
	udi_hid_raw_report_out_enable();
}

static bool udi_hid_raw_report_out_enable(void)
{
	return udd_ep_run(UDI_HID_RAW_EP_OUT,
							false,
							(uint8_t *) &udi_hid_raw_report_out,
							sizeof(udi_hid_raw_report_out),
							udi_hid_raw_report_out_received);
}

void raw_hid_send(uint8_t *data, uint8_t length) {
    uint32_t irqflags;

    if (length > RAW_EPSIZE) {
        return;
    }

	while (udi_hid_raw_b_report_trans_ongoing) {
	}  // Wait for any previous transfers to complete

	irqflags = __get_PRIMASK();
	__disable_irq();
	__DMB();

	memcpy(udi_hid_raw_report, data, length);  // Copy data into the send buffer

	//udi_hid_raw_b_report_valid = 1;  // Set report valid
	udi_hid_raw_send_report(length);       // Send report

	__DMB();
	__set_PRIMASK(irqflags);
}

#endif  // RAW

//********************************************************************************************
// CON
//********************************************************************************************
#ifdef CON

bool    udi_hid_con_enable(void);
void    udi_hid_con_disable(void);
bool    udi_hid_con_setup(void);
uint8_t udi_hid_con_getsetting(void);

UDC_DESC_STORAGE udi_api_t udi_api_hid_con = {
    .enable     = (bool (*)(void))udi_hid_con_enable,
    .disable    = (void (*)(void))udi_hid_con_disable,
    .setup      = (bool (*)(void))udi_hid_con_setup,
    .getsetting = (uint8_t(*)(void))udi_hid_con_getsetting,
    .sof_notify = NULL,
};

COMPILER_WORD_ALIGNED
static uint8_t udi_hid_con_rate;

COMPILER_WORD_ALIGNED
static uint8_t udi_hid_con_protocol;

COMPILER_WORD_ALIGNED
uint8_t udi_hid_con_report_set[UDI_HID_CON_REPORT_SIZE];

bool udi_hid_con_b_report_valid;

COMPILER_WORD_ALIGNED
uint8_t udi_hid_con_report[UDI_HID_CON_REPORT_SIZE];

volatile bool udi_hid_con_b_report_trans_ongoing;

COMPILER_WORD_ALIGNED
static uint8_t udi_hid_con_report_trans[UDI_HID_CON_REPORT_SIZE];

COMPILER_WORD_ALIGNED
UDC_DESC_STORAGE udi_hid_con_report_desc_t udi_hid_con_report_desc = {{
    0x06, 0x31,           0xFF,  // Vendor Page (PJRC Teensy compatible)
    0x09, 0x74,                  // Vendor Usage (PJRC Teensy compatible)
    0xA1, 0x01,                  // Collection (Application)
    0x09, 0x75,                  //   Usage (Vendor)
    0x15, 0x00,                  //   Logical Minimum (0x00)
    0x26, 0xFF,           0x00,  //   Logical Maximum (0x00FF)
    0x95, CONSOLE_EPSIZE,        //   Report Count
    0x75, 0x08,                  //   Report Size (8)
    0x81, 0x02,                  //   Input (Data)
    0x09, 0x76,                  //   Usage (Vendor)
    0x15, 0x00,                  //   Logical Minimum (0x00)
    0x26, 0xFF,           0x00,  //   Logical Maximum (0x00FF)
    0x95, CONSOLE_EPSIZE,        //   Report Count
    0x75, 0x08,                  //   Report Size (8)
    0x91, 0x02,                  //   Output (Data)
    0xC0,                        // End Collection
}};

static bool udi_hid_con_setreport(void);
static void udi_hid_con_setreport_valid(void);

static void udi_hid_con_report_sent(udd_ep_status_t status, iram_size_t nb_sent, udd_ep_id_t ep);

bool udi_hid_con_enable(void) {
    // Initialize internal values
    udi_hid_con_rate                   = 0;
    udi_hid_con_protocol               = 0;
    udi_hid_con_b_report_trans_ongoing = false;
    memset(udi_hid_con_report, 0, UDI_HID_CON_REPORT_SIZE);
    udi_hid_con_b_report_valid = false;
    return UDI_HID_CON_ENABLE_EXT();
}

void udi_hid_con_disable(void) { UDI_HID_CON_DISABLE_EXT(); }

bool udi_hid_con_setup(void) { return udi_hid_setup(&udi_hid_con_rate, &udi_hid_con_protocol, (uint8_t *)&udi_hid_con_report_desc, udi_hid_con_setreport); }

uint8_t udi_hid_con_getsetting(void) { return 0; }

static bool udi_hid_con_setreport(void) {
    if ((USB_HID_REPORT_TYPE_OUTPUT == (udd_g_ctrlreq.req.wValue >> 8)) && (0 == (0xFF & udd_g_ctrlreq.req.wValue)) && (UDI_HID_CON_REPORT_SIZE == udd_g_ctrlreq.req.wLength)) {
        udd_g_ctrlreq.payload      = udi_hid_con_report_set;
        udd_g_ctrlreq.callback     = udi_hid_con_setreport_valid;
        udd_g_ctrlreq.payload_size = UDI_HID_CON_REPORT_SIZE;
        return true;
    }
    return false;
}

bool udi_hid_con_send_report(void) {
    if (!main_b_con_enable) {
        return false;
    }

    if (udi_hid_con_b_report_trans_ongoing) {
        return false;
    }

    memcpy(udi_hid_con_report_trans, udi_hid_con_report, UDI_HID_CON_REPORT_SIZE);
    udi_hid_con_b_report_valid         = false;
    udi_hid_con_b_report_trans_ongoing = udd_ep_run(UDI_HID_CON_EP_IN | USB_EP_DIR_IN, false, udi_hid_con_report_trans, UDI_HID_CON_REPORT_SIZE, udi_hid_con_report_sent);

    return udi_hid_con_b_report_trans_ongoing;
}

static void udi_hid_con_report_sent(udd_ep_status_t status, iram_size_t nb_sent, udd_ep_id_t ep) {
    UNUSED(status);
    UNUSED(nb_sent);
    UNUSED(ep);
    udi_hid_con_b_report_trans_ongoing = false;
    if (udi_hid_con_b_report_valid) {
        udi_hid_con_send_report();
    }
}

static void udi_hid_con_setreport_valid(void) {}

#endif  // CON

