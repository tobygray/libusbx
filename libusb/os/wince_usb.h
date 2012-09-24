/*
 * Windows CE backend for libusb 1.0
 * Copyright (C) 2011-2012 RealVNC Ltd.
 * Portions taken from Windows backend, which is
 * Copyright (C) 2009-2010 Pete Batard <pbatard@gmail.com>
 * With contributions from Michael Plante, Orin Eman et al.
 * Parts of this code adapted from libusb-win32-v1 by Stephan Meyer
 * Major code testing contribution by Xiaofan Chen
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */
#pragma once

#include <windows.h>
#include "poll_windows.h"

#define safe_free(p) do {if (p != NULL) {free((void*)p); p = NULL;}} while(0)
#define safe_strcmp(str1, str2) strcmp(((str1==NULL)?"<NULL>":str1), ((str2==NULL)?"<NULL>":str2))
#define safe_tcslen(str) ((str==NULL)?0:_tcslen(str))
#define safe_strlen(str) ((str==NULL)?0:strlen(str))
#define safe_sprintf _sntprintf

#define MAX_DEVICE_COUNT            256
#define TIMER_REQUEST_RETRY_MS      100
#define MAX_TIMER_SEMAPHORES        128
#define ERR_BUFFER_SIZE             256
#define HTAB_SIZE                   1021

/*
 * API macros - from libusb-win32 1.x
 */
#define DLL_DECLARE_PREFIXED(api, ret, prefix, name, args)    \
	typedef ret (api * __dll_##name##_t)args;                 \
	static __dll_##name##_t prefix##name = NULL

#define DLL_LOAD_PREFIXED(dll, prefix, name, ret_on_failure)  \
	do {                                                      \
		HMODULE h = GetModuleHandle(L#dll);                   \
	if (!h)                                                   \
		h = LoadLibrary(L#dll);                               \
	if (!h) {                                                 \
		if (ret_on_failure) { return LIBUSB_ERROR_NOT_FOUND; }\
		else { break; }                                       \
	}                                                         \
	prefix##name = (__dll_##name##_t)GetProcAddress(h, L#name);\
	if (prefix##name) break;                                  \
	prefix##name = (__dll_##name##_t)GetProcAddress(h, L#name L"A");\
	if (prefix##name) break;                                  \
	prefix##name = (__dll_##name##_t)GetProcAddress(h, L#name L"W");\
	if (prefix##name) break;                                  \
	if(ret_on_failure)                                        \
		return LIBUSB_ERROR_NOT_FOUND;                        \
	} while(0)

#define DLL_DECLARE(api, ret, name, args)   DLL_DECLARE_PREFIXED(api, ret, , name, args)
#define DLL_LOAD(dll, name, ret_on_failure) DLL_LOAD_PREFIXED(dll, , name, ret_on_failure)

// This is a modified dump of the types in the ceusbkwrapper.h library header
// with functions transformed into extern pointers.
//
// This backend dynamically loads ceusbkwrapper.dll and doesn't include
// ceusbkwrapper.h directly to simplify the build process. The kernel
// side wrapper driver is built using the platform image build tools,
// which makes it difficult to reference directly from the libusbx build
// system.
struct UKW_DEVICE_PRIV;
typedef struct UKW_DEVICE_PRIV *UKW_DEVICE;
typedef UKW_DEVICE *PUKW_DEVICE, *LPUKW_DEVICE;

typedef struct {
	UINT8 bLength;
	UINT8 bDescriptorType;
	UINT16 bcdUSB;
	UINT8 bDeviceClass;
	UINT8 bDeviceSubClass;
	UINT8 bDeviceProtocol;
	UINT8 bMaxPacketSize0;
	UINT16 idVendor;
	UINT16 idProduct;
	UINT16 bcdDevice;
	UINT8 iManufacturer;
	UINT8 iProduct;
	UINT8 iSerialNumber;
	UINT8 bNumConfigurations;
} UKW_DEVICE_DESCRIPTOR, *PUKW_DEVICE_DESCRIPTOR, *LPUKW_DEVICE_DESCRIPTOR;

typedef struct {
	UINT8 bmRequestType;
	UINT8 bRequest;
	UINT16 wValue;
	UINT16 wIndex;
	UINT16 wLength;
} UKW_CONTROL_HEADER, *PUKW_CONTROL_HEADER, *LPUKW_CONTROL_HEADER;

// Collection of flags which can be used when issuing transfer requests
/* Indicates that the transfer direction is 'in' */
#define UKW_TF_IN_TRANSFER        0x00000001
/* Indicates that the transfer direction is 'out' */
#define UKW_TF_OUT_TRANSFER       0x00000000
/* Specifies that the transfer should complete as soon as possible,
 * even if no OVERLAPPED structure has been provided. */
#define UKW_TF_NO_WAIT            0x00000100
/* Indicates that transfers shorter than the buffer are ok */
#define UKW_TF_SHORT_TRANSFER_OK  0x00000200
#define UKW_TF_SEND_TO_DEVICE     0x00010000
#define UKW_TF_SEND_TO_INTERFACE  0x00020000
#define UKW_TF_SEND_TO_ENDPOINT   0x00040000
/* Don't block when waiting for memory allocations */
#define UKW_TF_DONT_BLOCK_FOR_MEM 0x00080000

/* Value to use when dealing with configuration values, such as UkwGetConfigDescriptor, 
 * to specify the currently active configuration for the device. */
#define UKW_ACTIVE_CONFIGURATION -1

DLL_DECLARE(WINAPI, HANDLE, UkwOpenDriver, ());
DLL_DECLARE(WINAPI, BOOL, UkwGetDeviceList, (HANDLE, LPUKW_DEVICE, DWORD, LPDWORD));
DLL_DECLARE(WINAPI, void, UkwReleaseDeviceList, (HANDLE, LPUKW_DEVICE, DWORD));
DLL_DECLARE(WINAPI, BOOL, UkwGetDeviceAddress, (UKW_DEVICE, unsigned char*, unsigned char*, unsigned long*));
DLL_DECLARE(WINAPI, BOOL, UkwGetDeviceDescriptor, (UKW_DEVICE, LPUKW_DEVICE_DESCRIPTOR));
DLL_DECLARE(WINAPI, BOOL, UkwGetConfigDescriptor, (UKW_DEVICE, DWORD, LPVOID, DWORD, LPDWORD));
DLL_DECLARE(WINAPI, void, UkwCloseDriver, (HANDLE));
DLL_DECLARE(WINAPI, BOOL, UkwCancelTransfer, (UKW_DEVICE, LPOVERLAPPED, DWORD));
DLL_DECLARE(WINAPI, BOOL, UkwIssueControlTransfer, (UKW_DEVICE, DWORD, LPUKW_CONTROL_HEADER, LPVOID, DWORD, LPDWORD, LPOVERLAPPED));
DLL_DECLARE(WINAPI, BOOL, UkwClaimInterface, (UKW_DEVICE, DWORD));
DLL_DECLARE(WINAPI, BOOL, UkwReleaseInterface, (UKW_DEVICE, DWORD));
DLL_DECLARE(WINAPI, BOOL, UkwSetInterfaceAlternateSetting, (UKW_DEVICE, DWORD, DWORD));
DLL_DECLARE(WINAPI, BOOL, UkwClearHaltHost, (UKW_DEVICE, UCHAR));
DLL_DECLARE(WINAPI, BOOL, UkwClearHaltDevice, (UKW_DEVICE, UCHAR));
DLL_DECLARE(WINAPI, BOOL, UkwGetConfig, (UKW_DEVICE, PUCHAR));
DLL_DECLARE(WINAPI, BOOL, UkwSetConfig, (UKW_DEVICE, UCHAR));
DLL_DECLARE(WINAPI, BOOL, UkwResetDevice, (UKW_DEVICE));
DLL_DECLARE(WINAPI, BOOL, UkwKernelDriverActive, (UKW_DEVICE, DWORD, PBOOL));
DLL_DECLARE(WINAPI, BOOL, UkwAttachKernelDriver, (UKW_DEVICE, DWORD));
DLL_DECLARE(WINAPI, BOOL, UkwDetachKernelDriver, (UKW_DEVICE, DWORD));
DLL_DECLARE(WINAPI, BOOL, UkwIssueBulkTransfer, (UKW_DEVICE, DWORD, UCHAR, LPVOID, DWORD, LPDWORD, LPOVERLAPPED));
DLL_DECLARE(WINAPI, BOOL, UkwIsPipeHalted, (UKW_DEVICE, UCHAR, LPBOOL));

// Used to determine if an endpoint status really is halted on a failed transfer.
#define STATUS_HALT_FLAG 0x1

struct wince_device_priv {
	UKW_DEVICE dev;
	UKW_DEVICE_DESCRIPTOR desc;
};

struct wince_device_handle_priv {
	// This member isn't used, but only exists to avoid an empty structure
	// for private data for the device handle.
	int reserved;
};

struct wince_transfer_priv {
	struct winfd pollable_fd;
	uint8_t interface_number;
};

