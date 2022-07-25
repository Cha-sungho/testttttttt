/*
 * drv_usb.c
 *
 *  Created on: 2021. 6. 10.
 *      Author: KHJ
 */

#include <stdio.h>
#include <string.h>

#include "F28x_Project.h"
#include "driverlib.h"
#include "device.h"

#include "usb_hal.h"
#include "usblib.h"
#include "usbmsc.h"
#include "host/usbhost.h"
#include "host/usbhmsc.h"
#include "fatfs/src/ff.h"

/*
 * USB + FATFS
 */
//*****************************************************************************
// Defines the size of the buffers that hold the path, or temporary data from
// the memory card.  There are two buffers allocated of this size.  The buffer
// size must be large enough to hold the longest expected full path name,
// including the file name, and a trailing null character.
//*****************************************************************************
#define PATH_BUF_SIZE   80
//*****************************************************************************
// Defines the size of the buffer that holds the command line.
//*****************************************************************************
#define CMD_BUF_SIZE    64
//*****************************************************************************
// This buffer holds the full path to the current working directory.  Initially
// it is root ("/").
//*****************************************************************************
static char g_cCwdBuf[PATH_BUF_SIZE] = "/";

// Current FAT fs state.
static FATFS g_sFatFs;
static DIR g_sDirObject;
static FILINFO g_sFileInfo;
static FIL g_sFileObject;

#define HCD_MEMORY_SIZE         512                             // The size of the host controller's memory pool in bytes.
uint8_t g_pHCDPool[HCD_MEMORY_SIZE];                            // The memory pool to provide to the Host controller driver.
tUSBHMSCInstance *g_psMSCInstance = 0;                          // The instance data for the MSC driver.
DECLARE_EVENT_DRIVER(g_sUSBEventDriver, 0, 0, USBHCDEvents);    // Declare the USB Events driver interface.

// The global that holds all of the host drivers in use in the application. In this case, only the MSC class is loaded.
static tUSBHostClassDriver const * const g_ppHostClassDrivers[] =
{
    &g_sUSBHostMSCClassDriver,
    &g_sUSBEventDriver
};

// This global holds the number of class drivers in the g_ppHostClassDrivers list.
#define NUM_CLASS_DRIVERS       (sizeof(g_ppHostClassDrivers)                 /\
                                 sizeof(g_ppHostClassDrivers[0]))
// Hold the current state for the application.
typedef enum
{
    STATE_NO_DEVICE,        // No device is present.
    STATE_DEVICE_ENUM,      // Mass storage device is being enumerated.
    STATE_DEVICE_READY,     // Mass storage device is ready.
    STATE_UNKNOWN_DEVICE,   // An unsupported device has been attached.
    STATE_POWER_FAULT       // A power fault has occurred.
} tState;
volatile tState g_eState;

tUSBMode g_eCurrentUSBMode;  // The current USB operating mode - Host, Device or unknown.

void ModeCallback(uint32_t ui32Index, tUSBMode eMode);
void MSCCallback(tUSBHMSCInstance *psMSCInstance, uint32_t ui32Event, void *pvEventData);
void USBHCDEvents(void *pvData);

void drv_InitUsb(void)
{
    SysCtl_setAuxClock(DEVICE_AUXSETCLOCK_CFG_USB);  // Set the clocking to run from the PLL at 60MHz
    // Initially wait for device connection.
    g_eState = STATE_NO_DEVICE;

    USBGPIOEnable();                                        // Configure the required pins for USB operation.
    Interrupt_register(INT_USBA, f28x_USB0HostIntHandler);  // Register the interrupt handler for USB Interrupts.
    USBStackModeSet(0, eUSBModeForceHost, ModeCallback);    // Initialize the USB stack mode and pass in a mode callback.
    USBHCDRegisterDrivers(0, g_ppHostClassDrivers, NUM_CLASS_DRIVERS);      // Register the host class drivers.
    g_psMSCInstance = USBHMSCDriveOpen(0, (tUSBHMSCCallback) MSCCallback);  // Open an instance of the mass storage class driver.
    USBHCDPowerConfigInit(0, USBHCD_VBUS_AUTO_HIGH | USBHCD_VBUS_FILTER);  // Initialize the power configuration. This sets the power enable signal to be active high and does not enable the power fault.
    USBHCDInit(0, g_pHCDPool, HCD_MEMORY_SIZE);                            // Initialize the USB controller for OTG operation with a 2ms polling rate.

    f_mount(0, &g_sFatFs);  // Initialize the file system.
}

#if 0
FRESULT drv_f_write(uint16_t *f_buf, uint16_t f_size)
{
    uint16_t f_file[128] = { 0, };
    static uint16_t f_name_cnt = 0;
    FRESULT f_ret;
    uint16_t f_write_size;

    sprintf(f_file, "0:/%d.txt\0", f_name_cnt++);

    f_ret = f_open(&g_sFileObject, f_file, FA_WRITE | FA_CREATE_ALWAYS);
    if (f_ret != FR_OK) return f_ret;
    f_ret = f_write(&g_sFileObject, f_buf, f_size, &f_write_size);
    if (f_ret != FR_OK) return f_ret;
    f_ret = f_close(&g_sFileObject);
    if (f_ret != FR_OK) return f_ret;

    if (f_size != f_write_size) f_ret = FR_RW_ERROR;

    return f_ret;
}
#endif

void drv_InitFatfs(void)
{
    uint16_t i, j;
    uint16_t f_buf[512] = { 0, };
    uint16_t f_file[128] = { 0, };
    FRESULT f_ret;
    uint16_t f_write_size;

    GPIO_setPinConfig(GPIO_35_GPIO35);
    GPIO_setDirectionMode(35, GPIO_DIR_MODE_OUT);

    while (1) {
        // See if a mass storage device has been enumerated.
        if (g_eState == STATE_DEVICE_ENUM) {
            // Take it easy on the Mass storage device if it is slow to start up after connecting.
            if (USBHMSCDriveReady(g_psMSCInstance) != 0) {
                // Wait about 100ms before attempting to check if the device is ready again.
                SysCtl_delay(SysCtl_getClock(DEVICE_OSCSRC_FREQ) / 30);
                break;
            }

            // Reset the working directory to the root.
            g_cCwdBuf[0] = '/';
            g_cCwdBuf[1] = '\0';

            // Attempt to open the directory.  Some drives take longer to start up than others, and this may fail (even though the USB device has enumerated) if it is still initializing.
            f_mount(0, &g_sFatFs);
            if (f_opendir(&g_sDirObject, g_cCwdBuf) == FR_OK) {
                g_eState = STATE_DEVICE_READY;  // The drive is fully ready, so move to that state.
            }
        }
        USBHCDMain();   // Run the main routine of the Host controller driver.
        if (g_eState == STATE_DEVICE_READY) break;
    }
#if 1

    for (i = 0; i < 1024; i++) {
        for (j = 0; j < 512; j++) {
            f_buf[j] = j + i;
        }
        GpioDataRegs.GPBSET.bit.GPIO35 = 1;
        sprintf(f_file, "0:/%d.txt\0", i);
        f_ret = f_open(&g_sFileObject, f_file, FA_WRITE | FA_CREATE_ALWAYS);
        if (f_ret != FR_OK) break;
        f_ret = f_write(&g_sFileObject, f_buf, 512, &f_write_size);
        if (f_ret != FR_OK) break;
        f_close(&g_sFileObject);
        GpioDataRegs.GPBCLEAR.bit.GPIO35 = 1;
    }
#endif
}

//******************************************************************************
// USB Mode callback
// \param ulIndex is the zero-based index of the USB controller making the callback.
// \param eMode indicates the new operating mode.
// This function is called by the USB library whenever an OTG mode change
// occurs and, if a connection has been made, informs us of whether we are to
// operate as a host or device.
// \return None.
//******************************************************************************
void ModeCallback(uint32_t ui32Index, tUSBMode eMode)
{
    g_eCurrentUSBMode = eMode;  // Save the new mode.
}

//******************************************************************************
// This is the callback from the MSC driver.
// \param ui32Instance is the driver instance which is needed when communicating with the driver.
// \param ui32Event is one of the events defined by the driver.
// \param pvData is a pointer to data passed into the initial call to register the callback.
// This function handles callback events from the MSC driver.  The only events
// currently handled are the MSC_EVENT_OPEN and MSC_EVENT_CLOSE.  This allows
// the main routine to know when an MSC device has been detected and
// enumerated and when an MSC device has been removed from the system.
// \return Returns \e true on success or \e false on failure.
//******************************************************************************
void MSCCallback(tUSBHMSCInstance *psMSCInstance, uint32_t ui32Event, void *pvEventData)
{
    // Determine the event.
    switch (ui32Event)
    {
        // Called when the device driver has successfully enumerated an MSC device.
        case MSC_EVENT_OPEN: {
            g_eState = STATE_DEVICE_ENUM;  // Proceed to the enumeration state.
            break;
        }
        // Called when the device driver has been unloaded due to error or the device is no longer present.
        case MSC_EVENT_CLOSE: {
            g_eState = STATE_NO_DEVICE;  // Go back to the "no device" state and wait for a new connection.
            break;
        }
        default: {
            break;
        }
    }
}

//******************************************************************************
// This is the generic callback from host stack.
// \param pvData is actually a pointer to a tEventInfo structure.
// This function will be called to inform the application when a USB event has
// occurred that is outside those related to the mass storage device.  At this
// point this is used to detect unsupported devices being inserted and removed.
// It is also used to inform the application when a power fault has occurred.
// This function is required when the g_USBGenericEventDriver is included in
// the host controller driver array that is passed in to the
// USBHCDRegisterDrivers() function.
// \return None.
//******************************************************************************
void USBHCDEvents(void *pvData)
{
    tEventInfo *pEventInfo;

    pEventInfo = (tEventInfo*) pvData;  // Cast this pointer to its actual type.

    switch (pEventInfo->ui32Event)
    {
        // New keyboard detected.
        case USB_EVENT_UNKNOWN_CONNECTED: {
            g_eState = STATE_UNKNOWN_DEVICE;  // An unknown device was detected.
            break;
        }
        // Keyboard has been unplugged.
        case USB_EVENT_DISCONNECTED: {
            g_eState = STATE_NO_DEVICE;  // Unknown device has been removed.
            break;
        }
        case USB_EVENT_POWER_FAULT: {
            g_eState = STATE_POWER_FAULT;  // No power means no device is present.
            break;
        }
        default: {
            break;
        }
    }
}
