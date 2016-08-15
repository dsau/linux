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
#include <linux/types.h>
#include <linux/mm.h>
#include <linux/skbuff.h>
#include <linux/spinlock.h>
#include <linux/netdevice.h>
#include <linux/ctype.h>
#include <linux/time.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <net/toffee-mocha/core.h>
#include <net/toffee-mocha/stats.h>
#include <net/toffee-mocha/proc.h>
#include <net/toffee-mocha/engine.h>

static int get_dev_ifindex(char *name);
static void proc_int_var_set(BYTE *value, int *int_var, BYTE *int_var_name, int max_value);

struct proc_dir_entry *tf_proc_dir;
struct proc_dir_entry *tf_proc_stats;
struct proc_dir_entry *tf_proc_config;
struct proc_dir_entry *tf_proc_io;
struct proc_dir_entry *tf_proc_info;

char *proc_buf;

#define PROCFS_MAX_SIZE 3048

static ssize_t tf_proc_io_write(struct file *fp, const char *buf, size_t len, loff_t * off)
{
	BYTE *val = NULL;   //Pointer pointing value

	int i;
	int return_char_ctr = 0;
	
	if (len > PROCFS_MAX_SIZE) { return -EFAULT; }
	if (copy_from_user(proc_buf, buf, len)) { return -EFAULT; }
	if (proc_buf == NULL || len > 80) { goto tf_proc_io_write_END; }
	*(proc_buf + len) = '\0'; len++; //add \0 at the end of buffer
	//Contents format: "<command>,<value>\n"

	for (i = 0; i < len; i++)
	{
		if (*(proc_buf + i) == ',' && (i + 1) < len) { val = (proc_buf + i + 1); }
		if (*(proc_buf + i) == ',') { *(proc_buf + i) = '\0'; }
		if (*(proc_buf + i) == '\n') { *(proc_buf + i) = '\0'; return_char_ctr++; }
	}
	if (return_char_ctr == 0 || return_char_ctr > 1) { goto tf_proc_io_write_END; }

	if (!strcmp(proc_buf, "pv_tf_lan_port_pv"))
	{
		G_tf_lan_port_ifindex = get_dev_ifindex(val);
		printk("@ ASSIGN_LAN [%s] - ifindex: %d\n", val, G_tf_lan_port_ifindex);
		if (G_tf_wan_port_ifindex == G_tf_lan_port_ifindex) { if (G_tf_wan_port_ifindex != TF_PORT_NONE) { G_tf_wan_port_ifindex = TF_PORT_NONE; } }
	}
	else if (!strcmp(proc_buf, "pv_tf_wan_port_pv"))
	{
		G_tf_wan_port_ifindex = get_dev_ifindex(val);
		printk("@ ASSIGN_WAN [%s] - ifindex: %d\n", val, G_tf_wan_port_ifindex);
		if (G_tf_lan_port_ifindex == G_tf_wan_port_ifindex) { if (G_tf_lan_port_ifindex != TF_PORT_NONE) { G_tf_lan_port_ifindex = TF_PORT_NONE; } }
	}
	else if (!strcmp(proc_buf, "pv_tfm_pkt_ms_delay_pv")) { proc_int_var_set(val, &G_tfm_pkt_ms_delay, proc_buf, 999); }
	else if (!strcmp(proc_buf, "pv_tfm_pkt_us_delay_pv")) { proc_int_var_set(val, &G_tfm_pkt_us_delay, proc_buf, 999); }
	else if (!strcmp(proc_buf, "pv_tfm_pkt_dynamic_delay_en_pv")) { proc_int_var_set(val, &G_tfm_pkt_dynamic_delay_en, proc_buf, 0); }
	else if (!strcmp(proc_buf, "pv_tfm_pkt_dynamic_delay_factor_pv")) { proc_int_var_set(val, &G_tfm_pkt_dynamic_delay_factor, proc_buf, 10); }
	else if (!strcmp(proc_buf, "pv_tfm_pkt_random_drop_en_pv")) { proc_int_var_set(val, &G_tfm_pkt_random_drop_en, proc_buf, 0); }
	else if (!strcmp(proc_buf, "pv_tfm_pkt_random_drop_factor_pv")) { proc_int_var_set(val, &G_tfm_pkt_random_drop_factor, proc_buf, 10); }
tf_proc_io_write_END:
	return len;
}

#ifdef CONFIG_TOFFEE_MOCHA_STATS
static ssize_t tf_proc_stats_read(struct file *fp, char *buf, size_t len, loff_t * off)
{
	static int finished = 0; if (finished) { finished = 0; return 0; } finished = 1;
	strcpy(buf, "");

	sprintf(buf, "bytes|%u|%u|%u|%u\n", atomic_read(&tf_stats_lan_rx_bytes), atomic_read(&tf_stats_lan_tx_bytes), atomic_read(&tf_stats_wan_rx_bytes), atomic_read(&tf_stats_wan_tx_bytes));
	atomic_set(&tf_stats_lan_rx_bytes, 0); atomic_set(&tf_stats_lan_tx_bytes, 0); atomic_set(&tf_stats_wan_rx_bytes, 0); atomic_set(&tf_stats_wan_tx_bytes, 0);

	sprintf(buf, "%spkts|%u|%u|%u|%u\n", buf, atomic_read(&tf_stats_lan_rx_pkts), atomic_read(&tf_stats_lan_tx_pkts), atomic_read(&tf_stats_wan_rx_pkts), atomic_read(&tf_stats_wan_tx_pkts));
	atomic_set(&tf_stats_lan_rx_pkts, 0); atomic_set(&tf_stats_lan_tx_pkts, 0); atomic_set(&tf_stats_wan_rx_pkts, 0); atomic_set(&tf_stats_wan_tx_pkts, 0);

	return strlen(buf);
}
#endif

static ssize_t tf_proc_config_read(struct file *fp, char *buf, size_t len, loff_t * off)
{
	bool found_lan_port = false; bool found_wan_port = false;
	static int finished = 0;
	struct net_device *dev = first_net_device(&init_net);
	
	if (finished) { finished = 0; return 0; } finished = 1;
	strcpy(buf, "");

	while (dev)
	{
		if (dev->ifindex == G_tf_lan_port_ifindex) { sprintf(buf, "%sport mode %s [%d] lan\n", buf, dev->name, G_tf_lan_port_ifindex); found_lan_port = true; }
		else if (dev->ifindex == G_tf_wan_port_ifindex) { sprintf(buf, "%sport mode %s [%d] wan\n", buf, dev->name, G_tf_wan_port_ifindex); found_wan_port = true; }
		dev = next_net_device(dev);
	}

	if (!found_lan_port) sprintf(buf, "%sport mode none [%d] lan\n", buf, G_tf_lan_port_ifindex);
	if (!found_wan_port) sprintf(buf, "%sport mode none [%d] wan\n", buf, G_tf_wan_port_ifindex);
	sprintf(buf, "%spacket ms delay %d\n", buf, G_tfm_pkt_ms_delay);
	sprintf(buf, "%spacket us delay %d\n", buf, G_tfm_pkt_us_delay);
	if (G_tfm_pkt_dynamic_delay_en) sprintf(buf, "%spacket dynamic delay enable\n", buf); else sprintf(buf, "%spacket dynamic delay disable\n", buf);
	sprintf(buf, "%spacket delay factor %d\n", buf, G_tfm_pkt_dynamic_delay_factor);
	if (G_tfm_pkt_random_drop_en) sprintf(buf, "%spacket random drop enable\n", buf); else sprintf(buf, "%spacket random drop disable\n", buf);
	sprintf(buf, "%spacket random drop %d\n", buf, G_tfm_pkt_random_drop_factor);
	return strlen(buf);
}

#ifdef CONFIG_TOFFEE_MOCHA_PROCINFO
static ssize_t tf_proc_info_read(struct file *fp, char *buf, size_t len, loff_t * off)
{
	static int finished = 0; if (finished) { finished = 0; return 0; } finished = 1;
	strcpy(buf, "");

	sprintf(buf, "%sVersion: %s\n", buf, TF_VERSION);

	sprintf(buf, "%s\nCompile options:\n", buf);
	sprintf(buf, "%s--------------------------------------------\n", buf);

	sprintf(buf, "%sStatistics: ", buf);
#ifdef CONFIG_TOFFEE_MOCHA_STATS
	sprintf(buf, "%sEnabled\n", buf);
#else
	sprintf(buf, "%sDisabled\n", buf);
#endif

	sprintf(buf, "%s--------------------------------------------\n", buf);
	return strlen(buf);
}
#endif


static struct file_operations tf_proc_config_fops = { .owner = THIS_MODULE,.read = tf_proc_config_read, };
static struct file_operations tf_proc_io_fops = { .owner = THIS_MODULE,.write = tf_proc_io_write, };

#ifdef CONFIG_TOFFEE_MOCHA_STATS
static struct file_operations tf_proc_stats_fops = { .owner = THIS_MODULE,.read = tf_proc_stats_read, };
#endif

#ifdef CONFIG_TOFFEE_MOCHA_PROCINFO
static struct file_operations tf_proc_info_fops = { .owner = THIS_MODULE,.read = tf_proc_info_read, };
#endif

void tf_init_proc(void)
{
	proc_buf = kmalloc(sizeof(u8)*PROCFS_MAX_SIZE, GFP_ATOMIC); if (proc_buf == NULL) { printk("proc_buf alloc failed!\n"); }

	tf_proc_dir = proc_mkdir(proc_dir_name, NULL);
	tf_proc_config = proc_create(proc_file_config_name, 0444, tf_proc_dir, &tf_proc_config_fops); if (tf_proc_config == NULL) { printk(KERN_ALERT "Error: Could not initialize %s\n", proc_file_config_name); }
	tf_proc_io = proc_create(proc_file_io_name, 0222, tf_proc_dir, &tf_proc_io_fops); if (tf_proc_io == NULL) { printk(KERN_ALERT "Error: Could not initialize %s\n", proc_file_io_name); }

#ifdef CONFIG_TOFFEE_MOCHA_STATS
	tf_proc_stats = proc_create(proc_file_stats_name, 0444, tf_proc_dir, &tf_proc_stats_fops); if (tf_proc_stats == NULL) { printk(KERN_ALERT "Error: Could not initialize %s\n", proc_file_stats_name); }
#endif

#ifdef CONFIG_TOFFEE_MOCHA_PROCINFO
	tf_proc_info = proc_create(proc_file_info_name, 0444, tf_proc_dir, &tf_proc_info_fops); if (tf_proc_info == NULL) { printk(KERN_ALERT "Error: Could not initialize %s\n", proc_file_info_name); }
#endif

}

int get_dev_ifindex(char *name)
{
	struct net_device *dev = dev_get_by_name(&init_net, name);
	if (dev != NULL) { return (int)dev->ifindex; }
	return TF_PORT_NONE;
}

void proc_int_var_set(BYTE *value, int *int_var, BYTE *int_var_name, int max_value)
{
	unsigned int value_int = simple_strtoul(value, NULL, 10);
	if (value_int < 0) return;
	if (max_value != 0) { if (value_int > max_value) return; }
	if ((*int_var) == value_int) { return; }
	printk("BEFORE: %s = %d\n", int_var_name, (*int_var));
	(*int_var) = value_int;
	printk("AFTER: %s = %d\n", int_var_name, (*int_var));
}
