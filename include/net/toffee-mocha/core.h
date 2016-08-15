/*
TOFFEE-Mocha provides dual licenses, designed to meet the usage and distribution requirements of different types of users.

GPLv2 License: Copyright (C) (2016) Kiran Kankipati (kiran.kankipati@gmail.com) All Rights Reserved.
TOFFEE-Mocha is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License Version 2, and not any other version, as published by the Free Software Foundation.
TOFFEE-Mocha is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.You should have received a copy of the GNU General Public License along with TOFFEE-Mocha; see the file COPYING.
If not, write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

BSD License (2-clause license): Copyright (2016) Kiran Kankipati. All rights reserved.
Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
   1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY KIRAN KANKIPATI ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL KIRAN KANKIPATI OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those of the authors and should not be interpreted as representing official policies, either expressed or implied, of Kiran Kankipati.

* This license is applicable exclusive to the TOFFEE-Mocha components. TOFFEE-Mocha may bundle or include other third-party open-source components. Kindly refer these component README and COPYRIGHT/LICENSE documents, released by its respective authors and project/module owners.
** For more details about Third-party components, you can also kindly refer TOFFEE-Mocha project website About page.
*/
#ifndef _TOFFEE_MOCHA_CONFIG_H
#define _TOFFEE_MOCHA_CONFIG_H
#include <linux/types.h>
#include <linux/ip.h>
#include <linux/in.h>
#include <linux/in6.h>
#include <linux/spinlock.h>
#include <net/inet_sock.h>
#include <net/snmp.h>

#define TF_VERSION "TOFFEE-Mocha-1.0.14"

#define TF_TRUE    1
#define TF_FALSE   0


typedef unsigned char  	BYTE;
typedef unsigned short 	BYTEx2;
typedef __u32	BYTEx4;
typedef __u64	BYTEx8;

#define TF_PORT_NONE -1
extern int G_tf_lan_port_ifindex;
extern int G_tf_wan_port_ifindex;
extern int G_tfm_pkt_ms_delay;
extern int G_tfm_pkt_us_delay;
extern int G_tfm_pkt_dynamic_delay_en;
extern int G_tfm_pkt_dynamic_delay_factor;
extern int G_tfm_pkt_random_delay_en;
extern int G_tfm_pkt_random_delay_factor;
extern int G_tfm_pkt_random_drop_en;
extern int G_tfm_pkt_random_drop_factor;

bool match_ip(BYTE *ip1, BYTE *ip2);

#endif
