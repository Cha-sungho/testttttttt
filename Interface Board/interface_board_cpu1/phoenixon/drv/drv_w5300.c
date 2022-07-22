/*
 * drv_tcpip.c
 *
 *  Created on: 2021. 6. 11.
 *      Author: KHJ
 */

#include <stdio.h>
#include <string.h>

#include "F28x_Project.h"
#include "driverlib.h"
#include "device.h"

#include "Types.h"
#include "socket.h"
#include "wizchip_conf.h"

#define IP_MAC 92
//#define RACK_NUMBER 3

uint8_t memsize[2][8] = { { 48, 4, 4, 8, 0, 0, 0, 0 }, { 48, 4, 4, 8, 0, 0, 0, 0 } };
wiz_NetInfo set_net_info;
wiz_NetInfo cmp_net_info;


void drv_InitW5300Gpio(void);
void drv_ResetW5300(void);
void drv_PrintNetInfo(void);

void drv_InitW5300Gpio(void)
{
    GPIO_setPinConfig(GPIO_14_GPIO14);
    GPIO_setDirectionMode(14, GPIO_DIR_MODE_OUT);
}

void drv_ResetW5300(void)
{
    GpioDataRegs.GPACLEAR.bit.GPIO14 = 1;
    DELAY_US(100000);
    GpioDataRegs.GPASET.bit.GPIO14 = 1;
    DELAY_US(100000);
}

void drv_InitNet(Uint16 IF_board_channel, Uint16 rack_number)
{
    uint8 ip[4] = { 192, 168, rack_number, 2+(uint8)IF_board_channel }; // 192.168.2.2~4
    uint8 gw[4] = { 192, 168, rack_number, 1 };
    uint8 sn[4] = { 255, 255, 255, 0 };
    uint8 mac[6] = { 0x00, 0x08, 0xDC, 0x00, rack_number, 2+(uint8)IF_board_channel };
    uint8 dhcp[1] = { 1 };

    drv_InitW5300Gpio();
    drv_ResetW5300();

//    ip[3]=2+(uint8)IF_board_channel;
//    mac[5]=2+(uint8)IF_board_channel;

    memcpy(set_net_info.mac, mac, 6);
    memcpy(set_net_info.ip, ip, 4);
    memcpy(set_net_info.gw, gw, 4);
    memcpy(set_net_info.sn, sn, 4);
    memcpy(&set_net_info.dhcp, dhcp, 1);

//    memcpy(cmp_net_info.mac, mac, 6);
//    memcpy(cmp_net_info.ip, ip, 4);
//    memcpy(cmp_net_info.gw, gw, 4);
//    memcpy(cmp_net_info.sn, sn, 4);
//    memcpy(&cmp_net_info.dhcp, dhcp, 1);

    /* wizchip initialize*/
    if (ctlwizchip(CW_INIT_WIZCHIP, (void*) memsize) == -1) {
        printf("wizchip init buffer error\r\n");
        while (1)
            ;
    }

    if (ctlnetwork(CN_SET_NETINFO, (void*) &set_net_info) == -1) {
        printf("wizchip init net-info error\r\n");
        while (1)
            ;
    }

    if (ctlnetwork(CN_GET_NETINFO, (void*) &cmp_net_info) == -1) {
        printf("wizchip init get-info error\r\n");
        while (1)
            ;
    }

    if (memcmp(&set_net_info, &cmp_net_info, sizeof(wiz_NetInfo)) != 0) {
        printf("wizchip init net-info fail\r\n");
    } else {
        printf("wizchip init net-info ok\r\n");
    }

    drv_PrintNetInfo();
}

void drv_PrintNetInfo(void)
{
    wiz_NetInfo net_info;

    ctlnetwork(CN_GET_NETINFO, (void*) &net_info);

    printf("MAC - 0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x\r\n", net_info.mac[0], net_info.mac[1], net_info.mac[2], net_info.mac[3], net_info.mac[4], net_info.mac[5]);
    printf("IP - %d.%d.%d.%d\r\n", net_info.ip[0], net_info.ip[1], net_info.ip[2], net_info.ip[3]);
    printf("Gate - %d.%d.%d.%d\r\n", net_info.gw[0], net_info.gw[1], net_info.gw[2], net_info.gw[3]);
    printf("Subnet - %d.%d.%d.%d\r\n", net_info.sn[0], net_info.sn[1], net_info.sn[2], net_info.sn[3]);
}
