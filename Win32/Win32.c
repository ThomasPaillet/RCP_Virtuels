/*
 * copyright (c) 2018-2022 Thomas Paillet <thomas.paillet@net-c.fr>

 * This file is part of RCP-Virtuels.

 * RCP-Virtuels is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * RCP-Virtuels is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with RCP-Virtuels. If not, see <https://www.gnu.org/licenses/>.
*/

#include "../operating_system.h"
#include "../main_window.h"

#include <winsock2.h>


HANDLE rs_port = INVALID_HANDLE_VALUE;


void WSAInit (void)
{
	WORD requested_version;
	WSADATA wsa_data;
	BOOL return_value = TRUE;

	requested_version = MAKEWORD (2, 0);

	if (WSAStartup (requested_version, &wsa_data)) return_value = FALSE;
	else if (LOBYTE (wsa_data.wVersion) != 2) {
			WSACleanup ();
			return_value = FALSE;
	}
	if (!return_value) MessageBox (NULL, "WSAInit error !", NULL, MB_OK | MB_ICONERROR);
}

void timersub (const struct timeval* tvp, const struct timeval* uvp, struct timeval* vvp)
{
	vvp->tv_sec = tvp->tv_sec - uvp->tv_sec;
	vvp->tv_usec = tvp->tv_usec - uvp->tv_usec;

	if (vvp->tv_usec < 0) {
		--vvp->tv_sec;
		vvp->tv_usec += 1000000;
	}
}

GSList *list_rs_port (void)
{
	HKEY key;
	DWORD max_value_len, max_data_size, max_data_len;
	TCHAR *value, *data;
	DWORD value_len, data_size, data_len;
	DWORD type, index;
	GSList *gslist;

	if (RegOpenKeyEx (HKEY_LOCAL_MACHINE, "HARDWARE\\DEVICEMAP\\SERIALCOMM", 0, KEY_QUERY_VALUE, &key) != ERROR_SUCCESS) return NULL;

	if (RegQueryInfoKey (key, NULL, NULL, NULL, NULL, NULL, NULL, NULL, &max_value_len, &max_data_size, NULL, NULL) != ERROR_SUCCESS) {
		RegCloseKey (key);

		return NULL;
	}

	max_data_len = max_data_size / sizeof (TCHAR);

	value = g_malloc ((max_value_len + 1) * sizeof (TCHAR));
	data = g_malloc ((max_data_len + 1) * sizeof (TCHAR));

	index = 0;
	value_len = max_value_len + 1;
	data_size = max_data_size;
	gslist = NULL;

	while (RegEnumValue (key, index, value, &value_len, NULL, &type, (LPBYTE)data, &data_size) == ERROR_SUCCESS) {
		if (type == REG_SZ) {
			data_len = data_size / sizeof (TCHAR);
			data[data_len] = '\0';
			gslist = g_slist_prepend (gslist, data);
			data = g_malloc ((max_data_len + 1) * sizeof (TCHAR));
		}
		value_len = max_value_len + 1;
		data_size = max_data_size;
		index++;
	}

	g_free (value);
	g_free (data);

	RegCloseKey (key);

	return gslist;
}

void close_rs_port (void)
{
	if (rs_port != INVALID_HANDLE_VALUE) CloseHandle (rs_port);
	rs_port = INVALID_HANDLE_VALUE;
}

int open_rs_port (const char *name)
{
	DCB dcb;

	rs_port = CreateFileA (name, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
	if (rs_port == INVALID_HANDLE_VALUE) return -1;

	SecureZeroMemory (&dcb, sizeof (DCB));
	dcb.DCBlength = sizeof (DCB);

	if (!GetCommState (rs_port, &dcb)) { close_rs_port (); return -1; }

	dcb.BaudRate = CBR_38400;
	dcb.fParity = FALSE;
	dcb.ByteSize = 8;
	dcb.Parity   = NOPARITY;
	dcb.StopBits = ONESTOPBIT;

	if (!SetCommState (rs_port, &dcb)) { close_rs_port (); return -1; }

	return 0;
}

VOID CALLBACK send_to_rs_port_completion_routine (_In_ DWORD dwErrorCode, _In_ DWORD dwNumberOfBytesTransfered, _Inout_ LPOVERLAPPED lpOverlapped)
{
	g_free (lpOverlapped);
}

void send_to_rs_port (const char *buffer, long unsigned int buffer_len)
{
	OVERLAPPED *lpOverlapped;

	if (rs_port == INVALID_HANDLE_VALUE) return;

	lpOverlapped = g_malloc (sizeof (OVERLAPPED));
	SecureZeroMemory (lpOverlapped, sizeof (OVERLAPPED));

	if (WriteFileEx (rs_port, buffer, buffer_len, lpOverlapped, send_to_rs_port_completion_routine) == 0) {
		g_free (lpOverlapped);
		g_idle_add ((GSourceFunc)show_rs_connection_error_window, NULL);
		return;
	}

	if (SleepEx (2000, TRUE) == 0) {
		g_free (lpOverlapped);
		g_idle_add ((GSourceFunc)show_rs_connection_error_window, NULL);
	}
}

VOID CALLBACK receive_from_rs_port_completion_routine (_In_ DWORD dwErrorCode, _In_ DWORD dwNumberOfBytesTransfered, _Inout_ LPOVERLAPPED lpOverlapped)
{
	g_free (lpOverlapped);
}

int receive_from_rs_port (char *buffer, long unsigned int buffer_len)
{
	OVERLAPPED *overlapped;

	if (rs_port == INVALID_HANDLE_VALUE) return 0;

	overlapped = g_malloc (sizeof (OVERLAPPED));
	SecureZeroMemory (overlapped, sizeof (OVERLAPPED));

	if (ReadFileEx (rs_port, buffer, buffer_len, overlapped, receive_from_rs_port_completion_routine) == 0) {
		g_free (overlapped);
		g_idle_add ((GSourceFunc)show_rs_connection_error_window, NULL);

		return 0;
	}

	SleepEx (INFINITE, TRUE);

	return 1;
}

