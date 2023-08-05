#pragma once

#include <stdint.h>

// bluetooth UART0 registers
#define BLUETOOTH_UART0_DR (volatile uint32_t *)0x20201000
#define BLUETOOTH_UART0_RSRECR (volatile uint32_t *)0x20201004
#define BLUETOOTH_UART0_FR (volatile uint32_t *)0x20201018
#define BLUETOOTH_UART0_ILPR (volatile uint32_t *)0x20201020
#define BLUETOOTH_UART0_IBRD (volatile uint32_t *)0x20201024
#define BLUETOOTH_UART0_FBRD (volatile uint32_t *)0x20201028
#define BLUETOOTH_UART0_LCRH (volatile uint32_t *)0x2020102c
#define BLUETOOTH_UART0_CR (volatile uint32_t *)0x20201030
#define BLUETOOTH_UART0_IFLS (volatile uint32_t *)0x20201034
#define BLUETOOTH_UART0_IMSC (volatile uint32_t *)0x20201038
#define BLUETOOTH_UART0_RIS (volatile uint32_t *)0x2020103c
#define BLUETOOTH_UART0_MIS (volatile uint32_t *)0x20201040
#define BLUETOOTH_UART0_ICR (volatile uint32_t *)0x20201044
#define BLUETOOTH_UART0_DMACR (volatile uint32_t *)0x20201048
#define BLUETOOTH_UART0_ITCR (volatile uint32_t *)0x20201080
#define BLUETOOTH_UART0_ITIP (volatile uint32_t *)0x20201084
#define BLUETOOTH_UART0_ITOP (volatile uint32_t *)0x20201088
#define BLUETOOTH_UART0_TDR (volatile uint32_t *)0x2020108c

// HCI packet types
#define BLUETOOTH_HCI_PKT_CMD 0x01
#define BLUETOOTH_HCI_PKT_ASYNC 0x02
#define BLUETOOTH_HCI_PKT_SYNC 0x03
#define BLUETOOTH_HCI_PKT_EV 0x04
#define BLUETOOTH_HCI_PKT_EXCMD 0x9

// HCI standard command opcodes (ie: (OGF << 8) | OCF)
#define BLUETOOTH_HCI_CMD_RESET 0x0c03
#define BLUETOOTH_HCI_CMD_GET_BDADDR 0x1009
#define BLUETOOTH_HCI_CMD_LE_SET_EVENT_MASK 0x2001
#define BLUETOOTH_HCI_CMD_LE_SET_ADV_PARAMS 0x2006
#define BLUETOOTH_HCI_CMD_LE_SET_ADV_DATA 0x2008
#define BLUETOOTH_HCI_CMD_LE_SET_ADV_ENABLE 0x200a

// HCI vendor-specific command opcodes (OGF is always 63)
#define BLUETOOTH_HCI_CMD_LOAD_FIRMWARE 0xfc2e

// HCI standard event codes
#define BLUETOOTH_HCI_EV_CMD_COMPLETE 0x0e
#define BLUETOOTH_HCI_EV_CMD_STATUS 0x0f

// HCI vendor-specific event codes

// Bluetooth-related constants
// Advertising types from Bluetooth core spec 5.2 Vol 4 Part E 7.8.5 (pg 2483)
#define BLUETOOTH_ADV_IND 0x00
#define BLUETOOTH_ADV_DIRECT_IND_HIGH 0x01
#define BLUETOOTH_ADV_SCAN_IND 0x02
#define BLUETOOTH_ADV_NONCONN_IND 0x03
#define BLUETOOTH_ADV_DIRECT_IND_LOW 0x04

void bluetooth_init(void);
int bluetooth_hci_command(uint16_t opcode, uint8_t *data, uint32_t input_len,
                          uint8_t *output, uint32_t output_len);
