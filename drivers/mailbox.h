#pragma once

#include <stdint.h>

// addresses defined in https://github.com/raspberrypi/firmware/wiki/Mailboxes
// mailbox base is 0x2000B880
#define MAILBOX_READ0 (volatile uint32_t *)0x2000B880
#define MAILBOX_PEEK0 (volatile uint32_t *)0x2000B890
#define MAILBOX_SENDER0 (volatile uint32_t *)0x2000B894
#define MAILBOX_STATUS0 (volatile uint32_t *)0x2000B898
#define MAILBOX_CONFIG0 (volatile uint32_t *)0x2000B89C

#define MAILBOX_WRITE1 (volatile uint32_t *)0x2000B8A0
#define MAILBOX_PEEK1 (volatile uint32_t *)0x2000B8B0
#define MAILBOX_SENDER1 (volatile uint32_t *)0x2000B8B4
#define MAILBOX_STATUS1 (volatile uint32_t *)0x2000B8B8
#define MAILBOX_CONFIG1 (volatile uint32_t *)0x2000B8BC

#define MAILBOX_CHANNEL_POWER 0
#define MAILBOX_CHANNEL_FRAMEBUFFER 1
#define MAILBOX_CHANNEL_VIRTUAL_UART 2
#define MAILBOX_CHANNEL_VCHIQ 3
#define MAILBOX_CHANNEL_LEDS 4
#define MAILBOX_CHANNEL_BUTTONS 5
#define MAILBOX_CHANNEL_TOUCHSCREEN 6
#define MAILBOX_CHANNEL_TAGS_ARM2VC 8
#define MAILBOX_CHANNEL_TAGS_VC2ARM 9

#define MAILBOX_STATUS_FULL 0x80000000
#define MAILBOX_STATUS_EMPTY 0x40000000

#define MAILBOX_REQUEST 0
#define MAILBOX_RESPONSE 0x80000000
#define MAILBOX_ERROR 0x80000001

// list of tags
#define MAILBOX_TAG_FIRMWARE_REV 0x00000001
#define MAILBOX_TAG_BOARD_MODEL 0x00010001
#define MAILBOX_TAG_BOARD_REV 0x00010002
#define MAILBOX_TAG_BOARD_MAC 0x00010003
#define MAILBOX_TAG_BOARD_SERIAL 0x00010004
#define MAILBOX_TAG_END 0x0

extern volatile uint32_t mailbox_buffer[36];

int mailbox_call(uint8_t channel);
