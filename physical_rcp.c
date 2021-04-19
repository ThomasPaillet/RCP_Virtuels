/*
 * copyright (c) 2021 Thomas Paillet <thomas.paillet@net-c.fr>

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

#include "rcp.h"


#if __BYTE_ORDER == __LITTLE_ENDIAN

#define PTZ_CMD (int)('p' + ('t' << 8) + ('z' << 16) + ('?' << 24))

	#define PTZ_SHARP_AXI_CMD (int)('#' + ('A' << 8) + ('X' << 16) + ('I' << 24))
	#define PTZ_SHARP_D_CMD (short)('#' + ('D' << 8))
	#define PTZ_SHARP_LPI_CMD (int)('#' + ('L' << 8) + ('P' << 16) + ('I' << 24))
	#define PTZ_SHARP_O_STANBY_CMD (int)('#' + ('O' << 8) + ('0' << 16) + ('&' << 24))
	#define PTZ_SHARP_O_QUERY_CMD (int)('#' + ('O' << 8) + ('&' << 16) + ('r' << 24))
	#define PTZ_SHARP_LPC_CMD (int)('#' + ('L' << 8) + ('P' << 16) + ('C' << 24))
	#define PTZ_SHARP_Z_CMD (short)('#' + ('Z' << 8))
	#define PTZ_SHARP_F_CMD (short)('#' + ('F' << 8))

	#define PTZ_SHARP_P50_CMD (int)('#' + ('P' << 8) + ('5' << 16) + ('0' << 24))
	#define PTZ_SHARP_T50_CMD (int)('#' + ('T' << 8) + ('5' << 16) + ('0' << 24))

#define CAM_CMD (int)('c' + ('a' << 8) + ('m' << 16) + ('?' << 24))

	#define CAM_ORS_CMD (int)('O' + ('R' << 8) + ('S' << 16) + (':' << 24))
	#define CAM_QRS_CMD (int)('Q' + ('R' << 8) + ('S' << 16) + ('&' << 24))
	#define CAM_OTP_CMD (int)('O' + ('T' << 8) + ('P' << 16) + (':' << 24))
	#define CAM_QTP_CMD (int)('Q' + ('T' << 8) + ('P' << 16) + ('&' << 24))

	#define CAM_ORI_CMD (int)('O' + ('R' << 8) + ('I' << 16) + (':' << 24))
	#define CAM_QRI_CMD (int)('Q' + ('R' << 8) + ('I' << 16) + ('&' << 24))
	#define CAM_OBI_CMD (int)('O' + ('B' << 8) + ('I' << 16) + (':' << 24))
	#define CAM_QBI_CMD (int)('Q' + ('B' << 8) + ('I' << 16) + ('&' << 24))
	#define CAM_ORP_CMD (int)('O' + ('R' << 8) + ('P' << 16) + (':' << 24))
	#define CAM_QRP_CMD (int)('Q' + ('R' << 8) + ('P' << 16) + ('&' << 24))
	#define CAM_OBP_CMD (int)('O' + ('B' << 8) + ('P' << 16) + (':' << 24))
	#define CAM_QBP_CMD (int)('Q' + ('B' << 8) + ('P' << 16) + ('&' << 24))

	#define CAM_ODT_CMD (int)('O' + ('D' << 8) + ('T' << 16) + (':' << 24))
	#define CAM_QDT_CMD (int)('Q' + ('D' << 8) + ('T' << 16) + ('&' << 24))
	#define CAM_OSA_CMD (int)('O' + ('S' << 8) + ('A' << 16) + (':' << 24))
	#define CAM_OFT_CMD (int)('O' + ('F' << 8) + ('T' << 16) + (':' << 24))
	#define CAM_QFT_CMD (int)('Q' + ('F' << 8) + ('T' << 16) + ('&' << 24))
	#define CAM_OGU_CMD (int)('O' + ('G' << 8) + ('U' << 16) + (':' << 24))
	#define CAM_QGU_CMD (int)('Q' + ('G' << 8) + ('U' << 16) + ('&' << 24))
	#define CAM_OSH_CMD (int)('O' + ('S' << 8) + ('H' << 16) + (':' << 24))
	#define CAM_QSH_CMD (int)('Q' + ('S' << 8) + ('H' << 16) + ('&' << 24))
	#define CAM_DCB_CMD (int)('D' + ('C' << 8) + ('B' << 16) + (':' << 24))
	#define CAM_QBR_CMD (int)('Q' + ('B' << 8) + ('R' << 16) + ('&' << 24))
	#define CAM_OAF_CMD (int)('O' + ('A' << 8) + ('F' << 16) + (':' << 24))
	#define CAM_QAF_CMD (int)('Q' + ('A' << 8) + ('F' << 16) + ('&' << 24))

	#define CAM_QID_CMD (int)('Q' + ('I' << 8) + ('D' << 16) + ('&' << 24))
	#define CAM_QSE_CMD (int)('Q' + ('S' << 8) + ('E' << 16) + (':' << 24))
	#define CAM_QSA_CMD (int)('Q' + ('S' << 8) + ('A' << 16) + (':' << 24))
	#define CAM_QSD_CMD (int)('Q' + ('S' << 8) + ('D' << 16) + (':' << 24))

	#define CAM_OAS_CMD (int)('O' + ('A' << 8) + ('S' << 16) + ('&' << 24))
	#define CAM_OWS_CMD (int)('O' + ('W' << 8) + ('S' << 16) + ('&' << 24))

	#define CAM_DUS_CMD (int)('D' + ('U' << 8) + ('S' << 16) + (':' << 24))
	#define CAM_DPG_CMD (int)('D' + ('P' << 8) + ('G' << 16) + (':' << 24))
	#define CAM_DIT_CMD (int)('D' + ('I' << 8) + ('T' << 16) + (':' << 24))
	#define CAM_DUP_CMD (int)('D' + ('U' << 8) + ('P' << 16) + (':' << 24))
	#define CAM_DDW_CMD (int)('D' + ('D' << 8) + ('W' << 16) + (':' << 24))

	#define CAM_XSF_CMD (int)('X' + ('S' << 8) + ('F' << 16) + (':' << 24))

	#define CAM_NOTHING_CMD (int)('&' + ('r' << 8) + ('e' << 16) + ('s' << 24))

#define EVENT_CONNECT_CMD (int)('n' + ('t' << 8) + ('?' << 16) + ('c' << 24))
#define MAN_SESSION_CMD (int)('_' + ('s' << 8) + ('e' << 16) + ('s' << 24))
#define CAMDATA_CMD (int)('e' + ('/' << 8) + ('c' << 16) + ('a' << 24))

#else

#define PTZ_CMD (int)(('p' << 24) + 't' << 16) + ('z' << 8) + '?')

	#define PTZ_SHARP_AXI_CMD (int)(('#' << 24) + 'A' << 16) + ('X' << 8) + 'I')
	#define PTZ_SHARP_D_CMD (short)(('#' << 8) + 'D')
	#define PTZ_SHARP_LPI_CMD (int)(('#' << 24) + 'L' << 16) + ('P' << 8) + 'I')
	#define PTZ_SHARP_O_STANBY_CMD (int)(('#' << 24) + 'O' << 16) + ('0' << 8) + '&')
	#define PTZ_SHARP_O_QUERY_CMD (int)(('#' << 24) + 'O' << 16) + ('&' << 8) + 'r')
	#define PTZ_SHARP_LPC_CMD (int)(('#' << 24) + 'L' << 16) + ('P' << 8) + 'C')
	#define PTZ_SHARP_Z_CMD (short)(('#' << 8) + 'Z')
	#define PTZ_SHARP_F_CMD (short)(('#' << 8) + 'F')

	#define PTZ_SHARP_P50_CMD (int)(('#' << 24) + 'P' << 16) + ('5' << 8) + '0')
	#define PTZ_SHARP_T50_CMD (int)(('#' << 24) + 'T' << 16) + ('5' << 8) + '0')

#define CAM_CMD (int)(('c' << 24) + 'a' << 16) + ('m' << 8) + '?')

	#define CAM_ORS_CMD (int)(('O' << 24) + 'R' << 16) + ('S' << 8) + ':')
	#define CAM_QRS_CMD (int)(('Q' << 24) + 'R' << 16) + ('S' << 8) + '&')
	#define CAM_OTP_CMD (int)(('O' << 24) + 'T' << 16) + ('P' << 8) + ':')
	#define CAM_QTP_CMD (int)(('Q' << 24) + 'T' << 16) + ('P' << 8) + '&')

	#define CAM_ORI_CMD (int)(('O' << 24) + 'R' << 16) + ('I' << 8) + ':')
	#define CAM_QRI_CMD (int)(('Q' << 24) + 'R' << 16) + ('I' << 8) + '&')
	#define CAM_OBI_CMD (int)(('O' << 24) + 'B' << 16) + ('I' << 8) + ':')
	#define CAM_QBI_CMD (int)(('Q' << 24) + 'B' << 16) + ('I' << 8) + '&')
	#define CAM_ORP_CMD (int)(('O' << 24) + 'R' << 16) + ('P' << 8) + ':')
	#define CAM_QRP_CMD (int)(('Q' << 24) + 'R' << 16) + ('P' << 8) + '&')
	#define CAM_OBP_CMD (int)(('O' << 24) + 'B' << 16) + ('P' << 8) + ':')
	#define CAM_QBP_CMD (int)(('Q' << 24) + 'B' << 16) + ('P' << 8) + '&')

	#define CAM_ODT_CMD (int)(('O' << 24) + 'D' << 16) + ('T' << 8) + ':')
	#define CAM_QDT_CMD (int)(('Q' << 24) + 'D' << 16) + ('T' << 8) + '&')
	#define CAM_OSA_CMD (int)(('O' << 24) + 'S' << 16) + ('A' << 8) + ':')
	#define CAM_OFT_CMD (int)(('O' << 24) + 'F' << 16) + ('T' << 8) + ':')
	#define CAM_QFT_CMD (int)(('Q' << 24) + 'F' << 16) + ('T' << 8) + '&')
	#define CAM_OGU_CMD (int)(('O' << 24) + 'G' << 16) + ('U' << 8) + ':')
	#define CAM_QGU_CMD (int)(('Q' << 24) + 'G' << 16) + ('U' << 8) + '&')
	#define CAM_OSH_CMD (int)(('O' << 24) + 'S' << 16) + ('H' << 8) + ':')
	#define CAM_QSH_CMD (int)(('Q' << 24) + 'S' << 16) + ('H' << 8) + '&')
	#define CAM_DCB_CMD (int)(('D' << 24) + 'C' << 16) + ('B' << 8) + ':')
	#define CAM_QBR_CMD (int)(('Q' << 24) + 'B' << 16) + ('R' << 8) + '&')
	#define CAM_OAF_CMD (int)(('O' << 24) + 'A' << 16) + ('F' << 8) + ':')
	#define CAM_QAF_CMD (int)(('Q' << 24) + 'A' << 16) + ('F' << 8) + '&')

	#define CAM_QID_CMD (int)(('Q' << 24) + 'I' << 16) + ('D' << 8) + '&')
	#define CAM_QSE_CMD (int)(('Q' << 24) + 'S' << 16) + ('E' << 8) + ':')
	#define CAM_QSA_CMD (int)(('Q' << 24) + 'S' << 16) + ('A' << 8) + ':')
	#define CAM_QSD_CMD (int)(('Q' << 24) + 'S' << 16) + ('D' << 8) + ':')

	#define CAM_OAS_CMD (int)(('O' << 24) + 'A' << 16) + ('S' << 8) + '&')
	#define CAM_OWS_CMD (int)(('O' << 24) + 'W' << 16) + ('S' << 8) + '&')

	#define CAM_DUS_CMD (int)(('D' << 24) + 'U' << 16) + ('S' << 8) + ':')
	#define CAM_DPG_CMD (int)(('D' << 24) + 'P' << 16) + ('G' << 8) + ':')
	#define CAM_DIT_CMD (int)(('D' << 24) + 'I' << 16) + ('T' << 8) + ':')
	#define CAM_DUP_CMD (int)(('D' << 24) + 'U' << 16) + ('P' << 8) + ':')
	#define CAM_DDW_CMD (int)(('D' << 24) + 'D' << 16) + ('W' << 8) + ':')

	#define CAM_XSF_CMD (int)(('X' << 24) + 'S' << 16) + ('F' << 8) + ':')

	#define CAM_NOTHING_CMD (int)(('&' << 24) + 'r' << 16) + ('e' << 8) + 's')

#define EVENT_CONNECT_CMD (int)(('n' << 24) + 't' << 16) + ('?' << 8) + 'c')
#define MAN_SESSION_CMD (int)(('_' << 24) + 's' << 16) + ('e' << 8) + 's')
#define CAMDATA_CMD (int)(('e' << 24) + '/' << 16) + ('c' << 8) + 'a')

#endif


//Iris position
char answer_axi[] = { "HTTP/1.0 200 OK\r\nConnection: Close\r\nContent-Type: text/plain\r\nContent-length: 6\r\n\r\naxiAAA" };

//Auto focus
char answer_d1[] = { "HTTP/1.0 200 OK\r\nConnection: Close\r\nContent-Type: text/plain\r\nContent-length: 3\r\n\r\nd10" };

//Tally On/Off
char answer_dA[] = { "HTTP/1.0 200 OK\r\nConnection: Close\r\nContent-Type: text/plain\r\nContent-length: 3\r\n\r\ndA0" };

//Lens information
char answer_lPI[] = { "HTTP/1.0 200 OK\r\nConnection: Close\r\nContent-Type: text/plain\r\nContent-length: 12\r\n\r\nlPIAAAAAAAAA" };

//Power
char answer_p[] = { "HTTP/1.0 200 OK\r\nConnection: Close\r\nContent-Type: text/plain\r\nContent-length: 2\r\n\r\np1" };

//Lens information notification
char answer_lPC[] = { "HTTP/1.0 200 OK\r\nConnection: Close\r\nContent-Type: text/plain\r\nContent-length: 4\r\n\r\nlPC1" };

//Zoom speed
char answer_zS[] = { "HTTP/1.0 200 OK\r\nConnection: Close\r\nContent-Type: text/plain\r\nContent-length: 4\r\n\r\nzS50" };

//Focus speed
char answer_fS[] = { "HTTP/1.0 200 OK\r\nConnection: Close\r\nContent-Type: text/plain\r\nContent-length: 4\r\n\r\nfS50" };

//Pan speed
char answer_pS[] = { "HTTP/1.0 200 OK\r\nConnection: Close\r\nContent-Type: text/plain\r\nContent-length: 4\r\n\r\npS50" };

//Tilt speed
char answer_tS[] = { "HTTP/1.0 200 OK\r\nConnection: Close\r\nContent-Type: text/plain\r\nContent-length: 4\r\n\r\ntS50" };

//Iris Auto/Manual
char answer_ORS[] = { "HTTP/1.0 200 OK\r\nConnection: Close\r\nContent-Type: text/plain\r\nContent-length: 5\r\n\r\nORS:0" };

//Pedestal
char answer_OTP[] = { "HTTP/1.0 200 OK\r\nConnection: Close\r\nContent-Type: text/plain\r\nContent-length: 7\r\n\r\nOTP:096" };

//R Gain
char answer_ORI[] = { "HTTP/1.0 200 OK\r\nConnection: Close\r\nContent-Type: text/plain\r\nContent-length: 7\r\n\r\nORI:096" };
//B Gain
char answer_OBI[] = { "HTTP/1.0 200 OK\r\nConnection: Close\r\nContent-Type: text/plain\r\nContent-length: 7\r\n\r\nOBI:096" };

//R Pedestal
char answer_ORP[] = { "HTTP/1.0 200 OK\r\nConnection: Close\r\nContent-Type: text/plain\r\nContent-length: 7\r\n\r\nORP:096" };
//B Pedestal
char answer_OBP[] = { "HTTP/1.0 200 OK\r\nConnection: Close\r\nContent-Type: text/plain\r\nContent-length: 7\r\n\r\nOBP:096" };

//Detail
char answer_ODT[] = { "HTTP/1.0 200 OK\r\nConnection: Close\r\nContent-Type: text/plain\r\nContent-length: 5\r\n\r\nODT:1" };
//TOTAL DTL LEVEL
char answer_OSA_30[] = { "HTTP/1.0 200 OK\r\nConnection: Close\r\nContent-Type: text/plain\r\nContent-length: 9\r\n\r\nOSA:30:80" };

//NB filter
char answer_OFT[] = { "HTTP/1.0 200 OK\r\nConnection: Close\r\nContent-Type: text/plain\r\nContent-length: 5\r\n\r\nOFT:0" };

//Gain
char answer_OGU[] = { "HTTP/1.0 200 OK\r\nConnection: Close\r\nContent-Type: text/plain\r\nContent-length: 6\r\n\r\nOGU:08" };

//Shutter
char answer_OSH[] = { "HTTP/1.0 200 OK\r\nConnection: Close\r\nContent-Type: text/plain\r\nContent-length: 5\r\n\r\nOSH:0" };

//Color bar/Camera
char answer_DCB[] = { "HTTP/1.0 200 OK\r\nConnection: Close\r\nContent-Type: text/plain\r\nContent-length: 5\r\n\r\nDCB:0" };
char answer_OBR[] = { "HTTP/1.0 200 OK\r\nConnection: Close\r\nContent-Type: text/plain\r\nContent-length: 5\r\n\r\nOBR:0" };

//Focus Auto/Manual
char answer_OAF[] = { "HTTP/1.0 200 OK\r\nConnection: Close\r\nContent-Type: text/plain\r\nContent-length: 5\r\n\r\nOAF:0" };

//Model number
char answer_OID[] = { "HTTP/1.0 200 OK\r\nConnection: Close\r\nContent-Type: text/plain\r\nContent-length: 12\r\n\r\nOID:AW-HE130" };

//Frequency
char answer_OSE_77[] = { "HTTP/1.0 200 OK\r\nConnection: Close\r\nContent-Type: text/plain\r\nContent-length: 8\r\n\r\nOSE:77:1" };

//Format
char answer_OSA_87[] = { "HTTP/1.0 200 OK\r\nConnection: Close\r\nContent-Type: text/plain\r\nContent-length: 8\r\n\r\nOSA:87:5_" };

//Contrast level Picture level
char answer_OSD_48[] = { "HTTP/1.0 200 OK\r\nConnection: Close\r\nContent-Type: text/plain\r\nContent-length: 9\r\n\r\nOSD:48:32" };

//"204 No Content"
char answer_204[] = { "HTTP/1.0 204 No Content\r\nStatus: 204\r\nConnection: Close\r\nContent-length:0\r\n\r\n" };

//OSD menu
char answer_DUS[] = { "HTTP/1.0 200 OK\r\nConnection: Close\r\nContent-Type: text/plain\r\nContent-length: 5\r\n\r\nDUS:0" };
char answer_DPG[] = { "HTTP/1.0 200 OK\r\nConnection: Close\r\nContent-Type: text/plain\r\nContent-length: 5\r\n\r\nDPG:1" };
char answer_DIT[] = { "HTTP/1.0 200 OK\r\nConnection: Close\r\nContent-Type: text/plain\r\nContent-length: 5\r\n\r\nDIT:1" };
char answer_DUP[] = { "HTTP/1.0 200 OK\r\nConnection: Close\r\nContent-Type: text/plain\r\nContent-length: 5\r\n\r\nDUP:1" };
char answer_DDW[] = { "HTTP/1.0 200 OK\r\nConnection: Close\r\nContent-Type: text/plain\r\nContent-length: 5\r\n\r\nDDW:1" };

//Scene file
char answer_XSF[] = { "HTTP/1.0 200 OK\r\nConnection: Close\r\nContent-Type: text/plain\r\nContent-length: 5\r\n\r\nXSF:1" };

char answer_session[] = { "HTTP/1.0 200 OK\r\nConnection: Close\r\nContent-Type: text/plain\r\nContent-length: 15\r\n\r\nEvent session:0" };

char answer_error[] = { "HTTP/1.0 500 Internal Server Error\r\nStatus: 500\r\nDate: Sat, 27 Apr 2013 02:42:58 GMT\r\nConnection: Close\r\nContent-length: 109\r\n\r\n<HTML><HEAD><TITLE>500 Internal Server Error</TITLE></HEAD><BODY><H1>Internal Server Error</H1></BODY></HTML>" };

//char answer[] = { "HTTP/1.0 200 OK\r\nConnection: Close\r\nContent-Type: text/plain\r\nContent-length: 0\r\n\r\n" };

//Camera information batch acquisition
char answer_camdata[] = { "HTTP/1.0 200 OK\r\nStatus: 200\r\nDate: Sat, 27 Apr 2013 02:42:58 GMT\r\nConnection: Close\r\nContent-Type: text/html\r\nAccept-Ranges: bytes\r\nCache-Control: no-cache\r\nContent-length: 218\r\n\r\np1\r\nOID:AW-HE130\r\nCGI_TIME:130\r\nOSA:87:0x05\r\nTITLE:RCP-VIRTUELS\r\nOGU:0x08\r\nOAW:9\r\nOSH:0x0\r\nODT:1\r\nOSF:0\r\nOBR:0\r\nsWZ1\r\nOSE:71:2\r\niNS0\r\nOUS:0\r\nd10\r\nd30\r\ns50\r\nOSA:30:80\r\nd20\r\nd60\r\nd40\r\nOSD:4F:95\r\nOER:0\r\nrt1\r\naxzAFF\r\nrER00" };
//OSA:87:0x05	[222]	//Format
//OGU:0x08	[252]	//Gain
//OAW:9				//AWB Mode = VAR
//OSH:0x0		[269]	//Shutter
//ODT:1		[276]	//Detail
//OSF:0				//Scene = Scene1
//OBR:0		[290]	//Mire
//sWZ1				//Speed With Zoom Pos = On
//OSE:71:2			//Preset Mode = Mode C
//iNS0				//Install Position = Desktop
//OUS:0				//OSD Off
//d10		[324]	//Focus auto
//d30		[329]	//Iris auto
//s50				//Latest Call Preset No. = 50
//OSA:30:80	[344]	//Total Detail Level
//d20				//ND Filter = 0 (fixed)
//d60				//Day/Night switching = Day
//d40				//Lamp = 0 (fixed)
//OSD:4F:95	[370]	//Iris Follow (00h --> FFh)
//OER:0				//Error Notice = Normal
//rt1				//P/T Mode of Preset = 1 (fixed)
//axzAAA				//Zoom Position
//rER00				//Error Status Info. = No Error

//axfAAA				//Focus Position
//

char zoom_cmd[] = { "#Z50" };
char focus_cmd[] = { "#F50" };

char r_gain_cmd[] = { "ORI:096" };
char b_gain_cmd[] = { "OBI:096" };
char r_pedestal_cmd[] = { "ORP:096" };
char b_pedestal_cmd[] = { "OBP:096" };
char master_detail_cmd[] = { "OSA:30:80" };
char ND_filter_cmd[] = { "OFT:0" };
char gain_cmd[] = { "OGU:08" };
char shutter_cmd[] = { "OSH:0" };

char iris_update_notification[] = { "\x0a\x5e\xf4\xe9\x0f\xdf\x13\x01\x02\x05\x26\x36\x00\x01\x00\x80\x00\x00\x00\x00\x00\x01\x00\x1c\x01\x00\x00\x00\x0d\x0a\x6c\x50\x49\x41\x41\x41\x41\x41\x41\x41\x41\x41\x0d\x0a\x00\x00\x00\x00\x00\x02\x00\x18\xa8\x13\x74\x76\x87\x81\x00\x01\x13\x01\x02\x05\x26\x36\x00\x00\x00\x00\x00" };
//20 [lPIAAAAAAAAA    ]

char iris_auto_update_notification[] = { "\x0a\x5e\xf4\xe8\x25\x95\x13\x01\x07\x02\x34\x03\x00\x01\x00\x80\x00\x00\x00\x00\x00\x01\x00\x14\x01\x00\x00\x00\x0d\x0a\x4f\x52\x53\x3a\x30\x0d\x0a\x00\x00\x00\x00\x02\x00\x18\xa8\x13\x74\x76\x87\x7d\x00\x01\x13\x01\x07\x02\x34\x03\x00\x00\x00\x00\x00" };
//12 [ORS:0   ]

char pedestal_update_notification[] = { "\x0a\x5e\xf4\xe9\x0f\xde\x13\x01\x02\x05\x26\x36\x00\x01\x00\x80\x00\x00\x00\x00\x00\x01\x00\x14\x01\x00\x00\x00\x0d\x0a\x4f\x54\x50\x3a\x30\x39\x36\x0d\x0a\x00\x00\x02\x00\x18\xa8\x13\x74\x76\x87\x81\x00\x01\x13\x01\x02\x05\x26\x36\x00\x00\x00\x00\x00" };
//12 [OTP:096 ]

char r_gain_update_notification[] = { "\x0a\x5e\xf4\xe9\x02\xc5\x13\x01\x01\x06\x19\x46\x00\x01\x00\x80\x00\x00\x00\x00\x00\x01\x00\x14\x01\x00\x00\x00\x0d\x0a\x4f\x52\x49\x3a\x30\x39\x36\x0d\x0a\x00\x00\x02\x00\x18\xa8\x13\x74\x76\x87\x81\x00\x01\x13\x01\x01\x06\x19\x46\x00\x00\x00\x00\x00" };
//12 [ORI:096 ]

char b_gain_update_notification[] = { "\x0a\x5e\xf4\xe9\x02\xc6\x13\x01\x01\x06\x19\x46\x00\x01\x00\x80\x00\x00\x00\x00\x00\x01\x00\x14\x01\x00\x00\x00\x0d\x0a\x4f\x42\x49\x3a\x30\x39\x36\x0d\x0a\x00\x00\x02\x00\x18\xa8\x13\x74\x76\x87\x81\x00\x01\x13\x01\x01\x06\x19\x46\x00\x00\x00\x00\x00" };
//12 [OBI:096 ]

char r_pedestal_update_notification[] = { "\x0a\x5e\xf4\xe9\x40\xba\x13\x01\x10\x19\x31\x37\x00\x01\x00\x80\x00\x00\x00\x00\x00\x01\x00\x14\x01\x00\x00\x00\x0d\x0a\x4f\x52\x50\x3a\x30\x39\x36\x0d\x0a\x00\x00\x02\x00\x18\xa8\x13\x74\x76\x87\x81\x00\x01\x13\x01\x10\x19\x31\x37\x00\x00\x00\x00\x00" };
//12 [ORP:096 ]

char b_pedestal_update_notification[] = { "\x0a\x5e\xf4\xe9\x40\xbb\x13\x01\x10\x19\x31\x37\x00\x01\x00\x80\x00\x00\x00\x00\x00\x01\x00\x14\x01\x00\x00\x00\x0d\x0a\x4f\x42\x50\x3a\x30\x39\x36\x0d\x0a\x00\x00\x02\x00\x18\xa8\x13\x74\x76\x87\x81\x00\x01\x13\x01\x10\x19\x31\x37\x00\x00\x00\x00\x00" };
//12 [OBP:096 ]

char detail_update_notification[] = { "\x0a\x5e\xf4\xe9\x21\x54\x13\x01\x06\x02\x28\x38\x00\x01\x00\x80\x00\x00\x00\x00\x00\x01\x00\x14\x01\x00\x00\x00\x0d\x0a\x4f\x44\x54\x3a\x31\x0d\x0a\x00\x00\x00\x00\x02\x00\x18\xa8\x13\x74\x76\x87\x81\x00\x01\x13\x01\x06\x02\x28\x38\x00\x00\x00\x00\x00" };
//12 [ODT:1   ]

char master_detail_update_notification[] = { "\x0a\x5e\xf4\xe9\x21\x6e\x13\x01\x06\x02\x28\x48\x00\x01\x00\x80\x00\x00\x00\x00\x00\x01\x00\x18\x01\x00\x00\x00\x0d\x0a\x4f\x53\x41\x3a\x33\x30\x3a\x38\x30\x0d\x0a\x00\x00\x00\x00\x02\x00\x18\xa8\x13\x74\x76\x87\x81\x00\x01\x13\x01\x06\x02\x28\x48\x00\x00\x00\x00\x00" };
//16 [OSA:30:80   ]

char ND_filter_update_notification[] = { "\x0a\x5e\xf4\xe9\x21\x84\x13\x01\x06\x02\x29\x12\x00\x01\x00\x80\x00\x00\x00\x00\x00\x01\x00\x14\x01\x00\x00\x00\x0d\x0a\x4f\x46\x54\x3a\x30\x0d\x0a\x00\x00\x00\x00\x02\x00\x18\xa8\x13\x74\x76\x87\x81\x00\x01\x13\x01\x06\x02\x29\x12\x00\x00\x00\x00\x00" };
//12 [OFT:0   ]

char gain_update_notification[] = { "\x0a\x5e\xf4\xe9\x22\x0a\x13\x01\x06\x02\x30\x56\x00\x01\x00\x80\x00\x00\x00\x00\x00\x01\x00\x14\x01\x00\x00\x00\x0d\x0a\x4f\x47\x55\x3a\x30\x38\x0d\x0a\x00\x00\x00\x02\x00\x18\xa8\x13\x74\x76\x87\x81\x00\x01\x13\x01\x06\x02\x30\x56\x00\x00\x00\x00\x00" };
//12 [OGU:08  ]

char shutter_update_notification[] = { "\x0a\x5e\xf4\xe9\x21\x79\x13\x01\x06\x02\x28\x55\x00\x01\x00\x80\x00\x00\x00\x00\x00\x01\x00\x14\x01\x00\x00\x00\x0d\x0a\x4f\x53\x48\x3a\x30\x0d\x0a\x00\x00\x00\x00\x02\x00\x18\xa8\x13\x74\x76\x87\x81\x00\x01\x13\x01\x06\x02\x28\x55\x00\x00\x00\x00\x00" };
//12 [OSH:0   ]

char mire_update_notification[] = { "\x0a\x5e\xf4\xe9\x21\x52\x13\x01\x06\x02\x28\x33\x00\x01\x00\x80\x00\x00\x00\x00\x00\x01\x00\x14\x01\x00\x00\x00\x0d\x0a\x44\x43\x42\x3a\x30\x0d\x0a\x00\x00\x00\x00\x02\x00\x18\xa8\x13\x74\x76\x87\x81\x00\x01\x13\x01\x06\x02\x28\x33\x00\x00\x00\x00\x00" };
//12 [DCB:0   ]

char auto_focus_update_notification[] = { "\x0a\x5e\xf4\xe9\x42\x1d\x13\x01\x10\x19\x33\x58\x00\x01\x00\x80\x00\x00\x00\x00\x00\x01\x00\x10\x01\x00\x00\x00\x0d\x0a\x64\x31\x30\x0d\x0a\x00\x00\x02\x00\x18\xa8\x13\x74\x76\x87\x81\x00\x01\x13\x01\x10\x19\x33\x58\x00\x00\x00\x00\x00" };
//08 [d10 ]

char frequency_update_notification[] = { "\x0a\x5e\xf4\xe9\x00\x0d\x13\x01\x01\x00\x06\x06\x00\x01\x00\x80\x00\x00\x00\x00\x00\x01\x00\x18\x01\x00\x00\x00\x0d\x0a\x4f\x53\x45\x3a\x37\x37\x3a\x31\x0d\x0a\x00\x00\x00\x00\x00\x02\x00\x18\xa8\x13\x74\x76\x87\x81\x00\x01\x13\x01\x01\x00\x06\x06\x00\x00\x00\x00\x00" };
//16 [OSE:77:1    ]

char format_update_notification[] = { "\x0a\x5e\xf4\xe8\x3c\x88\x13\x01\x10\x19\x50\x49\x00\x01\x00\x80\x00\x00\x00\x00\x00\x01\x00\x18\x01\x00\x00\x00\x0d\x0a\x4f\x53\x41\x3a\x38\x37\x3a\x35\x0d\x0a\x00\x00\x00\x00\x00\x02\x00\x18\xa8\x13\x74\x76\x87\x7d\x00\x01\x13\x01\x10\x19\x50\x49\x00\x00\x00\x00\x00" };
//16 [OSA:87:5    ]

char picture_level_update_notification[] = { "\x0a\x5e\xf4\xe9\x40\xd1\x13\x01\x10\x19\x32\x19\x00\x01\x00\x80\x00\x00\x00\x00\x00\x01\x00\x18\x01\x00\x00\x00\x0d\x0a\x4f\x53\x44\x3a\x34\x38\x3a\x33\x32\x0d\x0a\x00\x00\x00\x00\x02\x00\x18\xa8\x13\x74\x76\x87\x81\x00\x01\x13\x01\x10\x19\x32\x19\x00\x00\x00\x00\x00" };
//16 [OSD:48:32   ]

char ABB_execution_successful_update_notification[] = { "\x0a\x5e\xf4\xe9\x40\xb9\x13\x01\x10\x19\x31\x37\x00\x01\x00\x80\x00\x00\x00\x00\x00\x01\x00\x10\x01\x00\x00\x00\x0d\x0a\x4f\x41\x53\x0d\x0a\x00\x00\x02\x00\x18\xa8\x13\x74\x76\x87\x81\x00\x01\x13\x01\x10\x19\x31\x37\x00\x00\x00\x00\x00" };
//08 [OAS ]

char AWB_execution_successful_update_notification[] = { "\x0a\x5e\xf4\xe9\x02\xc4\x13\x01\x01\x06\x19\x46\x00\x01\x00\x80\x00\x00\x00\x00\x00\x01\x00\x10\x01\x00\x00\x00\x0d\x0a\x4f\x57\x53\x0d\x0a\x00\x00\x02\x00\x18\xa8\x13\x74\x76\x87\x81\x00\x01\x13\x01\x01\x06\x19\x46\x00\x00\x00\x00\x00" };
//08 [OWS ]

char version_update_notification[] = { "\x0a\x5e\xf4\xeb\x29\x0c\x13\x01\x06\x06\x53\x19\x00\x01\x00\x80\x00\x00\x00\x00\x00\x01\x00\x1c\x01\x00\x00\x00\x0d\x0a\x71\x53\x56\x33\x56\x30\x31\x2e\x30\x36\x4c\x30\x30\x30\x0d\x0a\x00\x00\x00\x02\x00\x18\xa8\x13\x74\x76\x87\x76\x00\x01\x13\x01\x06\x06\x53\x19\x00\x00\x00\x00\x00" };
//20 [qSV3V01.06L000  ]

physical_rcp_t physical_rcp = { IRIS_DEFAULT, IRIS_AUTO_DEFAULT, PEDESTAL_DEFAULT, R_GAIN_DEFAULT, B_GAIN_DEFAULT, R_PEDESTAL_DEFAULT, B_PEDESTAL_DEFAULT, DETAIL_DEFAULT, MASTER_DETAIL_DEFAULT, ND_FILTER_DEFAULT, GAIN_DEFAULT, SHUTTER_TYPE_DEFAULT, -1, FALSE, FALSE, FALSE, FALSE, FALSE, 0 };

gboolean physical_rcp_started = FALSE;

GThread *physical_rcp_thread = NULL;

GThread *update_physical_rcp_in_background_thread = NULL;


#define SEND_UPDATE_NOTIFICATION(n) \
	physical_rcp.socket_for_update_notification = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP); \
 \
	if (connect (physical_rcp.socket_for_update_notification, (struct sockaddr *)&physical_rcp.address_for_update_notification, sizeof (struct sockaddr_in)) == 0) { \
		send (physical_rcp.socket_for_update_notification, n##_update_notification, sizeof (n##_update_notification), 0); \
	} else physical_rcp.connected = FALSE; \
 \
	closesocket (physical_rcp.socket_for_update_notification);


void send_iris_auto_update_notification (void)
{
	if (physical_rcp.iris_auto) iris_auto_update_notification[34] = '1';
	else iris_auto_update_notification[34] = '0';

	SEND_UPDATE_NOTIFICATION(iris_auto)
}

void send_r_gain_update_notification (void)
{
	r_gain_update_notification[34] = '0' + (physical_rcp.r_gain >> 8);
	if (r_gain_update_notification[34] > '9') r_gain_update_notification[34] += 7;
	r_gain_update_notification[35] = '0' + ((physical_rcp.r_gain & 0x0FF) >> 4);
	if (r_gain_update_notification[35] > '9') r_gain_update_notification[35] += 7;
	r_gain_update_notification[36] = '0' + (physical_rcp.r_gain & 0x00F);
	if (r_gain_update_notification[36] > '9') r_gain_update_notification[36] += 7;

	SEND_UPDATE_NOTIFICATION(r_gain)
}

void send_b_gain_update_notification (void)
{
	b_gain_update_notification[34] = '0' + (physical_rcp.b_gain >> 8);
	if (b_gain_update_notification[34] > '9') b_gain_update_notification[34] += 7;
	b_gain_update_notification[35] = '0' + ((physical_rcp.b_gain & 0x0FF) >> 4);
	if (b_gain_update_notification[35] > '9') b_gain_update_notification[35] += 7;
	b_gain_update_notification[36] = '0' + (physical_rcp.b_gain & 0x00F);
	if (b_gain_update_notification[36] > '9') b_gain_update_notification[36] += 7;

	SEND_UPDATE_NOTIFICATION(b_gain)
}

void send_r_pedestal_update_notification (void)
{
	r_pedestal_update_notification[34] = '0' + (physical_rcp.r_pedestal >> 8);
	if (r_pedestal_update_notification[34] > '9') r_pedestal_update_notification[34] += 7;
	r_pedestal_update_notification[35] = '0' + ((physical_rcp.r_pedestal & 0x0FF) >> 4);
	if (r_pedestal_update_notification[35] > '9') r_pedestal_update_notification[35] += 7;
	r_pedestal_update_notification[36] = '0' + (physical_rcp.r_pedestal & 0x00F);
	if (r_pedestal_update_notification[36] > '9') r_pedestal_update_notification[36] += 7;

	SEND_UPDATE_NOTIFICATION(r_pedestal)
}

void send_b_pedestal_update_notification (void)
{
	b_pedestal_update_notification[34] = '0' + (physical_rcp.b_pedestal >> 8);
	if (b_pedestal_update_notification[34] > '9') b_pedestal_update_notification[34] += 7;
	b_pedestal_update_notification[35] = '0' + ((physical_rcp.b_pedestal & 0x0FF) >> 4);
	if (b_pedestal_update_notification[35] > '9') b_pedestal_update_notification[35] += 7;
	b_pedestal_update_notification[36] = '0' + (physical_rcp.b_pedestal & 0x00F);
	if (b_pedestal_update_notification[36] > '9') b_pedestal_update_notification[36] += 7;

	SEND_UPDATE_NOTIFICATION(b_pedestal)
}

void send_detail_update_notification (void)
{
	if (physical_rcp.detail) detail_update_notification[34] = '1';
	else detail_update_notification[34] = '0';

	SEND_UPDATE_NOTIFICATION(detail)
}

void send_master_detail_update_notification (void)
{
	master_detail_update_notification[37] = '0' + (physical_rcp.master_detail >> 4);
	if (master_detail_update_notification[37] > '9') master_detail_update_notification[37] += 7;
	master_detail_update_notification[38] = '0' + (physical_rcp.master_detail & 0x0F);
	if (master_detail_update_notification[38] > '9') master_detail_update_notification[38] += 7;

	SEND_UPDATE_NOTIFICATION(master_detail)
}

void send_ND_filter_update_notification (void)
{
	if (physical_rcp.ND_filter == 2) ND_filter_update_notification[34] = '3';
	else if (physical_rcp.ND_filter == 1) ND_filter_update_notification[34] = '4';
	else ND_filter_update_notification[34] = '0';

	SEND_UPDATE_NOTIFICATION(ND_filter)
}

void send_gain_update_notification (void)
{
	int value;

	if (physical_rcp.gain == 37) {
		gain_update_notification[34] = '8';
		gain_update_notification[35] = '0';
	} else {
		value = 0x2C - physical_rcp.gain;

		gain_update_notification[34] = '0' + (value >> 4);
		if (gain_update_notification[34] > '9') gain_update_notification[34] += 7;
		gain_update_notification[35] = '0' + (value & 0x0F);
		if (gain_update_notification[35] > '9') gain_update_notification[35] += 7;
	}

	SEND_UPDATE_NOTIFICATION(gain)
}

void send_shutter_update_notification (void)
{
	if (physical_rcp.shutter_type == 0) shutter_update_notification[34] = '0';
	else if (physical_rcp.shutter_type == 2) shutter_update_notification[34] = 'B';
	else if (physical_rcp.shutter_type == 3) shutter_update_notification[34] = 'C';
	else {
		if (physical_rcp.shutter_step == 0) {
			if (output_fps == _59_94fps) shutter_update_notification[34] = '3';
			else shutter_update_notification[34] = '2';
		} else if (physical_rcp.shutter_step == 1) {
			if ((output_fps == _25fps) || (output_fps == _50fps)) shutter_update_notification[34] = '3';
			else shutter_update_notification[34] = '4';
		} else if (physical_rcp.shutter_step == 2) shutter_update_notification[34] = '5';
		else if (physical_rcp.shutter_step == 3) shutter_update_notification[34] = '6';
		else if (physical_rcp.shutter_step == 4) shutter_update_notification[34] = '7';
		else if (physical_rcp.shutter_step == 5) shutter_update_notification[34] = '8';
		else if (physical_rcp.shutter_step == 6) shutter_update_notification[34] = '9';
		else if (physical_rcp.shutter_step == 7) shutter_update_notification[34] = 'A';
		else if (physical_rcp.shutter_step == 8) {
			if (output_fps == _23_98fps) shutter_update_notification[34] = 'D';
			else if (output_fps == _25fps) shutter_update_notification[34] = 'E';
			else if (output_fps == _29_97fps) shutter_update_notification[34] = 'F';
		}
	}

	SEND_UPDATE_NOTIFICATION(shutter)
}

void send_mire_update_notification (void)
{
	if (physical_rcp.mire) mire_update_notification[34] = '1';
	else mire_update_notification[34] = '0';

	SEND_UPDATE_NOTIFICATION(mire)
}

void send_auto_focus_update_notification (void)
{
	if (physical_rcp.auto_focus) auto_focus_update_notification[32] = '1';
	else auto_focus_update_notification[32] = '0';

	SEND_UPDATE_NOTIFICATION(auto_focus)
}

void send_frequency_update_notification (void)
{
	if (format_is_50Hz) frequency_update_notification[37] = '1';
	else frequency_update_notification[37] = '0';

	SEND_UPDATE_NOTIFICATION(frequency)
}

void send_format_update_notification (void)
{
	format_update_notification[37] = settings_array[1].answers[settings_parameters_indexes_array[1]][0];

	if (settings_array[1].answers[settings_parameters_indexes_array[1]][1] == '\0') {
		format_update_notification[38] = '\r';
		format_update_notification[39] = '\n';
		format_update_notification[40] = '\0';
	} else {
		format_update_notification[38] = settings_array[1].answers[settings_parameters_indexes_array[1]][1];
		format_update_notification[39] = '\r';
		format_update_notification[40] = '\n';
	}

	SEND_UPDATE_NOTIFICATION(format)
}

void send_picture_level_update_notification (void)
{
	int value = picture_level + 0x32;

	picture_level_update_notification[37] = '0' + (value >> 4);
	if (picture_level_update_notification[37] > '9') picture_level_update_notification[37] += 7;
	picture_level_update_notification[38] = '0' + (value & 0x0F);
	if (picture_level_update_notification[38] > '9') picture_level_update_notification[38] += 7;

	SEND_UPDATE_NOTIFICATION(picture_level)
}

void send_ABB_execution_successful_update_notification (void)
{
	SEND_UPDATE_NOTIFICATION(ABB_execution_successful)
}

void send_AWB_execution_successful_update_notification (void)
{
	SEND_UPDATE_NOTIFICATION(AWB_execution_successful)
}

gboolean send_version_update_notification (gpointer nothing)
{
	g_mutex_lock (&physical_rcp.mutex);

	physical_rcp.socket_for_update_notification = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (connect (physical_rcp.socket_for_update_notification, (struct sockaddr *)&physical_rcp.address_for_update_notification, sizeof (struct sockaddr_in)) == 0) {
		send (physical_rcp.socket_for_update_notification, version_update_notification, sizeof (version_update_notification), 0);

		closesocket (physical_rcp.socket_for_update_notification);

		g_mutex_unlock (&physical_rcp.mutex);

		return G_SOURCE_CONTINUE;
	} else {
		closesocket (physical_rcp.socket_for_update_notification);

		physical_rcp.connected = FALSE;
		physical_rcp.version_timeout_id = 0;
		physical_rcp.OSD_menu = FALSE;
		physical_rcp.ABB = FALSE;
		answer_session[98] = '0';

		g_mutex_unlock (&physical_rcp.mutex);

		return G_SOURCE_REMOVE;
	}
}

gpointer update_physical_rcp_in_background (rcp_t *rcp)
{
	g_mutex_lock (&physical_rcp.mutex);

	if (physical_rcp.OSD_menu != rcp->OSD_menu) {
		if (physical_rcp.OSD_menu) {
			send_cam_control_command (rcp, "DUS:1");
			rcp->OSD_menu = TRUE;
		} else {
			send_cam_control_command (rcp, "DUS:0");
			rcp->OSD_menu = FALSE;
		}
	}

	if (physical_rcp.ABB) {
		physical_rcp.ABB = FALSE;
		send_ABB_execution_successful_update_notification ();
	}

	if (physical_rcp.auto_focus != rcp->auto_focus) {
		physical_rcp.auto_focus = rcp->auto_focus;
		send_auto_focus_update_notification ();
	}

	if (physical_rcp.mire != rcp->mire) {
		physical_rcp.mire = rcp->mire;
		send_mire_update_notification ();
	}

	if (physical_rcp.r_gain != rcp->current_scene.r_gain) {
		physical_rcp.r_gain = rcp->current_scene.r_gain;
		send_r_gain_update_notification ();
	}

	if (physical_rcp.b_gain != rcp->current_scene.b_gain) {
		physical_rcp.b_gain = rcp->current_scene.b_gain;
		send_b_gain_update_notification ();
	}

	if (physical_rcp.r_pedestal != rcp->current_scene.r_pedestal) {
		physical_rcp.r_pedestal = rcp->current_scene.r_pedestal;
		send_r_pedestal_update_notification ();
	}

	if (physical_rcp.b_pedestal != rcp->current_scene.b_pedestal) {
		physical_rcp.b_pedestal = rcp->current_scene.b_pedestal;
		send_b_pedestal_update_notification ();
	}

	if (physical_rcp.detail != rcp->current_scene.detail) {
		physical_rcp.detail = rcp->current_scene.detail;
		send_detail_update_notification ();
	}

	if (physical_rcp.master_detail != rcp->current_scene.master_detail) {
		physical_rcp.master_detail = rcp->current_scene.master_detail;
		send_master_detail_update_notification ();
	}

	if (physical_rcp.ND_filter != rcp->current_scene.ND_filter) {
		physical_rcp.ND_filter = rcp->current_scene.ND_filter;
		send_ND_filter_update_notification ();
	}

	if (physical_rcp.gain != rcp->current_scene.gain) {
		physical_rcp.gain = rcp->current_scene.gain;
		send_gain_update_notification ();
	}

	if ((physical_rcp.shutter_type != rcp->current_scene.shutter_type) || \
		((physical_rcp.shutter_type == 1) && (physical_rcp.shutter_step != rcp->current_scene.shutter_step))) {
		physical_rcp.shutter_type = rcp->current_scene.shutter_type;
		physical_rcp.shutter_step = rcp->current_scene.shutter_step;
		send_shutter_update_notification ();
	}

	if (physical_rcp.iris_auto != rcp->current_scene.iris_auto) {
		physical_rcp.iris_auto = rcp->current_scene.iris_auto;
		send_iris_auto_update_notification ();
	}

	g_mutex_unlock (&physical_rcp.mutex);

	return NULL;
}

gboolean update_physical_rcp (rcp_t *rcp)
{
	if (update_physical_rcp_in_background_thread != NULL ) g_thread_join (update_physical_rcp_in_background_thread);

	update_physical_rcp_in_background_thread = g_thread_new (NULL, (GThreadFunc)update_physical_rcp_in_background, rcp);

	return G_SOURCE_REMOVE;
}

gpointer physical_rcp_server (gpointer nothing)
{
#ifdef _WIN32
	int addrlen;
#elif defined (__linux)
	socklen_t addrlen;
#endif
	char buffer[556];
	SOCKET src_socket;
	struct sockaddr_in src_addr;
	int data;
	rcp_t *rcp;
	int value;
	int_widget_t *int_widget;
	guint16 h_port;

	while (physical_rcp_started) {
		addrlen = sizeof (struct sockaddr_in);
		src_socket = accept (physical_rcp.socket, (struct sockaddr *)&src_addr, &addrlen);

		if (src_socket == INVALID_SOCKET) break;

		g_mutex_lock (&physical_rcp.mutex);

		recv (src_socket, buffer, 556, 0);

		if (*((int*)(buffer + 16)) == PTZ_CMD) {
			if (*((int*)(buffer + 24)) == PTZ_SHARP_AXI_CMD) {				//"aw_ptz?cmd=#AXI"
				if (buffer[28] == '&') {															//Iris position query command
					answer_axi[86] = '0' + (physical_rcp.iris >> 8);
					if (answer_axi[86] > '9') answer_axi[86] += 7;
					answer_axi[87] = '0' + ((physical_rcp.iris & 0x0FF) >> 4);
					if (answer_axi[87] > '9') answer_axi[87] += 7;
					answer_axi[88] = '0' + (physical_rcp.iris & 0x00F);
					if (answer_axi[88] > '9') answer_axi[88] += 7;

					send (src_socket, answer_axi, sizeof (answer_axi), 0);
				} else {																			//Iris position control command
					if (buffer[28] <= '9') data = ((int)(buffer[28] - '0')) << 8;
					else data = ((int)(buffer[28] - '7')) << 8;
					if (buffer[29] <= '9') data += ((int)(buffer[29] - '0')) << 4;
					else data += ((int)(buffer[29] - '7')) << 4;
					if (buffer[30] <= '9') data += buffer[30] - '0';
					else data += buffer[30] - '7';

					rcp = rcp_vision;
					if (rcp != NULL) {
						if (rcp->camera_is_on) {
							value = rcp->current_scene.iris + data - physical_rcp.iris;
							if (value < 0x555) value = 0x555;
							if (value > 0xFFF) value = 0xFFF;

							if (rcp->current_scene.iris != value) {
								rcp->current_scene.iris = value;

								send_ptz_control_command_3_digits (rcp, "#AXI", value, FALSE);

								int_widget = g_malloc (sizeof (int_widget_t));
								int_widget->widget = rcp->iris_scale;
								int_widget->handler_id = rcp->iris_handler_id;
								int_widget->value = &rcp->current_scene.iris;
								int_widget->post_action = NO_POST_ACTION;
								int_widget->rcp = rcp;
								g_idle_add ((GSourceFunc)update_scale, int_widget);
							}
						}
					}

					physical_rcp.iris = data;
					answer_axi[86] = buffer[28];
					answer_axi[87] = buffer[29];
					answer_axi[88] = buffer[30];

					send (src_socket, answer_axi, sizeof (answer_axi), 0);

					iris_update_notification[39] = buffer[28];
					iris_update_notification[40] = buffer[29];
					iris_update_notification[41] = buffer[30];

					SEND_UPDATE_NOTIFICATION(iris)
				}
			} else if (*((short*)(buffer + 24)) == PTZ_SHARP_D_CMD) {
				if (buffer[26] == '1') {									//"aw_ptz?cmd=#D1"
					if (buffer[27] == '&') { 														//Auto focus On/Off query command
						if (physical_rcp.auto_focus) answer_d1[85] = '1';
						else answer_d1[85] = '0';

						send (src_socket, answer_d1, sizeof (answer_d1), 0);
					} else {																		//Auto focus On/Off control command
						if (buffer[27] == '1') physical_rcp.auto_focus = TRUE;
						else physical_rcp.auto_focus = FALSE;

						rcp = rcp_vision;
						if (rcp != NULL) {
							if (rcp->camera_is_on) {
								if (rcp->auto_focus != physical_rcp.auto_focus) {
									rcp->auto_focus = physical_rcp.auto_focus;

									if (physical_rcp.auto_focus) send_ptz_control_command (rcp, "#D11");
									else send_ptz_control_command (rcp, "#D10");
								}
							}
						}

						answer_d1[85] = buffer[27];

						send (src_socket, answer_d1, sizeof (answer_d1), 0);

						auto_focus_update_notification[32] = buffer[27];

						SEND_UPDATE_NOTIFICATION(auto_focus)
					}
				} else if (buffer[26] == 'A') {								//"aw_ptz?cmd=#DA"		//Tally On/Off control command
					answer_dA[85] = buffer[27];

					send (src_socket, answer_dA, sizeof (answer_dA), 0);
				}
			} else if (*((int*)(buffer + 24)) == PTZ_SHARP_LPI_CMD) {		//"aw_ptz?cmd=#LPI"		//Lens information query command
				answer_lPI[93] = '0' + (physical_rcp.iris >> 8);
				if (answer_lPI[93] > '9') answer_lPI[93] += 7;
				answer_lPI[94] = '0' + ((physical_rcp.iris & 0x0FF) >> 4);
				if (answer_lPI[94] > '9') answer_lPI[94] += 7;
				answer_lPI[95] = '0' + (physical_rcp.iris & 0x00F);
				if (answer_lPI[95] > '9') answer_lPI[95] += 7;

				send (src_socket, answer_lPI, sizeof (answer_lPI), 0);
			} else if (*((int*)(buffer + 24)) == PTZ_SHARP_O_STANBY_CMD) {	//"aw_ptz?cmd=#O0"		//Power Standby control command
				rcp = rcp_vision;
				if (rcp != NULL) {
					if (rcp->error_code != 0x30) {
						if (rcp->camera_is_on) send_ptz_control_command (rcp, "#O0");
						else send_ptz_control_command (rcp, "#O1");
					}
				}

				send (src_socket, answer_p, sizeof (answer_p), 0);
			} else if (*((int*)(buffer + 24)) == PTZ_SHARP_O_QUERY_CMD) {	//"aw_ptz?cmd=#O"		//Power On/Standby query command
				send (src_socket, answer_p, sizeof (answer_p), 0);
			} else if (*((int*)(buffer + 24)) == PTZ_SHARP_LPC_CMD) {		//"aw_ptz?cmd=#LPC1"	//Lens information notification On/Off control command
				send (src_socket, answer_lPC, sizeof (answer_lPC), 0);
			} else if (*((short*)(buffer + 24)) == PTZ_SHARP_Z_CMD) {		//"aw_ptz?cmd=#Z"		//Zoom (speed control) control command
				rcp = rcp_vision;
				if (rcp != NULL) {
					if (rcp->camera_is_on) {
						zoom_cmd[2] = buffer[26];
						zoom_cmd[3] = buffer[27];

						send_ptz_control_command (rcp, zoom_cmd);
					}
				}

				answer_zS[84] = buffer[26];
				answer_zS[85] = buffer[27];

				send (src_socket, answer_zS, sizeof (answer_zS), 0);
			} else if (*((short*)(buffer + 24)) == PTZ_SHARP_F_CMD) {		//"aw_ptz?cmd=#F"		//Focus (speed control) control command
				rcp = rcp_vision;
				if (rcp != NULL) {
					if (rcp->camera_is_on) {
						focus_cmd[2] = buffer[26];
						focus_cmd[3] = buffer[27];

						send_ptz_control_command (rcp, focus_cmd);
					}
				}

				answer_fS[84] = buffer[26];
				answer_fS[85] = buffer[27];

				send (src_socket, answer_fS, sizeof (answer_fS), 0);
			} else if (*((int*)(buffer + 24)) == PTZ_SHARP_P50_CMD) {		//"aw_ptz?cmd=#P50"		//Pan (speed control) control command
				send (src_socket, answer_pS, sizeof (answer_pS), 0);
			} else if (*((int*)(buffer + 24)) == PTZ_SHARP_T50_CMD) {		//"aw_ptz?cmd=#T50"		//Tilt (speed control) control command
				send (src_socket, answer_tS, sizeof (answer_tS), 0);
			}
		} else if (*((int*)(buffer + 16)) == CAM_CMD) {
			if (*((int*)(buffer + 24)) == CAM_ORS_CMD) {					//"aw_cam?cmd=ORS:"		//Iris Auto/Manual control command
				if (buffer[28] == '1') physical_rcp.iris_auto = TRUE;
				else physical_rcp.iris_auto = FALSE;

				rcp = rcp_vision;
				if (rcp != NULL) {
					if (rcp->camera_is_on) {
						if (rcp->current_scene.iris_auto != physical_rcp.iris_auto) {
							rcp->current_scene.iris_auto = physical_rcp.iris_auto;

							if (physical_rcp.iris_auto) send_cam_control_command_now (rcp, "ORS:1");
							else send_cam_control_command_now (rcp, "ORS:0");

							int_widget = g_malloc (sizeof (int_widget_t));
							int_widget->widget = rcp->iris_auto_toggle_button;
							int_widget->handler_id = rcp->iris_auto_handler_id;
							int_widget->value = &rcp->current_scene.iris_auto;
							int_widget->post_action = IRIS_AUTO_POST_ACTION;
							int_widget->rcp = rcp;
							g_idle_add ((GSourceFunc)update_toggle_button, int_widget);
						}
					}
				}

				answer_ORS[87] = buffer[28];

				send (src_socket, answer_ORS, sizeof (answer_ORS), 0);

				iris_auto_update_notification[34] = buffer[28];

				SEND_UPDATE_NOTIFICATION(iris_auto)
			} else if (*((int*)(buffer + 24)) == CAM_QRS_CMD) {			//"aw_cam?cmd=QRS"		//Iris Auto/Manual query command
				if (physical_rcp.iris_auto) answer_ORS[87] = '1';
				else answer_ORS[87] = '0';

				send (src_socket, answer_ORS, sizeof (answer_ORS), 0);
			} else if (*((int*)(buffer + 24)) == CAM_OTP_CMD) {			//"aw_cam?cmd=OTP:"		//Pedestal control command
				if (buffer[28] <= '9') data = ((int)(buffer[28] - '0')) << 8;
				else data = ((int)(buffer[28] - '7')) << 8;
				if (buffer[29] <= '9') data += ((int)(buffer[29] - '0')) << 4;
				else data += ((int)(buffer[29] - '7')) << 4;
				if (buffer[30] <= '9') data += buffer[30] - '0';
				else data += buffer[30] - '7';

				rcp = rcp_vision;
				if (rcp != NULL) {
					if (rcp->camera_is_on) {
						value = rcp->current_scene.pedestal + data - physical_rcp.pedestal;
						if (value < 0x000) value = 0x000;
						if (value > 0x12C) value = 0x12C;

						if (rcp->current_scene.pedestal != value) {
							rcp->current_scene.pedestal = value;

							send_ptz_control_command_3_digits (rcp, "OTP:", value, FALSE);

							int_widget = g_malloc (sizeof (int_widget_t));
							int_widget->widget = rcp->pedestal_scale;
							int_widget->handler_id = rcp->pedestal_handler_id;
							int_widget->value = &rcp->current_scene.pedestal;
							int_widget->post_action = PEDESTAL_POST_ACTION;
							int_widget->rcp = rcp;
							g_idle_add ((GSourceFunc)update_scale, int_widget);
						}
					}
				}

				physical_rcp.pedestal = data;
				answer_OTP[87] = buffer[28];
				answer_OTP[88] = buffer[29];
				answer_OTP[89] = buffer[30];

				send (src_socket, answer_OTP, sizeof (answer_OTP), 0);

				pedestal_update_notification[34] = buffer[28];
				pedestal_update_notification[35] = buffer[29];
				pedestal_update_notification[36] = buffer[30];

				SEND_UPDATE_NOTIFICATION(pedestal)
			} else if (*((int*)(buffer + 24)) == CAM_QTP_CMD) {			//"aw_cam?cmd=QTP"		//Pedestal query command
				answer_OTP[87] = '0' + (physical_rcp.pedestal >> 8);
				if (answer_OTP[87] > '9') answer_OTP[87] += 7;
				answer_OTP[88] = '0' + ((physical_rcp.pedestal & 0x0FF) >> 4);
				if (answer_OTP[88] > '9') answer_OTP[88] += 7;
				answer_OTP[89] = '0' + (physical_rcp.pedestal & 0x00F);
				if (answer_OTP[89] > '9') answer_OTP[89] += 7;

				send (src_socket, answer_OTP, sizeof (answer_OTP), 0);
			} else if (*((int*)(buffer + 24)) == CAM_ORI_CMD) {			//"aw_cam?cmd=ORI:"		//R gain control command
				if (buffer[28] <= '9') data = ((int)(buffer[28] - '0')) << 8;
				else data = ((int)(buffer[28] - '7')) << 8;
				if (buffer[29] <= '9') data += ((int)(buffer[29] - '0')) << 4;
				else data += ((int)(buffer[29] - '7')) << 4;
				if (buffer[30] <= '9') data += buffer[30] - '0';
				else data += buffer[30] - '7';

				rcp = rcp_vision;
				if (rcp != NULL) {
					if (rcp->camera_is_on) {
						if (rcp->current_scene.r_gain != data) {
							rcp->current_scene.r_gain = data;

							r_gain_cmd[4] = buffer[28];
							r_gain_cmd[5] = buffer[29];
							r_gain_cmd[6] = buffer[30];

							send_cam_control_command_now (rcp, r_gain_cmd);

							int_widget = g_malloc (sizeof (int_widget_t));
							int_widget->widget = rcp->r_gain_scale;
							int_widget->handler_id = rcp->r_gain_handler_id;
							int_widget->value = &rcp->current_scene.r_gain;
							int_widget->post_action = NO_POST_ACTION;
							int_widget->rcp = rcp;
							g_idle_add ((GSourceFunc)update_scale, int_widget);
						}
					}
				}

				physical_rcp.r_gain = data;
				answer_ORI[87] = buffer[28];
				answer_ORI[88] = buffer[29];
				answer_ORI[89] = buffer[30];

				send (src_socket, answer_ORI, sizeof (answer_ORI), 0);

				r_gain_update_notification[34] = buffer[28];
				r_gain_update_notification[35] = buffer[29];
				r_gain_update_notification[36] = buffer[30];

				SEND_UPDATE_NOTIFICATION(r_gain)
			} else if (*((int*)(buffer + 24)) == CAM_QRI_CMD) {			//"aw_cam?cmd=QRI"		//R gain query command
				answer_ORI[87] = '0' + (physical_rcp.r_gain >> 8);
				if (answer_ORI[87] > '9') answer_ORI[87] += 7;
				answer_ORI[88] = '0' + ((physical_rcp.r_gain & 0x0FF) >> 4);
				if (answer_ORI[88] > '9') answer_ORI[88] += 7;
				answer_ORI[89] = '0' + (physical_rcp.r_gain & 0x00F);
				if (answer_ORI[89] > '9') answer_ORI[89] += 7;

				send (src_socket, answer_ORI, sizeof (answer_ORI), 0);
			} else if (*((int*)(buffer + 24)) == CAM_OBI_CMD) {			//"aw_cam?cmd=OBI:"		//B gain control command
				if (buffer[28] <= '9') data = ((int)(buffer[28] - '0')) << 8;
				else data = ((int)(buffer[28] - '7')) << 8;
				if (buffer[29] <= '9') data += ((int)(buffer[29] - '0')) << 4;
				else data += ((int)(buffer[29] - '7')) << 4;
				if (buffer[30] <= '9') data += buffer[30] - '0';
				else data += buffer[30] - '7';

				rcp = rcp_vision;
				if (rcp != NULL) {
					if (rcp->camera_is_on) {
						if (rcp->current_scene.b_gain != data) {
							rcp->current_scene.b_gain = data;

							b_gain_cmd[4] = buffer[28];
							b_gain_cmd[5] = buffer[29];
							b_gain_cmd[6] = buffer[30];

							send_cam_control_command_now (rcp, b_gain_cmd);

							int_widget = g_malloc (sizeof (int_widget_t));
							int_widget->widget = rcp->b_gain_scale;
							int_widget->handler_id = rcp->b_gain_handler_id;
							int_widget->value = &rcp->current_scene.b_gain;
							int_widget->post_action = NO_POST_ACTION;
							int_widget->rcp = rcp;
							g_idle_add ((GSourceFunc)update_scale, int_widget);
						}
					}
				}

				physical_rcp.b_gain = data;
				answer_OBI[87] = buffer[28];
				answer_OBI[88] = buffer[29];
				answer_OBI[89] = buffer[30];

				send (src_socket, answer_OBI, sizeof (answer_OBI), 0);

				b_gain_update_notification[34] = buffer[28];
				b_gain_update_notification[35] = buffer[29];
				b_gain_update_notification[36] = buffer[30];

				SEND_UPDATE_NOTIFICATION(b_gain)
			} else if (*((int*)(buffer + 24)) == CAM_QBI_CMD) {			//"aw_cam?cmd=QBI"		//B gain query command
				answer_OBI[87] = '0' + (physical_rcp.b_gain >> 8);
				if (answer_OBI[87] > '9') answer_OBI[87] += 7;
				answer_OBI[88] = '0' + ((physical_rcp.b_gain & 0x0FF) >> 4);
				if (answer_OBI[88] > '9') answer_OBI[88] += 7;
				answer_OBI[89] = '0' + (physical_rcp.b_gain & 0x00F);
				if (answer_OBI[89] > '9') answer_OBI[89] += 7;

				send (src_socket, answer_OBI, sizeof (answer_OBI), 0);
			} else if (*((int*)(buffer + 24)) == CAM_ORP_CMD) {			//"aw_cam?cmd=ORP:"		//R pedestal control command
				if (buffer[28] <= '9') data = ((int)(buffer[28] - '0')) << 8;
				else data = ((int)(buffer[28] - '7')) << 8;
				if (buffer[29] <= '9') data += ((int)(buffer[29] - '0')) << 4;
				else data += ((int)(buffer[29] - '7')) << 4;
				if (buffer[30] <= '9') data += buffer[30] - '0';
				else data += buffer[30] - '7';

				rcp = rcp_vision;
				if (rcp != NULL) {
					if (rcp->camera_is_on) {
						if (rcp->current_scene.r_pedestal != data) {
							rcp->current_scene.r_pedestal = data;

							r_pedestal_cmd[4] = buffer[28];
							r_pedestal_cmd[5] = buffer[29];
							r_pedestal_cmd[6] = buffer[30];

							send_cam_control_command_now (rcp, r_pedestal_cmd);

							int_widget = g_malloc (sizeof (int_widget_t));
							int_widget->widget = rcp->r_pedestal_scale;
							int_widget->handler_id = rcp->r_pedestal_handler_id;
							int_widget->value = &rcp->current_scene.r_pedestal;
							int_widget->post_action = NO_POST_ACTION;
							int_widget->rcp = rcp;
							g_idle_add ((GSourceFunc)update_scale, int_widget);
						}
					}
				}

				physical_rcp.r_pedestal = data;
				answer_ORP[87] = buffer[28];
				answer_ORP[88] = buffer[29];
				answer_ORP[89] = buffer[30];

				send (src_socket, answer_ORP, sizeof (answer_ORP), 0);

				r_pedestal_update_notification[34] = buffer[28];
				r_pedestal_update_notification[35] = buffer[29];
				r_pedestal_update_notification[36] = buffer[30];

				SEND_UPDATE_NOTIFICATION(r_pedestal)
			} else if (*((int*)(buffer + 24)) == CAM_QRP_CMD) {			//"aw_cam?cmd=QRP"		//R pedestal query command
				answer_ORP[87] = '0' + (physical_rcp.r_pedestal >> 8);
				if (answer_ORP[87] > '9') answer_ORP[87] += 7;
				answer_ORP[88] = '0' + ((physical_rcp.r_pedestal & 0x0FF) >> 4);
				if (answer_ORP[88] > '9') answer_ORP[88] += 7;
				answer_ORP[89] = '0' + (physical_rcp.r_pedestal & 0x00F);
				if (answer_ORP[89] > '9') answer_ORP[89] += 7;

				send (src_socket, answer_ORP, sizeof (answer_ORP), 0);
			} else if (*((int*)(buffer + 24)) == CAM_OBP_CMD) {			//"aw_cam?cmd=OBP:"		//B pedestal control command
				if (buffer[28] <= '9') data = ((int)(buffer[28] - '0')) << 8;
				else data = ((int)(buffer[28] - '7')) << 8;
				if (buffer[29] <= '9') data += ((int)(buffer[29] - '0')) << 4;
				else data += ((int)(buffer[29] - '7')) << 4;
				if (buffer[30] <= '9') data += buffer[30] - '0';
				else data += buffer[30] - '7';

				rcp = rcp_vision;
				if (rcp != NULL) {
					if (rcp->camera_is_on) {
						if (rcp->current_scene.b_pedestal != data) {
							rcp->current_scene.b_pedestal = data;

							b_pedestal_cmd[4] = buffer[28];
							b_pedestal_cmd[5] = buffer[29];
							b_pedestal_cmd[6] = buffer[30];

							send_cam_control_command_now (rcp, b_pedestal_cmd);

							int_widget = g_malloc (sizeof (int_widget_t));
							int_widget->widget = rcp->b_pedestal_scale;
							int_widget->handler_id = rcp->b_pedestal_handler_id;
							int_widget->value = &rcp->current_scene.b_pedestal;
							int_widget->post_action = NO_POST_ACTION;
							int_widget->rcp = rcp;
							g_idle_add ((GSourceFunc)update_scale, int_widget);
						}
					}
				}

				physical_rcp.b_pedestal = data;
				answer_OBP[87] = buffer[28];
				answer_OBP[88] = buffer[29];
				answer_OBP[89] = buffer[30];

				send (src_socket, answer_OBP, sizeof (answer_OBP), 0);

				b_pedestal_update_notification[34] = buffer[28];
				b_pedestal_update_notification[35] = buffer[29];
				b_pedestal_update_notification[36] = buffer[30];

				SEND_UPDATE_NOTIFICATION(b_pedestal)
			} else if (*((int*)(buffer + 24)) == CAM_QBP_CMD) {			//"aw_cam?cmd=QBP"		//B pedestal query command
				answer_OBP[87] = '0' + (physical_rcp.b_pedestal >> 8);
				if (answer_OBP[87] > '9') answer_OBP[87] += 7;
				answer_OBP[88] = '0' + ((physical_rcp.b_pedestal & 0x0FF) >> 4);
				if (answer_OBP[88] > '9') answer_OBP[88] += 7;
				answer_OBP[89] = '0' + (physical_rcp.b_pedestal & 0x00F);
				if (answer_OBP[89] > '9') answer_OBP[89] += 7;

				send (src_socket, answer_OBP, sizeof (answer_OBP), 0);
			} else if (*((int*)(buffer + 24)) == CAM_ODT_CMD) {			//"aw_cam?cmd=ODT:"		//Detail control command
				if (buffer[28] == '1') physical_rcp.detail = TRUE;
				else physical_rcp.detail = FALSE;

				answer_ODT[87] = buffer[28];

				send (src_socket, answer_ODT, sizeof (answer_ODT), 0);

				rcp = rcp_vision;
				if (rcp != NULL) {
					if (rcp->camera_is_on) {
						if (rcp->current_scene.detail != physical_rcp.detail) {
							rcp->current_scene.detail = physical_rcp.detail;

							if (physical_rcp.detail) send_cam_control_command_now (rcp, "ODT:1");
							else send_cam_control_command_now (rcp, "ODT:0");

							int_widget = g_malloc (sizeof (int_widget_t));
							int_widget->widget = rcp->detail_toggle_button;
							int_widget->handler_id = rcp->detail_handler_id;
							int_widget->value = &rcp->current_scene.detail;
							int_widget->post_action = DETAIL_POST_ACTION;
							int_widget->rcp = rcp;
							g_idle_add ((GSourceFunc)update_toggle_button, int_widget);
						}
					}
				}

				detail_update_notification[34] = buffer[28];

				SEND_UPDATE_NOTIFICATION(detail)
			} else if (*((int*)(buffer + 24)) == CAM_QDT_CMD) {			//"aw_cam?cmd=QDT"		//Detail query command
				if (physical_rcp.detail) answer_ODT[87] = '1';
				else answer_ODT[87] = '0';

				send (src_socket, answer_ODT, sizeof (answer_ODT), 0);
			} else if ((*((int*)(buffer + 24)) == CAM_OSA_CMD) && \
					(buffer[28] == '3') && (buffer[29] == '0')) {		//"aw_cam?cmd=OSA:30"
				if (buffer[30] == ':') {														//TOTAL DTL LEVEL control command
					if (buffer[31] <= '9') physical_rcp.master_detail = ((int)(buffer[31] - '0')) << 4;
					else physical_rcp.master_detail = ((int)(buffer[31] - '7')) << 4;
					if (buffer[32] <= '9') physical_rcp.master_detail += buffer[32] - '0';
					else physical_rcp.master_detail += buffer[32] - '7';

					rcp = rcp_vision;
					if (rcp != NULL) {
						if (rcp->camera_is_on) {
							if (rcp->current_scene.master_detail != physical_rcp.master_detail) {
								rcp->current_scene.master_detail = physical_rcp.master_detail;

								master_detail_cmd[7] = buffer[31];
								master_detail_cmd[8] = buffer[32];

								send_cam_control_command_now (rcp, master_detail_cmd);

								int_widget = g_malloc (sizeof (int_widget_t));
								int_widget->widget = rcp->master_detail_scale;
								int_widget->handler_id = rcp->master_detail_handler_id;
								int_widget->value = &rcp->current_scene.master_detail;
								int_widget->post_action = MASTER_DETAIL_POST_ACTION;
								int_widget->rcp = rcp;
								g_idle_add ((GSourceFunc)update_scale, int_widget);
							}
						}
					}

					answer_OSA_30[90] = buffer[31];
					answer_OSA_30[91] = buffer[32];

					send (src_socket, answer_OSA_30, sizeof (answer_OSA_30), 0);

					master_detail_update_notification[37] = buffer[31];
					master_detail_update_notification[38] = buffer[32];

					SEND_UPDATE_NOTIFICATION(master_detail)
				} else {																		//TOTAL DTL LEVEL query command
					answer_OSA_30[90] = '0' + (physical_rcp.master_detail >> 4);
					if (answer_OSA_30[90] > '9') answer_OSA_30[90] += 7;
					answer_OSA_30[91] = '0' + (physical_rcp.master_detail & 0x0F);
					if (answer_OSA_30[91] > '9') answer_OSA_30[91] += 7;

					send (src_socket, answer_OSA_30, sizeof (answer_OSA_30), 0);
				}
			} else if (*((int*)(buffer + 24)) == CAM_OFT_CMD) {			//"aw_cam?cmd=OFT:"		//ND filter control command
				if (buffer[28] == '3') physical_rcp.ND_filter = 2;
				else if (buffer[28] == '4') physical_rcp.ND_filter = 1;
				else physical_rcp.ND_filter = 0;

				rcp = rcp_vision;
				if (rcp != NULL) {
					if (rcp->camera_is_on) {
						if (rcp->current_scene.ND_filter != physical_rcp.ND_filter) {
							rcp->current_scene.ND_filter = physical_rcp.ND_filter;

							ND_filter_cmd[4] = buffer[28];

							send_cam_control_command_now (rcp, ND_filter_cmd);

							int_widget = g_malloc (sizeof (int_widget_t));
							int_widget->widget = rcp->ND_filter_combo_box;
							int_widget->handler_id = rcp->ND_filter_handler_id;
							int_widget->value = &rcp->current_scene.ND_filter;
							int_widget->post_action = NO_POST_ACTION;
							int_widget->rcp = rcp;
							g_idle_add ((GSourceFunc)update_combo_box, int_widget);
						}
					}
				}

				answer_OFT[87] = buffer[28];

				send (src_socket, answer_OFT, sizeof (answer_OFT), 0);

				ND_filter_update_notification[34] = buffer[28];

				SEND_UPDATE_NOTIFICATION(ND_filter)
			} else if (*((int*)(buffer + 24)) == CAM_QFT_CMD) {			//"aw_cam?cmd=QFT"		//ND filter query command
				if (physical_rcp.ND_filter == 2) answer_OFT[87] = '3';
				else if (physical_rcp.ND_filter == 1) answer_OFT[87] = '4';
				else answer_OFT[87] = '0';

				send (src_socket, answer_OFT, sizeof (answer_OFT), 0);
			} else if (*((int*)(buffer + 24)) == CAM_OGU_CMD) {			//"aw_cam?cmd=OGU:"		//Gain control command
				if ((buffer[28] == '8') && (buffer[29] == '0')) physical_rcp.gain = 37;
				else {
					data = ((int)(buffer[28] - '0')) << 4;
					if (buffer[29] <= '9') data += buffer[29] - '0';
					else data += buffer[29] - '7';

					physical_rcp.gain = 0x2C - data;
				}

				rcp = rcp_vision;
				if (rcp != NULL) {
					if (rcp->camera_is_on) {
						if (rcp->current_scene.gain != physical_rcp.gain) {
							rcp->current_scene.gain = physical_rcp.gain;

							gain_cmd[4] = buffer[28];
							gain_cmd[5] = buffer[29];

							send_cam_control_command_now (rcp, gain_cmd);

							int_widget = g_malloc (sizeof (int_widget_t));
							int_widget->widget = rcp->gain_combo_box;
							int_widget->handler_id = rcp->gain_handler_id;
							int_widget->value = &rcp->current_scene.gain;
							int_widget->post_action = NO_POST_ACTION;
							int_widget->rcp = rcp;
							g_idle_add ((GSourceFunc)update_combo_box, int_widget);
						}
					}
				}

				answer_OGU[87] = buffer[28];
				answer_OGU[88] = buffer[29];

				send (src_socket, answer_OGU, sizeof (answer_OGU), 0);

				gain_update_notification[34] = buffer[28];
				gain_update_notification[35] = buffer[29];

				SEND_UPDATE_NOTIFICATION(gain)
			} else if (*((int*)(buffer + 24)) == CAM_QGU_CMD) {			//"aw_cam?cmd=QGU"		//Gain query command
				if (physical_rcp.gain == 37) {
					answer_OGU[87] = '8';
					answer_OGU[88] = '0';
				} else {
					value = 0x2C - physical_rcp.gain;

					answer_OGU[87] = '0' + (value >> 4);
					if (answer_OGU[87] > '9') answer_OGU[87] += 7;
					answer_OGU[88] = '0' + (value & 0x0F);
					if (answer_OGU[88] > '9') answer_OGU[88] += 7;
				}

				send (src_socket, answer_OGU, sizeof (answer_OGU), 0);
			} else if (*((int*)(buffer + 24)) == CAM_OSH_CMD) {			//"aw_cam?cmd=OSH:"		//Shutter control command
				if (buffer[28] == '0') physical_rcp.shutter_type = 0;
				else if (buffer[28] == 'B') physical_rcp.shutter_type = 2;
				else if (buffer[28] == 'C') physical_rcp.shutter_type = 3;
				else {
					physical_rcp.shutter_type = 1;

					if (buffer[28] == '2') physical_rcp.shutter_step = 0;
					else if (buffer[28] == '3') {
						if (output_fps == _59_94fps) physical_rcp.shutter_step = 0;
						else physical_rcp.shutter_step = 1;
					} else if (buffer[28] == '4') physical_rcp.shutter_step = 1;
					else if (buffer[28] == '5') physical_rcp.shutter_step = 2;
					else if (buffer[28] == '6') physical_rcp.shutter_step = 3;
					else if (buffer[28] == '7') physical_rcp.shutter_step = 4;
					else if (buffer[28] == '8') physical_rcp.shutter_step = 5;
					else if (buffer[28] == '9') physical_rcp.shutter_step = 6;
					else if (buffer[28] == 'A') physical_rcp.shutter_step = 7;
					else physical_rcp.shutter_step = 8;
				}

				rcp = rcp_vision;
				if (rcp != NULL) {
					if (rcp->camera_is_on) {
						if ((rcp->current_scene.shutter_type != physical_rcp.shutter_type) || \
							(rcp->current_scene.shutter_step != physical_rcp.shutter_step)) {
							rcp->current_scene.shutter_type = physical_rcp.shutter_type;
							rcp->current_scene.shutter_step = physical_rcp.shutter_step;

							shutter_cmd[4] = buffer[28];

							send_cam_control_command_now (rcp, shutter_cmd);

							int_widget = g_malloc (sizeof (int_widget_t));
							int_widget->widget = rcp->shutter_type_combo_box;
							int_widget->handler_id = rcp->shutter_type_handler_id;
							int_widget->value = &rcp->current_scene.shutter_type;
							int_widget->post_action = SHUTTER_TYPE_POST_ACTION;
							int_widget->rcp = rcp;
							g_idle_add ((GSourceFunc)update_combo_box, int_widget);
						}
					}
				}

				answer_OSH[87] = buffer[28];

				send (src_socket, answer_OSH, sizeof (answer_OSH), 0);

				shutter_update_notification[34] = buffer[28];

				SEND_UPDATE_NOTIFICATION(shutter)
			} else if (*((int*)(buffer + 24)) == CAM_QSH_CMD) {			//"aw_cam?cmd=QSH"		//Shutter query command
				if (physical_rcp.shutter_type == 0) answer_OSH[87] = '0';
				else if (physical_rcp.shutter_type == 2) answer_OSH[87] = 'B';
				else if (physical_rcp.shutter_type == 3) answer_OSH[87] = 'C';
				else {
					if (physical_rcp.shutter_step == 0) {
						if (output_fps == _59_94fps) answer_OSH[87] = '3';
						else answer_OSH[87] = '2';
					} else if (physical_rcp.shutter_step == 1) {
						if ((output_fps == _25fps) || (output_fps == _50fps)) answer_OSH[87] = '3';
						else answer_OSH[87] = '4';
					} else if (physical_rcp.shutter_step == 2) answer_OSH[87] = '5';
					else if (physical_rcp.shutter_step == 3) answer_OSH[87] = '6';
					else if (physical_rcp.shutter_step == 4) answer_OSH[87] = '7';
					else if (physical_rcp.shutter_step == 5) answer_OSH[87] = '8';
					else if (physical_rcp.shutter_step == 6) answer_OSH[87] = '9';
					else if (physical_rcp.shutter_step == 7) answer_OSH[87] = 'A';
					else if (physical_rcp.shutter_step == 8) {
						if (output_fps == _23_98fps) answer_OSH[87] = 'D';
						else if (output_fps == _25fps) answer_OSH[87] = 'E';
						else if (output_fps == _29_97fps) answer_OSH[87] = 'F';
					}
				}

				send (src_socket, answer_OSH, sizeof (answer_OSH), 0);
			} else if (*((int*)(buffer + 24)) == CAM_DCB_CMD) {			//"aw_cam?cmd=DCB:"		//Color bar/Camera control command
				if (buffer[28] == '0') physical_rcp.mire = FALSE;
				else physical_rcp.mire = TRUE;

				rcp = rcp_vision;
				if (rcp != NULL) {
					if (rcp->camera_is_on) {
						if (rcp->mire != physical_rcp.mire) {
							rcp->mire = physical_rcp.mire;

							if (physical_rcp.mire) send_cam_control_command_now (rcp, "DCB:1");
							else send_cam_control_command_now (rcp, "DCB:0");

							int_widget = g_malloc (sizeof (int_widget_t));
							int_widget->widget = rcp->mire_toggle_button;
							int_widget->handler_id = rcp->mire_handler_id;
							int_widget->value = &rcp->mire;
							int_widget->post_action = NO_POST_ACTION;
							int_widget->rcp = rcp;
							g_idle_add ((GSourceFunc)update_toggle_button, int_widget);
						}
					}
				}

				answer_DCB[87] = buffer[28];

				send (src_socket, answer_DCB, sizeof (answer_DCB), 0);

				mire_update_notification[34] = buffer[28];

				SEND_UPDATE_NOTIFICATION(mire)
			} else if (*((int*)(buffer + 24)) == CAM_QBR_CMD) {			//"aw_cam?cmd=QBR"		//Color bar/Camera query command
				if (physical_rcp.mire) answer_OBR[87] = '1';
				else answer_OBR[87] = '0';

				send (src_socket, answer_OBR, sizeof (answer_OBR), 0);
			} else if (*((int*)(buffer + 24)) == CAM_OAF_CMD) {			//"aw_cam?cmd=OAF:"		//Focus Auto/Manual control command
				if (buffer[28] == '0') physical_rcp.auto_focus = FALSE;
				else physical_rcp.auto_focus = TRUE;

				rcp = rcp_vision;
				if (rcp != NULL) {
					if (rcp->camera_is_on) {
						if (rcp->auto_focus != physical_rcp.auto_focus) {
							rcp->auto_focus = physical_rcp.auto_focus;

							if (physical_rcp.auto_focus) send_cam_control_command_now (rcp, "OAF:1");
							else send_cam_control_command_now (rcp, "OAF:0");
						}
					}
				}

				answer_OAF[87] = buffer[28];

				send (src_socket, answer_OAF, sizeof (answer_OAF), 0);

				auto_focus_update_notification[34] = buffer[28];

				SEND_UPDATE_NOTIFICATION(auto_focus)
			} else if (*((int*)(buffer + 24)) == CAM_QAF_CMD) {			//"aw_cam?cmd=QAF"		//Focus Auto/Manual query command
				if (physical_rcp.auto_focus) answer_OAF[87] = '1';
				else answer_OAF[87] = '0';

				send (src_socket, answer_OAF, sizeof (answer_OAF), 0);
			} else if (*((int*)(buffer + 24)) == CAM_QID_CMD) {			//"aw_cam?cmd=QID"		//Model number query command
				send (src_socket, answer_OID, sizeof (answer_OID), 0);
			} else if ((*((int*)(buffer + 24)) == CAM_QSE_CMD) && \
					(buffer[28] == '7') && (buffer[29] == '7')) {		//"aw_cam?cmd=QSE:77"	//Frequency query command
				if (format_is_50Hz) answer_OSE_77[90] = '1';
				else answer_OSE_77[90] = '0';

				send (src_socket, answer_OSE_77, sizeof (answer_OSE_77), 0);
			} else if ((*((int*)(buffer + 24)) == CAM_QSA_CMD) && \
					(buffer[28] == '8') && (buffer[29] == '7')) {		//"aw_cam?cmd=QSA:87"	//Format query command
				answer_OSA_87[90] = settings_array[1].answers[settings_parameters_indexes_array[1]][0];
				if (settings_array[1].answers[settings_parameters_indexes_array[1]][1] == '\0') {
					answer_OSA_87[78] = '8';

					send (src_socket, answer_OSA_87, sizeof (answer_OSA_87) - 2, 0);
				} else {
					answer_OSA_87[91] = settings_array[1].answers[settings_parameters_indexes_array[1]][1];
					answer_OSA_87[78] = '9';

					send (src_socket, answer_OSA_87, sizeof (answer_OSA_87) - 1, 0);
				}
			} else if ((*((int*)(buffer + 24)) == CAM_QSD_CMD) && \
					(buffer[28] == '4') && (buffer[29] == '8')) {		//"aw_cam?cmd=QSD:48"	//Contrast level Picture level query command
				value = picture_level + 0x32;

				answer_OSD_48[90] = '0' + (value >> 4);
				if (answer_OSD_48[90] > '9') answer_OSD_48[90] += 7;
				answer_OSD_48[91] = '0' + (value & 0x0F);
				if (answer_OSD_48[91] > '9') answer_OSD_48[91] += 7;

				send (src_socket, answer_OSD_48, sizeof (answer_OSD_48), 0);
			} else if (*((int*)(buffer + 24)) == CAM_OAS_CMD) {			//"aw_cam?cmd=OAS"		//ABB (ABC) execution control command
				send (src_socket, answer_204, sizeof (answer_204), 0);

				rcp = rcp_vision;
				if (rcp != NULL) {
					if (rcp->camera_is_on && !rcp->camera_is_working) {
						physical_rcp.ABB = TRUE;

						ABB_button_clicked (NULL, rcp);
					} else send_ABB_execution_successful_update_notification ();
				} else send_ABB_execution_successful_update_notification ();
			} else if (*((int*)(buffer + 24)) == CAM_OWS_CMD) {			//"aw_cam?cmd=OWS"		//AWB (AWC) execution control command
				send (src_socket, answer_204, sizeof (answer_204), 0);

				send_AWB_execution_successful_update_notification ();
			} else if (*((int*)(buffer + 24)) == CAM_DUS_CMD) {			//"aw_cam?cmd=DUS:"		//OSD menu On/Off control command
				rcp = rcp_vision;
				if (rcp != NULL) {
					if (rcp->camera_is_on) {
						if (buffer[28] == '1') {
							send_cam_control_command_now (rcp, "DUS:1");
							rcp->OSD_menu = TRUE;

							physical_rcp.OSD_menu = TRUE;
							answer_DUS[87] = '1';
						} else {
							send_cam_control_command_now (rcp, "DUS:0");
							rcp->OSD_menu = FALSE;

							physical_rcp.OSD_menu = FALSE;
							answer_DUS[87] = '0';
						}
					} else {
						physical_rcp.OSD_menu = FALSE;
						answer_DUS[87] = '0';
					}
				} else {
					physical_rcp.OSD_menu = FALSE;
					answer_DUS[87] = '0';
				}

				send (src_socket, answer_DUS, sizeof (answer_DUS), 0);
			} else if (*((int*)(buffer + 24)) == CAM_DPG_CMD) {			//"aw_cam?cmd=DPG:1"	//MENU switch On control command
				rcp = rcp_vision;
				if (rcp != NULL) {
					if (rcp->camera_is_on) send_cam_control_command_now (rcp, "DPG");
				}

				send (src_socket, answer_DPG, sizeof (answer_DPG), 0);
			} else if (*((int*)(buffer + 24)) == CAM_DIT_CMD) {			//"aw_cam?cmd=DIT:1"	//ITEM switch On control command
				rcp = rcp_vision;
				if (rcp != NULL) {
					if (rcp->camera_is_on) send_cam_control_command_now (rcp, "DIT:1");
				}

				send (src_socket, answer_DIT, sizeof (answer_DIT), 0);
			} else if (*((int*)(buffer + 24)) == CAM_DUP_CMD) {			//"aw_cam?cmd=DUP:1"	//YES switch On control command
				rcp = rcp_vision;
				if (rcp != NULL) {
					if (rcp->camera_is_on) send_cam_control_command_now (rcp, "DUP:1");
				}

				send (src_socket, answer_DUP, sizeof (answer_DUP), 0);
			} else if (*((int*)(buffer + 24)) == CAM_DDW_CMD) {			//"aw_cam?cmd=DDW:1"	//NO switch On control command
				rcp = rcp_vision;
				if (rcp != NULL) {
					if (rcp->camera_is_on) send_cam_control_command_now (rcp, "DDW:1");
				}

				send (src_socket, answer_DDW, sizeof (answer_DDW), 0);
			} else if (*((int*)(buffer + 24)) == CAM_XSF_CMD) {			//"aw_cam?cmd=XSF:"		//Scene file control command
				answer_XSF[87] = buffer[28];

				send (src_socket, answer_XSF, sizeof (answer_XSF), 0);
			} else if (*((int*)(buffer + 24)) == CAM_NOTHING_CMD) {		//"aw_cam?cmd=&res=1"	//Internal Server Error
				send (src_socket, answer_error, sizeof (answer_error), 0);
			}
		} else if (*((int*)(buffer + 16)) == EVENT_CONNECT_CMD) {		//"event?connect="
			if (buffer[29] == 'a') {															//Update notification Start
				sscanf (buffer + 41, "%hu&", &h_port);

				physical_rcp.address_for_update_notification.sin_port = htons (h_port);
				physical_rcp.address_for_update_notification.sin_addr.s_addr = src_addr.sin_addr.s_addr;

				physical_rcp.connected = TRUE;

				rcp = rcp_vision;
				if (rcp != NULL) {
					physical_rcp.iris_auto = rcp->current_scene.iris_auto;
					physical_rcp.r_gain = rcp->current_scene.r_gain;
					physical_rcp.b_gain = rcp->current_scene.b_gain;
					physical_rcp.r_pedestal = rcp->current_scene.r_pedestal;
					physical_rcp.b_pedestal = rcp->current_scene.b_pedestal;
					physical_rcp.detail = rcp->current_scene.detail;
					physical_rcp.master_detail = rcp->current_scene.master_detail;
					physical_rcp.ND_filter = rcp->current_scene.ND_filter;
					physical_rcp.gain = rcp->current_scene.gain;
					physical_rcp.shutter_type = rcp->current_scene.shutter_type;
					physical_rcp.shutter_step = rcp->current_scene.shutter_step;
					physical_rcp.mire = rcp->mire;
					physical_rcp.auto_focus = rcp->auto_focus;
				}

				answer_session[98] = '1';

				if (physical_rcp.version_timeout_id == 0) physical_rcp.version_timeout_id = g_timeout_add (60000, send_version_update_notification, NULL);
			} else {																			//Update notification Stop
				if (physical_rcp.version_timeout_id != 0) {
					g_source_remove (physical_rcp.version_timeout_id);
					physical_rcp.version_timeout_id = 0;
				}

				physical_rcp.connected = FALSE;
				physical_rcp.OSD_menu = FALSE;
				physical_rcp.ABB = FALSE;

				answer_session[98] = '0';
			}

			send (src_socket, answer_204, sizeof (answer_204), 0);
		} else if (*((int*)(buffer + 16)) == MAN_SESSION_CMD) {			//"man_session?command=get"	//Registered number of update notifications
			send (src_socket, answer_session, sizeof (answer_session), 0);
		} else if (*((int*)(buffer + 8)) == CAMDATA_CMD) {				//"/live/camdata.html"	//Camera information batch acquisition
			if (settings_array[1].answers[settings_parameters_indexes_array[1]][1] == '\0') {
				answer_camdata[222] = '0';
				answer_camdata[223] = settings_array[1].answers[settings_parameters_indexes_array[1]][0];
			} else {
				answer_camdata[222] = settings_array[1].answers[settings_parameters_indexes_array[1]][0];
				answer_camdata[223] = settings_array[1].answers[settings_parameters_indexes_array[1]][1];
			}

			if (physical_rcp.gain == 37) {
				answer_camdata[252] = '8';
				answer_camdata[253] = '0';
			} else {
				value = 0x2C - physical_rcp.gain;

				answer_camdata[252] = '0' + (value >> 4);
				if (answer_camdata[252] > '9') answer_camdata[252] += 7;
				answer_camdata[253] = '0' + (value & 0x0F);
				if (answer_camdata[253] > '9') answer_camdata[253] += 7;
			}

			if (physical_rcp.shutter_type == 0) answer_camdata[269] = '0';
			else if (physical_rcp.shutter_type == 2) answer_camdata[269] = 'B';
			else if (physical_rcp.shutter_type == 3) answer_camdata[269] = 'C';
			else {
				if (physical_rcp.shutter_step == 0) {
					if (output_fps == _59_94fps) answer_camdata[269] = '3';
					else answer_camdata[269] = '2';
				} else if (physical_rcp.shutter_step == 1) {
					if ((output_fps == _25fps) || (output_fps == _50fps)) answer_camdata[269] = '3';
					else answer_camdata[269] = '4';
				} else if (physical_rcp.shutter_step == 2) answer_camdata[269] = '5';
				else if (physical_rcp.shutter_step == 3) answer_camdata[269] = '6';
				else if (physical_rcp.shutter_step == 4) answer_camdata[269] = '7';
				else if (physical_rcp.shutter_step == 5) answer_camdata[269] = '8';
				else if (physical_rcp.shutter_step == 6) answer_camdata[269] = '9';
				else if (physical_rcp.shutter_step == 7) answer_camdata[269] = 'A';
				else {
					if (output_fps == _23_98fps) answer_camdata[269] = 'D';
					else if (output_fps == _25fps) answer_camdata[269] = 'E';
					else if (output_fps == _29_97fps) answer_camdata[269] = 'F';
				}
			}

			if (physical_rcp.detail) answer_camdata[276] = '1';
			else answer_camdata[276] = '0';

			if (physical_rcp.mire) answer_camdata[290] = '1';
			else answer_camdata[290] = '0';

			if (physical_rcp.auto_focus) answer_camdata[324] = '1';
			else answer_camdata[324] = '0';

			if (physical_rcp.iris_auto) answer_camdata[329] = '1';
			else answer_camdata[329] = '0';

			answer_camdata[344] = '0' + (physical_rcp.master_detail >> 4);
			if (answer_camdata[344] > '9') answer_camdata[344] += 7;
			answer_camdata[345] = '0' + (physical_rcp.master_detail & 0x0F);
			if (answer_camdata[345] > '9') answer_camdata[345] += 7;

			value = ((physical_rcp.iris - 0x555) * 0xFF) / 0xAAA;
			answer_camdata[370] = '0' + (value >> 4);
			if (answer_camdata[370] > '9') answer_camdata[370] += 7;
			answer_camdata[371] = '0' + (value & 0x0F);
			if (answer_camdata[371] > '9') answer_camdata[371] += 7;

			send (src_socket, answer_camdata, sizeof (answer_camdata), 0);
		}

		g_mutex_unlock (&physical_rcp.mutex);

		closesocket (src_socket);
	}

	return NULL;
}

void init_physical_rcp (void)
{
	memset (&physical_rcp.address, 0, sizeof (struct sockaddr_in));
	physical_rcp.address.sin_family = AF_INET;
	physical_rcp.address.sin_port = htons (PHYSICAL_RCP_TCP_PORT);
	physical_rcp.address.sin_addr.s_addr = inet_addr (my_ip_address);

	memset (&physical_rcp.address_for_update_notification, 0, sizeof (struct sockaddr_in));
	physical_rcp.address_for_update_notification.sin_family = AF_INET;
	physical_rcp.address_for_update_notification.sin_port = htons (35200);
	physical_rcp.address_for_update_notification.sin_addr.s_addr = INADDR_NONE;

	g_mutex_init (&physical_rcp.mutex);
}

void start_physical_rcp (void)
{
	physical_rcp.socket = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (bind (physical_rcp.socket, (struct sockaddr *)&physical_rcp.address, sizeof (struct sockaddr_in)) == 0) {
		if (listen (physical_rcp.socket, 1) == 0) {
			physical_rcp_started = TRUE;
			physical_rcp_thread = g_thread_new (NULL, physical_rcp_server, NULL);
		}
	}
}

void stop_physical_rcp (void)
{
	physical_rcp_started = FALSE;

	shutdown (physical_rcp.socket, SHUT_RD);
	closesocket (physical_rcp.socket);

	g_mutex_lock (&physical_rcp.mutex);

	physical_rcp.connected = FALSE;
	if (physical_rcp.version_timeout_id != 0) {
		g_source_remove (physical_rcp.version_timeout_id);
		physical_rcp.version_timeout_id = 0;
	}
	physical_rcp.OSD_menu = FALSE;
	physical_rcp.ABB = FALSE;

	answer_session[98] = '0';

	g_mutex_unlock (&physical_rcp.mutex);

	if (physical_rcp_thread != NULL) {
		g_thread_join (physical_rcp_thread);
		physical_rcp_thread = NULL;
	}
}

