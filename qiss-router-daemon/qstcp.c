// qstcp.c

#include "qstcp.h"

static bool plc_on;

static unsigned int ipmask[33]; 

//jdg: about: vmalloc, memset
//
// http://www.makelinux.net/ldd3/chp-8-sect-4
//	#include <linux/vmalloc.h>
//	void * vmalloc(unsigned long size)	// defined in: inmm/vmalloc.c -- Usage is identical to user-space's malloc()
//
//	"kmalloc is the preferred way, as long as you don't need very big areas. The trouble is, if you
//	 want to do DMA from/to some hardware device, you'll need to use kmalloc, and you'll probably
//	 need bigger chunk. The solution is to allocate memory as soon as possible, before memory gets fragmented."
//
// http://man7.org/linux/man-pages/man3/memset.3.html
//	#include <string.h>
//	void *memset(void *s, int c, size_t n);
//
// TIP: get book "Linux Device Drivers"
// info:			http://lwn.net/Kernel/LDD3/
// download:	http://lwn.net/images/pdf/LDD3/ldd3_pdf.tar.bz2

//jdg: about: snprintf
// #include <stdio.h>
// int snprintf(char *str, size_t size, const char *format, ...);

// update time stamp
static unsigned int func_get_time_stamp(void)
{
	unsigned int sec;
	unsigned int usec;
	//
	memset(time_stamp, 0, TIME_LEN);
	do_gettimeofday( &(struct_txc.time) );
	//rtc_time_to_tm(struct_txc.time.tv_sec,&tm);
	//snprintf(time_stamp, TIME_LEN ,"UTC time :%d-%d-%d %d:%d:%d \n",tm.tm_year+1900,tm.tm_mon, tm.tm_mday,tm.tm_hour,tm.tm_min,tm.tm_sec);
	//
	//jdg: add explicit typecast
	//jdg: warning: ISO C90 forbids mixed declarations and code [-Wdeclaration-after-statement]
	//unsigned int sec = (unsigned int)struct_txc.time.tv_sec;
	//unsigned int usec = (unsigned int)struct_txc.time.tv_usec;
	sec = (unsigned int)struct_txc.time.tv_sec;
	usec = (unsigned int)struct_txc.time.tv_usec;
	//
	snprintf(time_stamp, TIME_LEN ,"{\"timestamp\":{\"seconds\":%u,\"microseconds\":%u}}", sec, usec);
	return 0;
}

//jdg: todo: rewrite func_iptonui()
//	lets use a standard lib function ?
//
//	> grep in4_pton /usr/src/linux-headers-3.16.0-4-common/include/linux/inet.h
//	extern int in4_pton(const char *src, int srclen, u8 *dst, int delim, const char **end);
//
//	ftp://ftp.ecn.purdue.edu/xzl/software/wear/MobiSys16/msm/net/core/utils.c
//	/**
//	 * in4_pton - convert an IPv4 address from literal to binary representation
//	 * @src: the start of the IPv4 address string
//	 * @srclen: the length of the string, -1 means strlen(src)
//	 * @dst: the binary (u8[4] array) representation of the IPv4 address
//	 * @delim: the delimiter of the IPv4 address in @src, -1 means no delimiter
//	 * @end: A pointer to the end of the parsed string will be placed here
//	 *
//	 * Return one on success, return zero when any error occurs
//	 * and @end will point to the end of the parsed string.
//	 *
//	 */
//	int in4_pton(const char *src, int srclen,
//					 u8 *dst,
//					 int delim, const char **end) ;
//
//jdg: pfffff -- cant find proper manual for int4_pton ... lets just use the old in_aton

unsigned int func_iptonui_test_compare(char * ipd)
{
	unsigned int my_ipAddress;
	my_ipAddress = in_aton(ipd);
	return my_ipAddress;
} // unsigned int func_iptonui_test_compare(char * ipd)

// parse ip string in dot "xx.xx.xx.xx"
unsigned int func_iptonui(char * ipd)
{
	unsigned int int_num;
	unsigned digi_ind;
	char num_str[16];
	unsigned int ip_num;
	unsigned int ind;
	unsigned int cnr;
	
	//jdg: compile-time warning: passing argument 2 of ‘simple_strtoul’ from incompatible pointer type [enabled by default]
	//char after;
	//jdg: replaced 'char after;' in entire file to 'char *after;' as used in 'simple_strtoul()'
	//jdg: NOTE: this seems to have fixed the bug that the kernel hang when writing new policer table 
	char *after;

	int_num = 0;
	//unsigned int temp_array[4]={0};
	digi_ind = 0;
	ip_num = 0;
	//unsigned int anum= 0;	//jdg: compile-time warning: unused variable ‘anum’ [-Wunused-variable]
	ind = 0;
	cnr = 0;
	
	while ( (ind<16) && (int_num<4) )
	{
		//printk("ind: %u int_num: %u int_array is: %u.%u.%u.%u\n", ind, int_num, temp_array[0],temp_array[1],temp_array[2],temp_array[3]);
		while ( (ipd[ind] != '.') && (ind<16) )
		{
			num_str[digi_ind] = ipd[ind];
			ind++;
			digi_ind++;
			//printk("ind: %u int_num: %u int_array is: %u.%u.%u.%u\n", ind, int_num, temp_array[0],temp_array[1],temp_array[2],temp_array[3]);
		}
		
		num_str[digi_ind] = '\0';	//jdg: add end-of-string character: this fix the bug of malformed-ip-address
		
		digi_ind = 0;
		//
		//jdg: compile-time warning: passing argument 2 of ‘simple_strtoul’ from incompatible pointer type [enabled by default]
		//cnr = (unsigned int)(simple_strtoul( num_str, &after, 10) );

//jdg: find func prototype for simple_strtoul
//		> grep -B2 simple_strtoul /usr/src/linux-headers-3.16.0-4-common/include/linux/kernel.h
//		*
//		* Returns 0 on success, -ERANGE on overflow and -EINVAL on parsing error.
//		* Used as a replacement for the obsolete simple_strtoull. Return code must
//		--
//		*
//		* Returns 0 on success, -ERANGE on overflow and -EINVAL on parsing error.
//		* Used as a replacement for the obsolete simple_strtoull. Return code must
//		--
//		/* Obsolete, do not use.  Use kstrto<foo> instead */
//				
//		extern unsigned long simple_strtoul(const char *,char **,unsigned int);
		
		cnr = (unsigned int) simple_strtoul( num_str, &after, 10) ;
		
		//printk("this cnr = %u\n",cnr);
		//printk("num_str=%s cnr=%u\n",num_str,cnr); //jdg: used to find bug
		//temp_array[int_num] = cnr;
		
		ip_num += ( cnr<< (int_num * 8) );
		
		//printk("find num %lu saved in pos %u\n", temp_array[int_num], int_num);
		//printk("int_array is %u.%u.%u.%u\n", temp_array[0],temp_array[1],temp_array[2],temp_array[3]);
		int_num++;
		ind++;
	}
	
	//printk("find all num %u.%u.%u.%u\n", temp_array[0],temp_array[1],temp_array[2],temp_array[3]);
	
	//printk("return %u\n",ip_num);
	
	//jdg: compile-time warning: ISO C90 forbids mixed declarations and code [-Wdeclaration-after-statement]
	//jdg: compile-time warning: unused variable ‘ipdt’ [-Wunused-variable]
	//char * ipdt = (unsigned char *)&(ip_num);
	//
	//printk("ip in dot form is %d.%d.%d.%d\n",ipdt[0]&0xff,ipdt[1]&0xff,ipdt[2]&0xff,ipdt[3]&0xff);
	return ip_num;
	
} // unsigned int func_iptonui(char * ipd)

// get policy
static bool func_get_policy(void)
{
	if (this_flow_len > tcpl[this_flow_ind][tcpl_pbucket])
	{
		this_flow_color = FLOW_RED;
		//printk("peak bucket empty\n");
		return false;
	}
	if (this_flow_len > tcpl[this_flow_ind][tcpl_abucket])
	{
		this_flow_color = FLOW_YELLOW;
		//printk("avg bucket empty\n");
		return false;
	}
	this_flow_color = FLOW_GREEN;
	return true;
} // static bool func_get_policy()

// update flow
//jdg: compile-time warning: no return statement in function returning non-void [-Wreturn-type]
//static unsigned int func_update_flow(bool plc)
void func_update_flow(bool plc)
{
	if (plc)
	{
		tcpl[this_flow_ind][tcpl_fcount]		+= this_flow_len;
		tcpl[this_flow_ind][tcpl_abucket]	-= this_flow_len;
		tcpl[this_flow_ind][tcpl_pbucket]	-= this_flow_len;
	}
	else
	{
		tcpl[this_flow_ind][tcpl_lcount]		+= this_flow_len;
	}
	tcpl[this_flow_ind][tcpl_time_usec]		= this_flow_usec;
	tcpl[this_flow_ind][tcpl_time_sec]		= this_flow_sec;
}

// add new flow line in tcpl and tcpo
//static bool func_add_flow (unsigned int fd)		//jdg
static bool func_add_flow(unsigned int fd)
{
	unsigned int tp_id;
	bool find_tp;

	tcpo[fd][tcpo_sid]			= this_flow_sid;
	tcpo[fd][tcpo_did]			= this_flow_did;
	tcpo[fd][tcpo_dir]			= this_flow_dir;
	tcpo[fd][tcpo_fic]			= this_flow_fic;
	
	tcpl[fd][tcpl_sid]			= this_flow_sid;
	tcpl[fd][tcpl_did]			= this_flow_did;
	tcpl[fd][tcpl_dir]			= this_flow_dir;
	tcpl[fd][tcpl_fic]			= this_flow_fic;
	
	tcpl[fd][tcpl_fcount]		= 0;
	tcpl[fd][tcpl_lcount]		= 0;
	tcpl[fd][tcpl_time_sec]		= this_flow_sec;
	tcpl[fd][tcpl_time_usec]	= this_flow_usec;
	
	tcpl[fd][tcpl_fsum]			= 0;
	tcpl[fd][tcpl_lsum]			= 0;
	
	tcpl[fd][tcpl_on]				= 1;
	
	//jdg: compile-time warning: ISO C90 forbids mixed declarations and code [-Wdeclaration-after-statement]
	//unsigned int tp_id = 0;
	tp_id = 0;
	//
	find_tp = false;
	
	while (tp_id < MAX_DAS)
	{
		if ( (tcpl[fd][tcpo_sid] == tp_plc[tp_id][plc_sid]) || (tp_plc[tp_id][plc_sid] == 0) )
		{
			if ( tcpl[fd][tcpo_did] == tp_plc[tp_id][plc_did] )
			{
				if ( tcpl[fd][tcpo_dir] == tp_plc[tp_id][plc_dir] )
				{
					find_tp=true;
					tcpl[fd][tcpl_arate]		= tp_plc[tp_id][plc_avg]   * 1000;
					tcpl[fd][tcpl_prate]		= tp_plc[tp_id][plc_peak]  * 1000;
					tcpl[fd][tcpl_avg_max]	= tp_plc[tp_id][plc_abkt]  * 1000;
					tcpl[fd][tcpl_peak_max]	= tp_plc[tp_id][plc_pbkt]	* 1000;
					tcpl[fd][tcpl_abucket]	= tp_plc[tp_id][plc_abkt]  * 1000;
					tcpl[fd][tcpl_pbucket]	= tp_plc[tp_id][plc_pbkt]  * 1000;
					break;
				}
			}
		}
		
		tp_id++;
	}
	
	if ((this_flow_fic == 0)&&(find_tp == false))
	{
		tcpl[fd][tcpl_arate]		= AVG_RT_U;
		tcpl[fd][tcpl_prate]		= PEK_RT_U;
		tcpl[fd][tcpl_avg_max]	= AVG_BK_U;
		tcpl[fd][tcpl_peak_max]	= PEK_BK_U;
		tcpl[fd][tcpl_abucket]	= AVG_BK_U;
		tcpl[fd][tcpl_pbucket]	= PEK_BK_U;
		tcpl[fd][tcpl_on]=1;
	}
	else if (find_tp == false)
	{
		tcpl[fd][tcpl_arate]		= AVG_RT_R;
		tcpl[fd][tcpl_prate]		= PEK_RT_R;
		tcpl[fd][tcpl_avg_max]	= AVG_BK_R;
		tcpl[fd][tcpl_peak_max]	= PEK_BK_R;
		tcpl[fd][tcpl_abucket]	= AVG_BK_R;
		tcpl[fd][tcpl_pbucket]	= PEK_BK_R;
		tcpl[fd][tcpl_on]=1;
	}
	else;
	
	return true;
	
} // static bool func_add_flow(unsigned int fd)

// get flow index with packet info
//static unsigned int func_get_flow_ind (void)		//jdg
static unsigned int func_get_flow_ind(void)
{
	unsigned int fd = 0;
	unsigned int newfd = MAX_FLOW;
	bool check_new = true;
	while (fd<MAX_FLOW)
	{
		if (	( this_flow_sid == tcpl[fd][tcpl_sid] ) &&
				( this_flow_did == tcpl[fd][tcpl_did] ) &&
				( this_flow_dir == tcpl[fd][tcpl_dir] ) &&
				( this_flow_fic == tcpl[fd][tcpl_fic] )		)
		{
			//printk("flow found\n"); //for test
			return fd;
		}
		else
		{
			if (check_new)
			{
				if (tcpl[fd][tcpl_on] == 0)
				{
					newfd=fd;
					check_new=false;
				}
			}
		}
		fd++;
	}
	if (check_new)
	{
		return MAX_FLOW;
	}
	else
	{
		//func_add_flow (newfd);		//jdg
		func_add_flow(newfd);
		return newfd;
	}
	
} // static unsigned int func_get_flow_ind(void)

// refill bucket
static bool func_refill_bucket(unsigned int pd)
{
	//jdg: replace int div by do_div():
	if (bool_small_tp_effect) small_bucket = SMALL_TP * pd / 1000;
	//if (bool_small_tp_effect) small_bucket = do_div(SMALL_TP * pd, 1000);
	
	/* for refill in regular time
	 unsigned int fd = 0;
	 while (fd<MAX_FLOW)
	 {
	 if (tcpl[fd][tcpl_on])
	 {
	 tcpl[fd][tcpl_bucket]+=tcpl[fd][tcpl_avg] * pd / 1000;
	 if (tcpl[fd][tcpl_bucket]>(tcpl[fd][tcpl_peak]* pd / 1000))
	 {
	 tcpl[fd][tcpl_bucket] = tcpl[fd][tcpl_peak] * pd / 1000;
	 }
	 }
	 fd++;
	 }
	 */
	return true;
	
} // static bool func_refill_bucket(unsigned int pd)

//refill flow
static bool func_refill_flow(unsigned fd)
{
	unsigned int oa_bucket;
	unsigned int op_bucket;
	unsigned long long aplus;
	unsigned long long pplus;
	
	//printk("a bkt = %u\n",tcpl[fd][tcpl_abucket]);
	//printk("p bkt = %u\n",tcpl[fd][tcpl_pbucket]);
	
	oa_bucket = tcpl[fd][tcpl_abucket];
	op_bucket = tcpl[fd][tcpl_pbucket];
	
	if (tcpl[fd][tcpl_time_sec]<=this_flow_sec)
	{
		tcpl[fd][tcpl_abucket] += ( this_flow_sec-tcpl[fd][tcpl_time_sec] ) * tcpl[fd][tcpl_arate];
		tcpl[fd][tcpl_pbucket] += ( this_flow_sec-tcpl[fd][tcpl_time_sec] ) * tcpl[fd][tcpl_prate];
	}
	else
	{
		tcpl[fd][tcpl_abucket] += this_flow_sec * tcpl[fd][tcpl_arate];
		tcpl[fd][tcpl_pbucket] += this_flow_sec * tcpl[fd][tcpl_prate];
	}
	
	/*for test
	 //printk("org sec = %u, new sec = %u\n", tcpl[fd][tcpl_time_sec],this_flow_sec);
	 //printk("org usec = %u, new usec = %u\n", tcpl[fd][tcpl_time_usec],this_flow_usec);
	 //printk("refill by sec = %u\n", (tcpl[fd][tcpl_abucket]-org_bucket));
	 */// for test end
	
	//printk("org sec = %u, new sec = %u\n", tcpl[fd][tcpl_time_sec],this_flow_sec);
	//printk("org usec = %u, new usec = %u\n", tcpl[fd][tcpl_time_usec],this_flow_usec);
	
	
	if (tcpl[fd][tcpl_time_usec]<=this_flow_usec)
	{
		aplus = (this_flow_usec-tcpl[fd][tcpl_time_usec]);
		aplus *= tcpl[fd][tcpl_arate];
		pplus = (this_flow_usec-tcpl[fd][tcpl_time_usec]);
		pplus *= tcpl[fd][tcpl_prate];
		
		tcpl[fd][tcpl_abucket] += (unsigned int)(aplus/1000000);
		tcpl[fd][tcpl_pbucket] += (unsigned int)(pplus/1000000);
	}
	else
	{
		//jdg:
		//unsigned long long aplus = (tcpl[fd][tcpl_time_usec]-this_flow_usec);
		aplus = (tcpl[fd][tcpl_time_usec]-this_flow_usec);
		
		aplus *= tcpl[fd][tcpl_arate];
		
		//jdg:
		//unsigned long long pplus = (tcpl[fd][tcpl_time_usec]-this_flow_usec);
		pplus = (tcpl[fd][tcpl_time_usec]-this_flow_usec);
		
		pplus *= tcpl[fd][tcpl_prate];
		
		tcpl[fd][tcpl_abucket] -= (unsigned int)(aplus/1000000);
		tcpl[fd][tcpl_pbucket] -= (unsigned int)(pplus/1000000);
	}
	
	//printk("refill a bkt = %u\n",tcpl[fd][tcpl_abucket]-oa_bucket);
	//printk("refill p bkt = %u\n",tcpl[fd][tcpl_pbucket]-op_bucket);
	
	/* for test
	 //printk("org bucket = %u, total refill bucket = %u, bucket now = %u\n", org_bucket, (tcpl[fd][tcpl_abucket]-org_bucket),tcpl[fd][tcpl_abucket]);
	 //printk("packet len = %u\n", this_flow_len);
	 */ //for test end
	
	if (tcpl[fd][tcpl_abucket]>tcpl[fd][tcpl_avg_max])
	{
		tcpl[fd][tcpl_abucket]=tcpl[fd][tcpl_avg_max];
	}
	if (tcpl[fd][tcpl_pbucket]>tcpl[fd][tcpl_peak_max])
	{
		tcpl[fd][tcpl_pbucket]=tcpl[fd][tcpl_peak_max];
	}
	
	//printk("a bkt now %u\n",tcpl[fd][tcpl_abucket]);
	//printk("p bkt now %u\n",tcpl[fd][tcpl_pbucket]);
	
	//printk("packet len = %u\n", this_flow_len);
	
	//printk("bucket after overflow check is %u\n", tcpl[fd][tcpl_abucket]);    for test
	
	tcpl[fd][tcpl_time_sec]=this_flow_sec;
	tcpl[fd][tcpl_time_usec]=this_flow_usec;
	
	return true;
	
} // static bool func_refill_flow(unsigned fd)


//match between two ips
static unsigned int tempx= 0;

//jdg: unused
//static unsigned int func_bit_match (unsigned int num_a, unsigned int num_b)
//{
//	unsigned int count = 0;
//	
//	tempx=(num_a&num_b)|((~num_a)&(~num_b));
//	
//	while (tempx)
//	{
//		tempx=tempx & (tempx-1);
//		count++;
//	}
//	return count;
//}

//determin FIC via tos
//static unsigned int func_get_fic (unsigned char tos)		//jdg
static unsigned int func_get_fic(unsigned char tos)
{
	unsigned char dscp = tos & (0xfc);
	if (dscp == 0xc8) return 1;
	if (dscp == 0xb0) return 2;
	if (dscp == 0xb8) return 3;
	return 0;
}

//check best match of prefic, return corresponding DID
//static unsigned int func_get_did (unsigned int ipaddr)		//jdg
static unsigned int func_get_did(unsigned int ipaddr)
{
	unsigned char *ipd;
	unsigned char *idd;
	unsigned int das_ind;
	unsigned int best_mask;
	unsigned int best_did;
	unsigned int prefix;
	unsigned int mask;
	unsigned int this_did;
	unsigned int ip_go;
	unsigned int ip_masked;
	
	//unsigned int best_match = 0;
	//unsigned int this_match = 0;
	das_ind = 0;
	best_mask = 0;
	best_did = 0;
	
	prefix = 0;
	mask = 0;
	this_did = 0;
	ip_go = ipaddr;
	ip_masked = ip_go;
	
	while ((tc_dasl[das_ind][dasl_prefix]||tc_dasl[das_ind][dasl_did])&&(das_ind<MAX_DAS))
	{
		prefix = tc_dasl[das_ind][dasl_prefix];
		mask = tc_dasl[das_ind][dasl_mask];
		this_did = tc_dasl[das_ind][dasl_did];
		
		/*//for test
		 printk("das_ind is %u \n", das_ind);
		 printk("prefix to be matched is %u \n", prefix);
		 //printk("corresponding did is %u \n", this_did);
		 unsigned char *dp = (unsigned char *)&(prefix);
		 printk("DID IP is %d.%d.%d.%d\n",dp[0]&0xff,dp[1]&0xff, dp[2]&0xff, dp[3]&0xff);
		 printk("ip to be matched is %u \n", ip_go);
		 unsigned char *pp = (unsigned char *)&(ip_go);
		 printk("IP is %d.%d.%d.%d\n",pp[0]&0xff,pp[1]&0xff, pp[2]&0xff, pp[3]&0xff);
		 // for test end*/
		
		//unsigned char *iip = (unsigned char *)&(ip_go);
		//printk("IP is %d.%d.%d.%d\n",iip[0]&0xff,iip[1]&0xff, iip[2]&0xff, iip[3]&0xff);
		
		//new
		if (mask<=32)
		{
			ip_masked = ip_go & ipmask[mask];
		}
		else
		{
			ip_masked = ip_go;
		}
		
		ipd = (unsigned char *)&(ip_masked);
		idd = (unsigned char *)&(prefix);
		
		//printk("IP masked by %u is %d.%d.%d.%d\n", mask, ipd[0]&0xff,ipd[1]&0xff, ipd[2]&0xff, ipd[3]&0xff);
		
		//printk("0 compare %d in prefix with %d in ip\n", (idd[0]&0xff), (ipd[0]&0xff));
		if ((idd[0]&0xff) == (ipd[0]&0xff))
		{
			//printk("1 compare %d in prefix with %d in ip\n", (idd[1]&0xff), (ipd[1]&0xff));
			if ((idd[1]&0xff) == (ipd[1]&0xff))
			{
				//printk("2 compare %d in prefix with %d in ip\n", (idd[2]&0xff), (ipd[2]&0xff));
				if ((idd[2]&0xff) == (ipd[2]&0xff))
				{
					//printk("3 compare %d in prefix with %d in ip\n", (idd[3]&0xff), (ipd[3]&0xff));
					if ((idd[3]&0xff) == (ipd[3]&0xff))
					{
						if (mask>=best_mask)
						{
							best_did = this_did;
							best_mask = mask;
							//printk("match in dasl line %u\n", das_ind);
						}
						
						//break;
					}
				}
			}
		}
		//new
		
		/* //keep
		 
		 this_match=func_bit_match(prefix, ip_masked);
		 
		 //printk("match is %u \n", this_match);// for test
		 
		 if (this_match>best_match)
		 {
		 best_match = this_match;
		 best_mask = mask;
		 best_did = this_did;
		 }
		 else if ((this_match == best_match)&&(mask>best_mask))
		 {
		 best_match = this_match;
		 best_mask = mask;
		 best_did = this_did;
		 }
		 */
		
		das_ind++;
	}
	
	return best_did;
	
} // static unsigned int func_get_did(unsigned int ipaddr)

/*
// ------+++------ TRIGGERED / TIMED UPDATES
*/

//hook function defined here
unsigned int func_hook_func(
		const struct nf_hook_ops *ops,
		struct sk_buff *skb,
		const struct net_device *in,
		const struct net_device *out,
		int (*okfn)(struct sk_buff *)
	)
{
	//jdg: warning: assignment discards 'const' qualifier from pointer target type [enabled by default]
	char * in_name;
	char * out_name;
	//
	//	> grep -A7 'struct net_device {' /usr/src/linux-headers-3.16.0-4-common/include/linux/netdevice.h
	//	struct net_device {
	//		char                    name[IFNAMSIZ];
	//		...
	//	}
	//
	// > egrep '\#define\sIFNAMSIZ' /usr/src/linux-headers-3.16.0-4-common/include/uapi/linux/if.h
	// #define IFNAMSIZ        16
	//
	//jdg: error: incompatible types when assigning to type 'char[16]' from type 'const char *'
	//jdg: so we dont change 'char *' to 'char []':
	//char in_name[IFNAMSIZ];
	//char out_name[IFNAMSIZ];
	//
	//jdg: did add 'const' in qstcp.h: static const char main_interface[IFNAMSIZ] = "br1";
	//char null_str[IFNAMSIZ] = "null";
	
	unsigned int m_n;
	unsigned int i_n;
	unsigned int o_n;

	//if (plc_on == false)
	//{
	//printk("plc false");
	//    return NF_ACCEPT;
	//}
	do_gettimeofday( &(struct_txc.time) );
	this_flow_sec = struct_txc.time.tv_sec;
	this_flow_usec = struct_txc.time.tv_usec;
	
	ip = ip_hdr(skb);//get ip head
	this_flow_len = ntohs(ip->tot_len);
	//printk("a packet len-%u\n", this_flow_len); //for test
	// if small packet bucket not empty, not do anything
	if ( (this_flow_len < SMALL_LEN) & bool_small_tp_effect)
	{
		if (small_bucket > this_flow_len)
		{
			printk("ignore a small packet, len = %u\n", this_flow_len);
			small_bucket-=this_flow_len;
			return NF_ACCEPT;
		}
	}
	
	this_flow_fic = func_get_fic( ip->tos );

	//jdg: warning: ISO C90 forbids mixed declarations and code [-Wdeclaration-after-statement]
	//jdg: warning: assignment discards 'const' qualifier from pointer target type [enabled by default]
	//out_name = (out->name == NULL) ? "null" : out->name;
	//out_name = (out->name == NULL) ? null_str : out->name;
	//in_name  = (in->name  == NULL) ? null_str : in->name;
	out_name = (out->name == NULL) ? "null" : out->name;
	in_name  = (in->name  == NULL) ? "null" : in->name;
	
	//jdg: huh? cast char[] to (unsigned int *) ?? -- I assume to accelerate below comparison
	m_n = *(unsigned int *)main_interface;
	i_n = *(unsigned int *)in_name;
	o_n = *(unsigned int *)out_name;
	
	
	//printk("determin sid did == == == == == == == == == == == == == == == == == =\n");
	
	if ( (m_n != i_n) && ( m_n != o_n ) )
	{
		//printk("interface rec from %s, tran to %s, not managed\n", in_name, out_name);//for test
		return NF_ACCEPT;
	}
	if ( m_n == i_n )
	{
		//printk("interface rec from %s, tran to %s, rx flow\n", in_name, out_name);//for test
		this_flow_sid = ip->saddr;
		this_flow_did = func_get_did( ip->daddr );
		this_flow_dir = DIR_U;
	}
	else
	{
		//printk("interface rec from %s, tran to %s, tx flow\n", in_name, out_name);//for test
		this_flow_sid = ip->daddr;
		this_flow_did = func_get_did(ip->saddr);
		this_flow_dir = DIR_D;
	}
	
	this_flow_ind = func_get_flow_ind();
	
	/* for test
	 unsigned char *p = (unsigned char *)&(ip->saddr);
	 unsigned char *dp = (unsigned char *)&(ip->daddr);
	 printk("source ip is %d.%d.%d.%d\n",p[0]&0xff,p[1]&0xff, p[2]&0xff, p[3]&0xff);
	 printk("destination ip is %d.%d.%d.%d\n",dp[0]&0xff,dp[1]&0xff, dp[2]&0xff, dp[3]&0xff);
	 //printk("did matched is %u \n", this_flow_did);
	 
	 unsigned char *sip = (unsigned char *)&(this_flow_sid);
	 printk("SID is %d.%d.%d.%d\n",sip[0]&0xff,sip[1]&0xff, sip[2]&0xff, sip[3]&0xff);
	 unsigned char *dip = (unsigned char *)&(this_flow_did);
	 printk("DID is %d.%d.%d.%d\n",dip[0]&0xff,dip[1]&0xff, dip[2]&0xff, dip[3]&0xff);
	 
	 
	 //for test end*/
	
	//check flow number within limit
	if ( this_flow_ind >= MAX_FLOW )
	{
		printk("flows number more than managable, drop packet\n");
		return NF_DROP;
	}
	
	func_refill_flow( this_flow_ind );
	
	this_flow_pol = func_get_policy();
	
	func_update_flow( this_flow_pol );
	
	if (this_flow_pol)
	{
		//printk("packet pass\n");
		return NF_ACCEPT;
	}
	else
	{
		//unsigned char *sip = (unsigned char *)&(this_flow_sid);
		//unsigned char *dip = (unsigned char *)&(this_flow_did);
		//printk("DROP packt from %d.%d.%d.%d to %d.%d.%d.%d\n",sip[0]&0xff,sip[1]&0xff, sip[2]&0xff, sip[3]&0xff, dip[0]&0xff,dip[1]&0xff, dip[2]&0xff, dip[3]&0xff);
		return NF_DROP;
	}
	
} // unsigned int func_hook_func()

/// function in timer expires interrupt
void func_short_timer_do(unsigned long l)
{
	unsigned int f_ind;
	
	//printk(KERN_ALERT" == =jiffies at time.short beginning is: %ld == =\n", jiffies);
	
	f_ind = 0;
	spd = config_1.short_pd;
	prd = config_1.period;
	
	while (f_ind<MAX_FLOW)
	{
		if (tcpl[f_ind][tcpl_on])
		{
			tcpl[f_ind][tcpl_fcount1] = tcpl[f_ind][tcpl_fcount];
			tcpl[f_ind][tcpl_lcount1] = tcpl[f_ind][tcpl_lcount];
			
			tcpl[f_ind][tcpl_fburst] = (tcpl[f_ind][tcpl_fcount1]>tcpl[f_ind][tcpl_fcount2]) ?
				((tcpl[f_ind][tcpl_fcount1]-tcpl[f_ind][tcpl_fcount2]) * 1000/spd) : 0;

			tcpl[f_ind][tcpl_lburst]=(tcpl[f_ind][tcpl_lcount1]>tcpl[f_ind][tcpl_lcount2]) ?
				((tcpl[f_ind][tcpl_lcount1]-tcpl[f_ind][tcpl_lcount2]) * 1000/spd) : 0;
		}
		f_ind++;
	}
	f_ind= 0;
	while (f_ind<MAX_FLOW)
	{
		if (tcpl[f_ind][tcpl_on])
		{
			tcpl[f_ind][tcpl_fcount2] = tcpl[f_ind][tcpl_fcount1];
			tcpl[f_ind][tcpl_lcount2] = tcpl[f_ind][tcpl_lcount1];
			
			tcpl[f_ind][tcpl_lfmax] = (tcpl[f_ind][tcpl_fburst]>=tcpl[f_ind][tcpl_lfmax]) ?
				tcpl[f_ind][tcpl_fburst] : tcpl[f_ind][tcpl_lfmax];
			
			tcpl[f_ind][tcpl_llmax] = (tcpl[f_ind][tcpl_lburst]>=tcpl[f_ind][tcpl_llmax]) ?
				tcpl[f_ind][tcpl_lburst] : tcpl[f_ind][tcpl_llmax];
			
			tcpl[f_ind][tcpl_fsum] += tcpl[f_ind][tcpl_fburst];
			tcpl[f_ind][tcpl_lsum] += tcpl[f_ind][tcpl_lburst];
		}
		f_ind++;
	}
	
	if time_after((jiffies + HZ * spd/1000), jiffies)
	{
		mod_timer(&short_timer, jiffies + HZ * spd/1000);//reset timer
	}
	else
	{
		mod_timer(&short_timer, HZ * spd/1000);//reset timer
	}
	
} // void func_short_timer_do(unsigned long l)

void func_long_timer_do(unsigned long l)
{
	unsigned int f_ind;
	unsigned long long temp_avg;
	
	//printk(KERN_ALERT" == =jiffies at time.long beginning is: %ld == =\n", jiffies);
	func_get_time_stamp(); //get packet in long time
	func_refill_bucket(config_1.long_pd);
	f_ind = 0;
	lpd = config_1.long_pd;
	spd = config_1.short_pd;
	temp_avg = 0;
	
	while (f_ind < MAX_FLOW)
	{
		if (tcpl[f_ind][tcpl_on])
		{
			tcpo[f_ind][tcpo_fmax]		= tcpl[f_ind][tcpl_lfmax];
			tcpo[f_ind][tcpo_lmax]		= tcpl[f_ind][tcpl_llmax];
			tcpl[f_ind][tcpl_lfmax]		= 0;
			tcpl[f_ind][tcpl_llmax]		= 0;
			
			temp_avg = tcpl[f_ind][tcpl_fsum] * spd;
			tcpo[f_ind][tcpo_favg]		= (unsigned int)(temp_avg/lpd);
			
			temp_avg = tcpl[f_ind][tcpl_lsum] * spd;
			tcpo[f_ind][tcpo_lavg]		= (unsigned int)(temp_avg/lpd);
			
			tcpl[f_ind][tcpl_fsum]		= 0;
			tcpl[f_ind][tcpl_lsum]		= 0;
			
			tcpl[f_ind][tcpl_fcount]	= 0;
			tcpl[f_ind][tcpl_fcount1]	= 0;
			tcpl[f_ind][tcpl_fcount2]	= 0;
			tcpl[f_ind][tcpl_lcount]	= 0;
			tcpl[f_ind][tcpl_lcount1]	= 0;
			tcpl[f_ind][tcpl_lcount2]	= 0;
			
		}
		f_ind++;
	}
	if time_after((jiffies + HZ * lpd/1000), jiffies)
	{
		mod_timer(&long_timer, jiffies + HZ * lpd/1000);//reset timer
	}
	else
	{
		mod_timer(&long_timer, HZ * lpd/1000);//reset timer
	}
	
} // void func_long_timer_do(unsigned long l)

void func_alive_timer_do(unsigned long l)
{
	unsigned int f_ind;
	unsigned int max_cap;
	
	printk(KERN_ALERT" == =jiffies at time.alive beginning is: %ld == =\n", jiffies);
	f_ind = 0;
	max_cap = 0;
	alv = config_1.alive;
	
	while (f_ind<MAX_FLOW)
	{
		max_cap = (tcpo[f_ind][tcpo_dir] == DIR_U) ? icap : ecap;
		if ((tcpo[f_ind][tcpo_favg]<(max_cap/1000))&&(tcpl[f_ind][tcpl_on]))
		{
			tcpo[f_ind][tcpo_sid]			= 0;
			tcpo[f_ind][tcpo_did]			= 0;
			tcpo[f_ind][tcpo_dir]			= 0;
			tcpo[f_ind][tcpo_fic]			= 0;
			tcpo[f_ind][tcpo_fmax]			= 0;
			tcpo[f_ind][tcpo_favg]			= 0;
			tcpo[f_ind][tcpo_lmax]			= 0;
			tcpo[f_ind][tcpo_lavg]			= 0;
			
			tcpl[f_ind][tcpl_sid]			= 0;
			tcpl[f_ind][tcpl_did]			= 0;
			tcpl[f_ind][tcpl_dir]			= 0;
			tcpl[f_ind][tcpl_fic]			= 0;
			tcpl[f_ind][tcpl_fcount]		= 0;
			tcpl[f_ind][tcpl_fcount1]		= 0;
			tcpl[f_ind][tcpl_fcount2]		= 0;
			tcpl[f_ind][tcpl_fburst]		= 0;
			tcpl[f_ind][tcpl_lcount]		= 0;
			tcpl[f_ind][tcpl_lcount1]		= 0;
			tcpl[f_ind][tcpl_lcount2]		= 0;
			tcpl[f_ind][tcpl_lburst]		= 0;
			tcpl[f_ind][tcpl_time_sec]		= 0;
			tcpl[f_ind][tcpl_time_usec]	= 0;
			tcpl[f_ind][tcpl_arate]			= 0;
			tcpl[f_ind][tcpl_prate]			= 0;
			tcpl[f_ind][tcpl_avg_max]		= 0;
			tcpl[f_ind][tcpl_peak_max]		= 0;
			tcpl[f_ind][tcpl_abucket]		= 0;
			tcpl[f_ind][tcpl_pbucket]		= 0;
			tcpl[f_ind][tcpl_on]				= 0;
		}
		f_ind++;
	}
	if time_after((jiffies + HZ*alv), jiffies)
	{
		mod_timer(&alive_timer, jiffies + HZ*alv);//reset timer
	}
	else
	{
		mod_timer(&alive_timer, HZ*alv);//reset timer
	}
	
} // void func_alive_timer_do(unsigned long l)

//timer set-up
void func_short_timer_set(void)
{
	init_timer(&short_timer);//initialization
	
	short_timer.expires = jiffies + HZ*config_1.short_pd/1000;
	short_timer.function = func_short_timer_do;    //interrupt function
	add_timer(&short_timer);    //register timer to kernel
}

void func_long_timer_set(void)
{
	init_timer(&long_timer);//initialization
	
	long_timer.expires = jiffies + HZ*config_1.long_pd/1000;
	long_timer.function = func_long_timer_do;    //interrupt function
	add_timer(&long_timer);    //register timer to kernel
}

void func_alive_timer_set(void)
{
	init_timer(&alive_timer);//initialization
	
	alive_timer.expires = jiffies + HZ*config_1.alive;
	alive_timer.function = func_alive_timer_do;    //interrupt function
	add_timer(&alive_timer);    //register timer to kernel
}

/*
// ------+++------ PROC
*/

// proc tp on fuctions
static ssize_t func_tpon_proc_write(
		struct file *filp, const char __user * buff,
		size_t len, loff_t * offset
	)
{
	memset(tp_on_str,0,TP_ON_LEN);
	len = len < TP_ON_LEN ? len : TP_ON_LEN;
	if (copy_from_user(tp_on_str, buff, len))
	{
		printk(KERN_INFO "tpon: copy_from_user() error!\n");
		return -EFAULT;
	}
	if (len>3)
	{
		bool_small_tp_effect = true;
	}
	else
	{
		bool_small_tp_effect = false;
	}
	return len;
}

static int func_tpon_proc_show(struct seq_file *m, void *v)
{
	if (bool_small_tp_effect)
	{
		seq_printf(m, "small packet bypass is turned ON\n");
	}
	else
	{
		seq_printf(m, "small packet bypass is turned OFF\n");
	}
	return 0;
}

static int func_tpon_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, func_tpon_proc_show, NULL);
}

static const struct file_operations tpon_proc_fops = {
	.owner      = THIS_MODULE,
	.write      = func_tpon_proc_write,
	.open       = func_tpon_proc_open,
	.read       = seq_read,
	.llseek     = seq_lseek,
	.release    = single_release,
};

// proc time_stamp fuctions
static int func_tstamp_proc_show(struct seq_file *m, void *v)
{
	seq_printf(m, time_stamp);
	return 0;
}

static int func_tstamp_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, func_tstamp_proc_show, NULL);
}

static const struct file_operations tstamp_proc_fops = {
	.owner      = THIS_MODULE,
	.open       = func_tstamp_proc_open,
	.read       = seq_read,
	.llseek     = seq_lseek,
	.release    = single_release,
};

// proc links.count fuctions
static int func_func_lcount_proc_show(struct seq_file *m, void *v)
{
	seq_printf(m, "links.count not for read\n");
	return 0;
}

static int func_lcount_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, func_func_lcount_proc_show, NULL);
}

static const struct file_operations lcount_proc_fops = {
	.owner      = THIS_MODULE,
	.open       = func_lcount_proc_open,
	.read       = seq_read,
	.llseek     = seq_lseek,
	.release    = single_release,
};

// proc link.table fuctions
static ssize_t func_ltable_proc_write(
		struct file *filp, const char __user * buff,
		size_t len, loff_t * offset
	)
{
	char num_str[NUM_LEN];
	char link_str[64];
	char link_name[IFNAMSIZ];
	size_t vlen;
	char * temp_links;
	unsigned int m_n;
	unsigned link_i;
	char * one_link;
	char * icap_str;
	char * name_str;
	char * ecap_str;
	char * after;
	unsigned int tfd;
	unsigned int l_n;
	
	memset(links_table_str, 0, LINKS_TABLE_LEN);
	len = len < LINKS_TABLE_LEN ? len : LINKS_TABLE_LEN;
	
	vlen = 0;
	
	if (copy_from_user(links_table_str, buff, len))
	{
		printk(KERN_INFO "config: copy_from_user() error!\n");
		return -EFAULT;
	}
	
	//links_table_str="{\"links\":[{\"name\":\"eth0\",\"icap\":94000,\"ecap\":94000},{\"name\":\"wlan0\",\"icap\":95000,\"ecap\":95000}]}";
	
	temp_links = links_table_str;
	links_table_str = func_json("links", 0, temp_links, strlen(temp_links), &vlen);
	if (vlen == 0)
		links_table_str = temp_links;
	
	printk("links table str now is %s\n",links_table_str);
	
	m_n = *(unsigned int *)main_interface;
	link_i = 0;
	one_link = func_json(0, 0, links_table_str, strlen(links_table_str), &vlen);
	
	while (one_link != NULL)
	{
		
		snprintf(link_str, 64 , "%.*s", (int)vlen, one_link);
		printk("%s\n",link_str);
		name_str = func_json("name", 0, one_link, strlen(one_link), &vlen);
		snprintf(link_name, IFNAMSIZ , "%.*s", (int)vlen, name_str);
		printk("%s\n",link_name);
		
		snprintf(main_interface, IFNAMSIZ , "%.*s", (int)vlen, name_str);
  
		l_n = *(unsigned int *)link_name;
		if (l_n == m_n)
		{
			printk("find main interface\n");
			
			//jdg: warning: assignment discards 'const' qualifier from pointer target type [enabled by default]
			//icap_str = func_json("icap", 0, one_link, strlen(one_link), &vlen);
			
			icap_str = func_json("icap", 0, one_link, strlen(one_link), &vlen);
			if (icap_str != NULL)
			{
				snprintf(num_str, NUM_LEN , "%.*s", (int)vlen, icap_str);
				icap = simple_strtoul( num_str, &after, 10) * 1000;
				printk("icap = %u\n", icap);
			}
			ecap_str = func_json("ecap", 0, one_link, strlen(one_link), &vlen);
			if (ecap_str != NULL)
			{
				snprintf(num_str, NUM_LEN , "%.*s", (int)vlen, ecap_str);
				ecap = simple_strtoul( num_str, &after, 10) * 1000;
				printk("ecap = %u\n", ecap);
			}
			
		}
		
		link_i++;
		printk("next link\n");
		one_link = func_json(0, link_i, links_table_str, strlen(links_table_str), &vlen);
		printk("vlen is %d", (int)vlen);
		
	}
	tfd = 0;
	while (tfd < MAX_FLOW)
	{
		tcpl[tfd][tcpl_on] = 0;
		tfd++;
	}
	return len;
	
} // static ssize_t func_ltable_proc_write()

static int func_ltable_proc_show(struct seq_file *m, void *v)
{
	seq_printf(m, "link %s: icap = %u, ecap = %u \n", main_interface, icap, ecap);
	return 0;
}

static int func_ltable_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, func_ltable_proc_show, NULL);
}

static const struct file_operations ltable_proc_fops = {
	.owner      = THIS_MODULE,
	.open       = func_ltable_proc_open,
	.write      = func_ltable_proc_write,
	.read       = seq_read,
	.llseek     = seq_lseek,
	.release    = single_release,
};

// proc das.count fuctions
static int func_dcount_proc_show(struct seq_file *m, void *v)
{
	unsigned int das_count;
	unsigned int total_das;
	
	das_count = 0;
	total_das = 0;
	while (das_count<MAX_DAS)
	{
		if (tc_dasl[das_count][dasl_prefix]||tc_dasl[das_count][dasl_did])
		{
			total_das++;
		}
		das_count++;
	}
	seq_printf(m, "-%u-\n",total_das);
	return 0;
}

static int func_dcount_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, func_dcount_proc_show, NULL);
}

static const struct file_operations dcount_proc_fops = {
	.owner      = THIS_MODULE,
	.open       = func_dcount_proc_open,
	.read       = seq_read,
	.llseek     = seq_lseek,
	.release    = single_release,
};

// proc das.table fuctions
static int func_dtable_proc_show(struct seq_file *m, void *v)
{
	int flow_count;
	unsigned char *sp;
	unsigned char *dp;
	
	flow_count = 0;
	while (flow_count < MAX_DAS)
	{
		if (tc_dasl[flow_count][dasl_prefix]||tc_dasl[flow_count][dasl_did])
		{
			sp = (unsigned char *)&(tc_dasl[flow_count][dasl_prefix]);
			dp = (unsigned char *)&(tc_dasl[flow_count][dasl_did]);
			
			seq_printf(m,"%d.%d.%d.%d-%d.%d.%d.%d-%u\n",
						  sp[0]&0xff,sp[1]&0xff,sp[2]&0xff,sp[3]&0xff,
						  dp[0]&0xff,dp[1]&0xff,dp[2]&0xff,dp[3]&0xff,
						  tc_dasl[flow_count][dasl_mask]);
		}
		flow_count++;
	}
	return 0;
}

static int func_dtable_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, func_dtable_proc_show, NULL);
}

static ssize_t func_dtable_proc_write(
		struct file *filp, const char __user * buff,
		size_t len, loff_t * offset
	)
{
	char * temp_das;
	unsigned int max_das_str;
	char das_str[max_das_str];
	unsigned int temp_dasl[MAX_DAS][3];
	unsigned int temp_dasl_i;
	char did_str[9];
	char prx_str[9];
	char mask_str[3];
	char *after;
	unsigned int das_i;
	char * one_das;
	unsigned int das_str_pos;
	unsigned int das_len;
	unsigned int did_i;
	bool did_find;
	bool did_finish;
	unsigned int did_n;
	char * dids;
	bool find_prx;
	bool find_mask;
	unsigned int prx_i;
	unsigned int mask_i;
	unsigned int mask;
	unsigned int prx;
	char b1;
	char b2;
	char b3;
	unsigned int efunc_dasl;
	unsigned int tfd;
	size_t vlen;

	memset(in_das,0,DASL_TABLE_LEN);
	len = len < DASL_TABLE_LEN ? len : LINKS_TABLE_LEN;
	
	if (copy_from_user(in_das, buff, len))
	{
		printk(KERN_INFO "dasl: copy_from_user() error!\n");
		return -EFAULT;
	}
	
	//in_das = "{\"das\":[{\"5698FFFF\":[\"02000000/08\",\"4D040000/16\"]},{\"DFFFFFFF\":[\"00000000/00\"]}]}";
	
	temp_das = in_das;
	max_das_str = 768;
	
	vlen = 0;
	temp_dasl_i = 0;
	
	//did_str[8]='-';
	
	in_das= func_json("das", 0, temp_das, strlen(temp_das), &vlen);
	if (vlen == 0) in_das = temp_das;
	//printk("in_das now is %s\n",in_das);
	
	das_i = 0;
	one_das = func_json(0, das_i, in_das, strlen(in_das), &vlen);
	
	while (one_das != NULL)
	{
		snprintf(das_str, max_das_str , "%.*s", (int)vlen, one_das);
		//printk("das_str No. %u is %s\n", das_i, das_str);
		
		das_str_pos = 0;
		das_len = vlen;
		did_i = 0;
		did_find = false;
		did_finish = false;
		
		while ((das_str_pos<das_len)&&(did_finish == false))
		{
			if (das_str[das_str_pos] == '"')
			{
				if (did_find == false)
				{
					did_find= true;
				}
				else
				{
					did_finish = true;
				}
			}
			else
			{
				if ((did_find == true)&&(did_i<8))
				{
					did_str[did_i]=das_str[das_str_pos];
					did_i++;
				}
			}
			das_str_pos++;
		}
		//printk("did str is %s\n", did_str);
		
		did_n = simple_strtoul( did_str, &after, 16);
		//printk("did str turn to num is %x\n", did_n);
		
		did_n=ntohl(did_n);
		//printk("did char order corrected is %x\n", did_n);
		temp_dasl[temp_dasl_i][dasl_did] = did_n;
		
		dids = (unsigned char *)&(did_n);
		//printk("did in dot form is %d.%d.%d.%d\n",dids[0]&0xff,dids[1]&0xff,dids[2]&0xff,dids[3]&0xff);
		
		find_prx = false;
		find_mask = false;
		
		prx_i = 0;
		mask_i = 0;
		mask = 0;
		prx = 0;
		
		while (das_str_pos<das_len)
		{
			b1 = (find_prx)?'Y':'N';
			b2 = (find_mask)?'Y':'N';
			//printk("%u - %d - %c - %c -%c\n",das_str_pos, das_len, das_str[das_str_pos],b1,b2);
			if (das_str[das_str_pos] == '"')
			{
				if (find_prx == false)
				{
					find_prx = true;
					prx_i= 0;
					//printk("turn find prx true\n");
					temp_dasl[temp_dasl_i][dasl_did] = did_n;
				}
				else
				{
					find_prx = false;
					find_mask = false;
					mask = simple_strtoul( mask_str, &after, 10);
					temp_dasl[temp_dasl_i][dasl_mask] = mask;
					temp_dasl_i++;
					//printk("mask str is %s\n", mask_str);
					//printk("mask is %u\n", mask);
					mask_str[0]='-';
					mask_str[1]='-';
				}
			}
			else if (das_str[das_str_pos] == '/')
			{
				mask_i= 0;
				find_mask = true;
				prx = simple_strtoul( prx_str, &after, 16);
				temp_dasl[temp_dasl_i][dasl_prefix] = ntohl(prx);
				//printk("prx str is %s\n", prx_str);
				//printk("prx is %x\n", temp_dasl[temp_dasl_i][dasl_prefix]);
				
				b3 = (find_mask)?'Y':'N';
				//printk("find mask-%c\n",b3);
				
			}
			else if ( (find_prx == true) && (find_mask == false) )
			{
				if (prx_i<8)
				{
					prx_str[prx_i]=das_str[das_str_pos];
					prx_i++;
				}
			}
			else if ( (find_prx == true) && (find_mask == true) )
			{
				if (mask_i<2)
				{
					//printk("mask_i %u, das_str_char %c",mask_i,das_str[das_str_pos]);
					mask_str[mask_i]=das_str[das_str_pos];
					mask_i++;
				}
			}
			das_str_pos++;
		}
		
		das_i++;
		one_das = func_json(0, das_i, in_das, strlen(in_das), &vlen);
	}
	
	efunc_dasl = temp_dasl_i;
	
	temp_dasl_i = 0;
	while (temp_dasl_i<MAX_DAS)
	{
		if (temp_dasl_i<efunc_dasl)
		{
			tc_dasl[temp_dasl_i][dasl_prefix]	= temp_dasl[temp_dasl_i][dasl_prefix];
			tc_dasl[temp_dasl_i][dasl_mask]		= temp_dasl[temp_dasl_i][dasl_mask];
			tc_dasl[temp_dasl_i][dasl_did]		= temp_dasl[temp_dasl_i][dasl_did];
		}
		else
		{
			tc_dasl[temp_dasl_i][dasl_prefix]	= 0;
			tc_dasl[temp_dasl_i][dasl_mask]		= 0;
			tc_dasl[temp_dasl_i][dasl_did]		= 0;
		}
		
		temp_dasl_i++;
	}
	
	tfd = 0;
	while (tfd<MAX_FLOW)
	{
		tcpl[tfd][tcpl_on]= 0;
		tfd++;
	}
	
	printk("das finish here\n");
	printk("%s\n",__func__);
	
	return len;
	
} // static ssize_t func_dtable_proc_write()

static const struct file_operations dtable_proc_fops = {
	.owner      = THIS_MODULE,
	.open       = func_dtable_proc_open,
	.read       = seq_read,
	.write      = func_dtable_proc_write,
	.llseek     = seq_lseek,
	.release    = single_release,
};

// proc policer.count fuctions
static int func_pcount_proc_show(struct seq_file *m, void *v)
{
	unsigned int flow_count;
	unsigned char *sp;
	unsigned char *dp;
	
	seq_printf(m," == == == == == == show flow policer table == == == == == == \n sid - did - dir - avg_rate - avg_max - peak_rate - peak_max - reg\n");
	//for test
	flow_count = 0;
	while (flow_count<MAX_FLOW)
	{
		if (tcpl[flow_count][tcpl_on])
		{
			sp = (unsigned char *) & ( tcpl[flow_count][tcpl_sid] );
			dp = (unsigned char *) & ( tcpl[flow_count][tcpl_did] );
			
			seq_printf(m, "%d.%d.%d.%d-%d.%d.%d.%d-%u-%u-%u-%u-%u-%u\n",
						  sp[0]&0xff,sp[1]&0xff,sp[2]&0xff,sp[3]&0xff,
						  dp[0]&0xff,dp[1]&0xff,dp[2]&0xff,dp[3]&0xff,
						  tcpl[flow_count][tcpl_dir],
						  tcpl[flow_count][tcpl_arate],
						  tcpl[flow_count][tcpl_avg_max],
						  tcpl[flow_count][tcpl_prate],
						  tcpl[flow_count][tcpl_peak_max],
						  tcpl[flow_count][tcpl_reg]
						  );
		}
		flow_count++;
	}
	return 0;
}

static int func_pcount_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, func_pcount_proc_show, NULL);
}

static const struct file_operations pcount_proc_fops = {
	.owner      = THIS_MODULE,
	.open       = func_pcount_proc_open,
	.read       = seq_read,
	.llseek     = seq_lseek,
	.release    = single_release,
};

//jdg:	suspect somewhere here is the "mangled ip-address" bug
//			in: func_ptable_proc_write() --or maybe in: func_iptonui()
//

//jdg: kernel hang bug is also in func_ptable_proc_write()

// proc policer.table fuctions
static ssize_t func_ptable_proc_write(
		struct file *filp, const char __user * buff,
		size_t len, loff_t * offset
	)
{
	size_t vlen;
	char *after;
	char plc_str[256];
	char ip_str[16];
	char dir_str[4];
	char avg_str[8];
	char abkt_str[8];
	char peak_str[8];
	char pbkt_str[8];
	char * temp_plc;
	bool plc_remove;
	char * one_plc;
	unsigned int plc_i;
	char * one_sid;
	char * one_dir;
	char * one_did;
	char * one_avg;
	char * one_abkt;
	char * one_peak;
	char * one_pbkt;
	char * sipt;
	char * dipt;
	unsigned int remove_id;
	unsigned int re_flow;
	unsigned int fd_plc;
	bool find_plc;
	unsigned int f_id;
	//char temp_dir;	//jdg unused
	
	plc_on = true;
	//if (plc_on == true)
	//{
	//    printk("plc_on true");
	//}
	//else
	//{
	//    printk("plc_on false");
	//}
	
	//printk("start policer parser: len = %u\n", len );	//jdg
	printk("start policer parser: len = %u\n", (unsigned int)len );

	//jdg: above line is last printed just before kernel-hang -> so is following memset(() the problem ?

	printk("jdg: just before memset(in_plc, 0, POLICER_LEN); with in_plc=%p\n", in_plc);
	
	//jdg: declared in qstcp.h: "static char * in_plc;"
	memset(in_plc, 0, POLICER_LEN);

	printk("jdg: just after memset(in_plc, 0, POLICER_LEN);");

	//printk("mem set\n");
	len = len < POLICER_LEN ? len : POLICER_LEN;
	
	//printk("len set = %u\n",len);
	if (copy_from_user(in_plc, buff, len))
	{
		printk(KERN_INFO "policer: copy_from_user() error!\n");
		return -EFAULT;
	}
	//printk("copy from user\n");
	
	vlen = 0;
	plc_remove = false;
	
	//unsigned int tp_id = 0;
	
	temp_plc = func_json("policer", 0, in_plc, strlen(in_plc), &vlen);
	
	if (vlen == 0)
	{
		printk("fail to find key \"policer\"\n");		//jdg: was "plocier"
		return 0;
	}
	
	//printk("policer str is now = %s\n",in_plc);	//jdg: was "plocier"
	
	one_plc = func_json(0, 0, temp_plc, strlen(temp_plc), &vlen);
	plc_i = 0;
	
	while (one_plc != NULL)
	{
		snprintf(plc_str, 256 , "%.*s", (int)vlen, one_plc);
		//printk("plc str No.%u is %s\n", plc_i, plc_str);
		
		one_sid = func_json("SID", 0, plc_str, strlen(plc_str), &vlen);
		if (one_sid != NULL)
		{
			snprintf(ip_str, 16 , "%.*s", (int)vlen, one_sid);
			//printk("sid str is %s\n", ip_str);
			plc_rec[plc_sid] = func_iptonui(ip_str);
		}
		
		one_did = func_json("DID", 0, plc_str, strlen(plc_str), &vlen);
		if (one_did != NULL)
		{
			snprintf(ip_str, 16 , "%.*s", (int)vlen, one_did);
			printk("did str is %s\n", ip_str);
			plc_rec[plc_did] = func_iptonui(ip_str);
		}
		
		one_dir = func_json("DIR", 0, plc_str, strlen(plc_str), &vlen);
		if (one_dir != NULL)
		{
			snprintf(dir_str, 4 , "%.*s", (int)vlen, one_dir);
			//temp_dir = dir_str[0];	//jdg unused
			plc_rec[plc_dir] = (dir_str[0] == 'U')?DIR_U:DIR_D;
			//printk("dir str is %c number form is %x\n", dir_str[0],plc_rec[plc_dir]);
		}
		
		one_avg = func_json("AVG", 0, plc_str, strlen(plc_str), &vlen);
		if (one_avg != NULL)
		{
			snprintf(avg_str, 8 , "%.*s", (int)vlen, one_avg);
			plc_rec[plc_avg] = simple_strtoul( avg_str, &after, 10);
			//printk("avg is %u\n", plc_rec[plc_avg]);
		}
		
		one_abkt = func_json("ABKT", 0, plc_str, strlen(plc_str), &vlen);
		if (one_abkt != NULL)
		{
			snprintf(abkt_str, 8 , "%.*s", (int)vlen, one_abkt);
			plc_rec[plc_abkt] = simple_strtoul( abkt_str, &after, 10);
			//printk("abkt is %u\n", plc_rec[plc_abkt]);
		}
		else
		{
			plc_rec[plc_abkt] = 0;
		}
		
		one_peak = func_json("PEAK", 0, plc_str, strlen(plc_str), &vlen);
		if (one_peak != NULL)
		{
			snprintf(peak_str, 8 , "%.*s", (int)vlen, one_peak);
			plc_rec[plc_peak] = simple_strtoul( peak_str, &after, 10);
			//printk("peak is %u\n", plc_rec[plc_peak]);
		}
		else
		{
			plc_rec[plc_peak] = 0;
		}
		
		one_pbkt = func_json("PBKT", 0, plc_str, strlen(plc_str), &vlen);
		if (one_pbkt != NULL)
		{
			snprintf(pbkt_str, 8 , "%.*s", (int)vlen, one_pbkt);
			plc_rec[plc_pbkt] = simple_strtoul( pbkt_str, &after, 10);
			//printk("pbkt is %u\n", plc_rec[plc_pbkt]);
		}
		else
		{
			plc_rec[plc_pbkt] = 0;
		}
		
		//sipt = (unsigned char *)&(plc_rec[plc_sid]);
		//dipt = (unsigned char *)&(plc_rec[plc_did]);
		sipt = (unsigned char *) &( plc_rec[plc_sid] );
		dipt = (unsigned char *) &( plc_rec[plc_did] );
		
		printk("policer sid-did-dir-avg-abkt-peak-pbkt\n %d.%d.%d.%d-%d.%d.%d.%d-%x-%u-%u-%u-%u\n",
				 sipt[0]&0xff,			sipt[1]&0xff,		sipt[2]&0xff,			sipt[3]&0xff,
				 dipt[0]&0xff,			dipt[1]&0xff,		dipt[2]&0xff,			dipt[3]&0xff,
				 plc_rec[plc_dir],	plc_rec[plc_avg],	plc_rec[plc_abkt],	plc_rec[plc_peak],	plc_rec[plc_pbkt]
		);
		
		// update default policer value for unregistered flow
		if ((plc_rec[plc_sid]==0) && (plc_rec[plc_avg>0]) && (plc_rec[plc_abkt]>0) && (plc_rec[plc_peak]>0) && (plc_rec[plc_pbkt]>0))
		{
			AVG_RT_U = plc_rec[plc_avg];		// default value for unregistered
			AVG_BK_U = plc_rec[plc_abkt];
			PEK_RT_U = plc_rec[plc_peak];
			PEK_BK_U = plc_rec[plc_pbkt];
		}
		
		if (plc_rec[plc_avg] == 0) // remove policer and flow bucket limit
		{
			plc_remove = true;
			remove_id = 0;
			printk("remove above policer\n");
			while (remove_id<MAX_DAS)
			{
				if (tp_plc[remove_id][plc_sid] == plc_rec[plc_sid])
				{
					if (tp_plc[remove_id][plc_did] == plc_rec[plc_did])
					{
						tp_plc[remove_id][plc_sid] = 0;
						tp_plc[remove_id][plc_did] = 0;
					}
				}
				
				remove_id++;
			}
			re_flow = 0;
			while (re_flow < MAX_FLOW)
			{
				if ((tcpl[re_flow][tcpl_sid] == plc_rec[plc_sid])||((tcpl[re_flow][tcpl_reg] == 0)&&(plc_rec[plc_sid] == 0)))
				{
					if (tcpl[re_flow][tcpl_did] == plc_rec[plc_did])
					{
						//printk("remove policer effeck on flow no. %d\n", re_flow);
						tcpl[re_flow][tcpl_arate]		= AVG_RT_U;
						tcpl[re_flow][tcpl_prate]		= PEK_RT_U;
						tcpl[re_flow][tcpl_avg_max]	= AVG_BK_U;
						tcpl[re_flow][tcpl_peak_max]	= PEK_BK_U;
						tcpl[re_flow][tcpl_abucket]	= AVG_BK_U;
						tcpl[re_flow][tcpl_pbucket]	= PEK_BK_U;
						tcpl[re_flow][tcpl_reg]			= 0;
					}
				}
				re_flow++;
			}
		}
		else
		{
			fd_plc = 0;
			find_plc = false;
			
			while (fd_plc < MAX_DAS)
			{
				if (tp_plc[fd_plc][plc_sid] == plc_rec[plc_sid])
				{
					if (tp_plc[fd_plc][plc_did] == plc_rec[plc_did])
					{
						if (tp_plc[fd_plc][plc_dir] == plc_rec[plc_dir])
						{
							find_plc = true;
							break;
						}
					}
				}
				fd_plc++;
			}
			if (find_plc == true)
			{
				printk("replace policer no. %u\n", fd_plc);
			}
			else
			{
				fd_plc = 0;
				while (fd_plc < MAX_DAS)
				{
					if (tp_plc[fd_plc][plc_sid] == 0)
					{
						if (tp_plc[fd_plc][plc_did] == 0)
						{
							break;
						}
					}
					fd_plc++;
				}
				printk("new policer no. %u\n", fd_plc);
			}
			
			tp_plc[fd_plc][plc_sid]		= plc_rec[plc_sid];
			tp_plc[fd_plc][plc_did]		= plc_rec[plc_did];
			tp_plc[fd_plc][plc_dir]		= plc_rec[plc_dir];
			tp_plc[fd_plc][plc_avg]		= plc_rec[plc_avg];
			tp_plc[fd_plc][plc_abkt]	= plc_rec[plc_abkt];
			tp_plc[fd_plc][plc_peak]	= plc_rec[plc_peak];
			tp_plc[fd_plc][plc_pbkt]	= plc_rec[plc_pbkt];
			//tp_id++;
		}
		//if (tp_id>=MAX_DAS) tp_id=MAX_DAS-1;
		
		f_id = 0;
		
		while (f_id < MAX_FLOW)
		{
			if (plc_rec[plc_avg] == 0) break;
			
			//char * psp = (unsigned char *)&(plc_rec[plc_sid]);
			//char * pdp = (unsigned char *)&(plc_rec[plc_did]);
			
			//char * fsp = (unsigned char *)&(tcpl[f_id][tcpl_sid]);
			//char * fdp = (unsigned char *)&(tcpl[f_id][tcpl_did]);
			
			//printk("policer sid-did-dir %d.%d.%d.%d-%d.%d.%d.%d-%x\n",psp[0]&0xff,psp[1]&0xff,psp[2]&0xff,psp[3]&0xff,pdp[0]&0xff,pdp[1]&0xff,pdp[2]&0xff,pdp[3]&0xff,plc_rec[plc_dir]);
			//printk("flow %u sid-did-dir %d.%d.%d.%d-%d.%d.%d.%d-%x\n",f_id,fsp[0]&0xff,fsp[1]&0xff,fsp[2]&0xff,fsp[3]&0xff,fdp[0]&0xff,fdp[1]&0xff,fdp[2]&0xff,fdp[3]&0xff,tcpl[f_id][tcpl_dir]);
			
			if (tcpl[f_id][tcpl_sid] == plc_rec[plc_sid])
			{
				if (plc_rec[plc_did] == tcpl[f_id][tcpl_did])
				{
					if (plc_rec[plc_dir] == tcpl[f_id][tcpl_dir])
					{
						printk("policer effeck on flow no. %d\n", f_id);
						tcpl[f_id][tcpl_arate]		= plc_rec[plc_avg]	* 1000;
						tcpl[f_id][tcpl_prate]		= plc_rec[plc_peak]	* 1000;
						tcpl[f_id][tcpl_avg_max]	= plc_rec[plc_abkt]	* 1000;
						tcpl[f_id][tcpl_peak_max]	= plc_rec[plc_pbkt]	* 1000;
						tcpl[f_id][tcpl_abucket]	= plc_rec[plc_abkt]	* 1000;
						tcpl[f_id][tcpl_pbucket]	= plc_rec[plc_pbkt]	* 1000;
						tcpl[f_id][tcpl_reg]			= 1;
					}
				}
			}
			if ((plc_rec[plc_sid] == 0) && (plc_remove == false))
			{
				if (plc_rec[plc_did] == tcpl[f_id][tcpl_did])
				{
					if (plc_rec[plc_dir] == tcpl[f_id][tcpl_dir])
					{
						if (tcpl[f_id][tcpl_reg] == 0)
						{
							//printk("policer for all unregistered flow\n");
							tcpl[f_id][tcpl_arate]		= plc_rec[plc_avg]	* 1000;
							tcpl[f_id][tcpl_prate]		= plc_rec[plc_peak]	* 1000;
							tcpl[f_id][tcpl_avg_max]	= plc_rec[plc_abkt]	* 1000;
							tcpl[f_id][tcpl_peak_max]	= plc_rec[plc_pbkt]	* 1000;
							tcpl[f_id][tcpl_abucket]	= plc_rec[plc_abkt]	* 1000;
							tcpl[f_id][tcpl_pbucket]	= plc_rec[plc_pbkt]	* 1000;
						}
						
					}
				}
			}
			
			f_id++;
		}
		
		plc_remove = false;
		
		plc_i++;
		one_plc = func_json(0, plc_i, temp_plc, strlen(temp_plc), &vlen);
		
	}
	
	//printk("policer finish here\n");
	printk("%s finish here\n",__func__);
	return len;
	
} // static ssize_t func_ptable_proc_write()


static int func_ptable_proc_show(struct seq_file *m, void *v)
{
	unsigned int flow_count;
	unsigned char *sp;
	unsigned char *dp;
	
	seq_printf(m," == == == == == == show flow policer table == == == == == == \n");
	//for test
	flow_count = 0;
	while (flow_count<MAX_DAS)
	{
		if (tp_plc[flow_count][plc_sid]||tp_plc[flow_count][plc_did])
		{
			sp = (unsigned char *)&(tp_plc[flow_count][plc_sid]);
			dp = (unsigned char *)&(tp_plc[flow_count][plc_did]);
			
			seq_printf(m,"%d.%d.%d.%d-%d.%d.%d.%d-%u-%u-%u-%u-%u\n",
						  sp[0]&0xff,sp[1]&0xff,sp[2]&0xff,sp[3]&0xff,
						  dp[0]&0xff,dp[1]&0xff,dp[2]&0xff,dp[3]&0xff,
						  tp_plc[flow_count][plc_dir],
						  tp_plc[flow_count][plc_avg],
						  tp_plc[flow_count][plc_abkt],
						  tp_plc[flow_count][plc_peak],
						  tp_plc[flow_count][plc_pbkt]);
		}
		flow_count++;
	}
	return 0;
	
}

static int func_ptable_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, func_ptable_proc_show, NULL);
}

static const struct file_operations ptable_proc_fops = {
	.owner      = THIS_MODULE,
	.open       = func_ptable_proc_open,
	.read       = seq_read,
	.write      = func_ptable_proc_write,
	.llseek     = seq_lseek,
	.release    = single_release,
};

// proc config fuctions
static ssize_t func_config_proc_write(
		struct file *filp, const char __user * buff,
		size_t len, loff_t * offset
	)
{
	char short_af, long_af, alive_af, pd_af;
	char num_str[NUM_LEN];
	unsigned int short_len, long_len, alive_len, pd_len, config_len;
	unsigned int temp_pd;
	const char * temp_cfg_str;
	char * short_pd;
	char * long_pd;
	char * alive_pd;
	char * pd;

	memset(config_str,0,CONFIG_LEN);
	len = len < CONFIG_LEN ? len : CONFIG_LEN;
	if (copy_from_user(config_str, buff, len))
	{
		printk(KERN_INFO "config: copy_from_user() error!\n");
		return -EFAULT;
	}
	
	temp_cfg_str = config_str;
	
	config_str = func_json("config", 0, temp_cfg_str, strlen(temp_cfg_str), &config_len);
	if (config_str == NULL) config_str = temp_cfg_str;
	
	printk("parse %s\n",config_str);
	short_pd = func_json("short", 0, config_str, strlen(config_str), &short_len);
	long_pd = func_json("long", 0, config_str, strlen(config_str), &long_len);
	alive_pd = func_json("alive", 0, config_str, strlen(config_str), &alive_len);
	pd = func_json("period", 0, config_str, strlen(config_str), &pd_len);
	
	if (short_pd != NULL)
	{
		memset(num_str, 0, NUM_LEN);
		snprintf(num_str, NUM_LEN , "%.*s",(int)short_len,short_pd);
		temp_pd = simple_strtoul( short_pd, &short_af, 10);
		config_1.short_pd = (temp_pd>=MIN_SHORT)?temp_pd:MIN_SHORT;
		printk("s %u\n",config_1.short_pd);
	}
	if (long_pd != NULL)
	{
		memset(num_str, 0, NUM_LEN);
		snprintf(num_str, NUM_LEN , "%.*s",(int)long_len,long_pd);
		temp_pd = simple_strtoul( long_pd, &long_af, 10);
		config_1.long_pd = (temp_pd>=MIN_LONG)?temp_pd:MIN_LONG;
		printk("l %u\n",config_1.long_pd);
	}
	if (alive_pd != NULL)
	{
		memset(num_str, 0, NUM_LEN);
		snprintf(num_str, NUM_LEN , "%.*s",(int)alive_len,alive_pd);
		temp_pd = simple_strtoul( alive_pd, &alive_af, 10);
		config_1.alive = (temp_pd>=MIN_ALIVE)?temp_pd:MIN_ALIVE;
		printk("a %u\n",config_1.alive);
	}
	if (pd != NULL)
	{
		memset(num_str, 0, NUM_LEN);
		snprintf(num_str, NUM_LEN , "%.*s",(int)pd_len,pd);
		temp_pd = simple_strtoul( pd,&pd_af, 10);
		config_1.period = (temp_pd>=MIN_PRD)?temp_pd:MIN_PRD;
		printk("p %u\n",config_1.period);
	}
	
	/* deprecated
	 unsigned int config_str_ind = 0;
	 unsigned int num_str_ind = 0;
	 unsigned char num_str[6];
	 bool num_start = false;
	 bool num_end = false;
	 
	 unsigned int config_num[4];
	 unsigned int num_ind= 0;
	 char* after;
	 
	 while (config_str_ind < len)
	 {
	 if ((config_str[config_str_ind] == '-')&&(!num_start))
	 {
	 num_start = true;
	 config_str_ind++;
	 }
	 else if ((config_str[config_str_ind] == '-')&&(num_start))
	 {
	 num_end = true;
	 num_str[num_str_ind]='-';
	 num_str_ind= 0;
	 }
	 else if (config_str[config_str_ind] != '-')
	 {
	 num_str_ind=(num_str_ind<5)?num_str_ind:4;
	 num_str[num_str_ind]=config_str[config_str_ind];
	 num_str_ind++;
	 config_str_ind++;
	 }
	 if (num_start && num_end)
	 {
	 num_start=false;
	 num_end = false;
	 num_ind = (num_ind<4)?num_ind:3;
	 config_num[num_ind]=simple_strtoul( num_str, &after, 10);
	 num_ind++;
	 memset(num_str,0,6);
	 }
	 }
	 snprintf(config_str,CONFIG_LEN,"-%u-%u-%u-%u-",config_num[0],config_num[1],config_num[2],config_num[3]);
	 */
	spd = config_1.short_pd;
	lpd = config_1.long_pd;
	alv = config_1.alive;
	//reset timer according to new config
	if time_after((jiffies + HZ * spd/1000), jiffies)
	{
		mod_timer(&short_timer, jiffies + HZ * spd/1000);//reset timer
	}
	else
	{
		mod_timer(&short_timer, HZ * spd/1000);//reset timer
	}
	
	if time_after((jiffies + HZ * lpd/1000), jiffies)
	{
		mod_timer(&long_timer, jiffies + HZ * lpd/1000);//reset timer
	}
	else
	{
		mod_timer(&long_timer, HZ * lpd/1000);//reset timer
	}
	
	if time_after((jiffies + HZ*alv), jiffies)
	{
		mod_timer(&alive_timer, jiffies + HZ * alv);//reset timer
	}
	else
	{
		mod_timer(&alive_timer, HZ*alv);//reset timer
	}
	
	printk("config finish here");
	return len;
	
} // static ssize_t func_config_proc_write()

static int func_config_proc_show(struct seq_file *m, void *v)
{
	printk("read config\n");
	seq_printf(m, "-%u-%u-%u-%u-\n",config_1.short_pd, config_1.long_pd, config_1.alive, config_1.period);
	//seq_printf(m, "config str is %s\n",config_str);
	return 0;
}

static int func_config_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, func_config_proc_show, NULL);
}

static const struct file_operations config_proc_fops = {
	.owner      = THIS_MODULE,
	.write      = func_config_proc_write,
	.open       = func_config_proc_open,
	.read       = seq_read,
	.llseek     = seq_lseek,
	.release    = single_release,
};

// proc flow count fuctions
static int func_fcount_proc_show(struct seq_file *m, void *v)
{
	unsigned int flow_count;
	unsigned int total_flow;
	unsigned char *sp;
	unsigned char *dp;
	
	flow_count = 0;
	total_flow = 0;
	
	while (flow_count<MAX_FLOW)
	{
		if (tcpl[flow_count][tcpl_sid]||tcpl[flow_count][tcpl_did])
		{
			total_flow++;
		}
		flow_count++;
	}
	seq_printf(m, "-%u-\n",total_flow);
	
	// for test
	seq_printf(m," == == == == == == show tcpl == == == == == == \n");
	flow_count = 0;
	while (flow_count<MAX_FLOW)
	{
		if (tcpl[flow_count][tcpl_on])
		{
			sp = (unsigned char *)&(tcpl[flow_count][tcpl_sid]);
			dp = (unsigned char *)&(tcpl[flow_count][tcpl_did]);
			
			seq_printf(m,"%d.%d.%d.%d-%d.%d.%d.%d-%u-%u-fcount:%u-%u-%u-%u-lcount:%u-%u-%u-%u-time:%u-%u-rate:%u-%u-max:%u-%u-bucket:%u-%u-on:%u-lfmax:%u-%u\n",
						  sp[0]&0xff,sp[1]&0xff,sp[2]&0xff,sp[3]&0xff,
						  dp[0]&0xff,dp[1]&0xff,dp[2]&0xff,dp[3]&0xff,
						  tcpl[flow_count][tcpl_dir],
						  tcpl[flow_count][tcpl_fic],
						  tcpl[flow_count][tcpl_fcount],
						  tcpl[flow_count][tcpl_fcount1],
						  tcpl[flow_count][tcpl_fcount2],
						  tcpl[flow_count][tcpl_fburst],
						  tcpl[flow_count][tcpl_lcount],
						  tcpl[flow_count][tcpl_lcount1],
						  tcpl[flow_count][tcpl_lcount2],
						  tcpl[flow_count][tcpl_lburst],
						  tcpl[flow_count][tcpl_time_sec],
						  tcpl[flow_count][tcpl_time_usec],
						  tcpl[flow_count][tcpl_arate],
						  tcpl[flow_count][tcpl_prate],
						  tcpl[flow_count][tcpl_avg_max],
						  tcpl[flow_count][tcpl_peak_max],
						  tcpl[flow_count][tcpl_abucket],
						  tcpl[flow_count][tcpl_pbucket],
						  tcpl[flow_count][tcpl_on],
						  tcpl[flow_count][tcpl_lfmax],
						  tcpl[flow_count][tcpl_llmax]
						  );
		}
		flow_count++;
	}
	
	return 0;
	
} // static int func_fcount_proc_show(struct seq_file *m, void *v)

static int func_fcount_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, func_fcount_proc_show, NULL);
}

static const struct file_operations fcount_proc_fops = {
	.owner      = THIS_MODULE,
	.open       = func_fcount_proc_open,
	.read       = seq_read,
	.llseek     = seq_lseek,
	.release    = single_release,
};

// proc flow table functions
static int func_ftable_proc_show(struct seq_file *m, void *v)
{
	unsigned int flow_count;
	char * tp_line;
	char flow_dir;
	unsigned int line_count;
	unsigned char *sp;
	unsigned char *dp;

	flow_count = 0;
	memset(flow_table_str,0,FLOW_TABLE_LEN);
	flow_table_str=strcat(flow_table_str, "{\"flows\":[");
	//printk("ftable head\n");
	tp_line = (char *)vmalloc(100);
	line_count = 0;
	
	while (flow_count<MAX_FLOW)
	{
		if (tcpl[flow_count][tcpl_on])
		{
			sp = (unsigned char *)&(tcpl[flow_count][tcpl_sid]);
			dp = (unsigned char *)&(tcpl[flow_count][tcpl_did]);
			flow_dir = (tcpo[flow_count][tcpo_dir] == DIR_U)?'U':'D';
			//printk("flow dir \n");
			
			if (line_count == 0)
			{
				snprintf(tp_line, 100 , "{\"%s:%d.%d.%d.%d:%d.%d.%d.%d:%c:%u\" : \"%u:%u:%u:%u\"}",
							main_interface,
							sp[0]&0xff,sp[1]&0xff,sp[2]&0xff,sp[3]&0xff,
							dp[0]&0xff,dp[1]&0xff,dp[2]&0xff,dp[3]&0xff,
							flow_dir,
							tcpo[flow_count][tcpo_fic],
							tcpo[flow_count][tcpo_fmax],
							tcpo[flow_count][tcpo_favg],
							tcpo[flow_count][tcpo_lmax],
							tcpo[flow_count][tcpo_lavg]);
				line_count=1;
				//printk("flow %u llmax = %u\n",flow_count, tcpl[flow_count][tcpl_llmax]);
			}
			
			else
			{
				snprintf(tp_line, 100 , ",{\"%s:%d.%d.%d.%d:%d.%d.%d.%d:%c:%u\" : \"%u:%u:%u:%u\"}",
							main_interface,
							sp[0]&0xff,sp[1]&0xff,sp[2]&0xff,sp[3]&0xff,
							dp[0]&0xff,dp[1]&0xff,dp[2]&0xff,dp[3]&0xff,
							flow_dir,
							tcpo[flow_count][tcpo_fic],
							tcpo[flow_count][tcpo_fmax],
							tcpo[flow_count][tcpo_favg],
							tcpo[flow_count][tcpo_lmax],
							tcpo[flow_count][tcpo_lavg]);
			}
			
			//printk("%s",tp_line);
			strcat(flow_table_str, tp_line);
			
			/*for test
			 unsigned char *sp = (unsigned char *)&(tcpo[flow_count][tcpo_sid]);
			 unsigned char *dp = (unsigned char *)&(tcpo[flow_count][tcpo_did]);
			 seq_printf(m,"%d.%d.%d.%d-%d.%d.%d.%d-%u-%u-%u-%u-%u-%u\n",
			 sp[0]&0xff,sp[1]&0xff,sp[2]&0xff,sp[3]&0xff,
			 dp[0]&0xff,dp[1]&0xff,dp[2]&0xff,dp[3]&0xff,
			 tcpo[flow_count][tcpo_dir],
			 tcpo[flow_count][tcpo_fic],
			 tcpo[flow_count][tcpo_fmax],
			 tcpo[flow_count][tcpo_favg],
			 tcpo[flow_count][tcpo_lmax],
			 tcpo[flow_count][tcpo_lavg]);
			 */ //for test end
		}
		flow_count++;
	}
	//printk("all lines\n");
	strcat(flow_table_str, "]}\n");
	//printk("ftable end\n");
	seq_printf(m,flow_table_str);
	vfree(tp_line);
	return 0;
	
} // static int func_ftable_proc_show(struct seq_file *m, void *v)

static int func_ftable_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, func_ftable_proc_show, NULL);
}

static const struct file_operations ftable_proc_fops = {
	.owner      = THIS_MODULE,
	.open       = func_ftable_proc_open,
	.read       = seq_read,
	.llseek     = seq_lseek,
	.release    = single_release,
};

/*
// ------+++------ MODULE INIT
*/

//kernel function initiate
static int __init func_hook_init(void)
{
	unsigned int msk_i;
	unsigned int ini_ind;
	unsigned int ini_ind_2;
	
	printk(" == == == =initiate %s == == == == \n", __func__);
	
	plc_on = false;
	
	// get initate time
	do_gettimeofday(&(struct_txc.time));
	printk("get initial time\n");
	
	//initate mask
	ipmask[32] = 0xffffffff;
	//printk("msk_i = 32, msk = %x\n", ipmask[32]);
	msk_i = 31;
	
	while (msk_i>0)
	{
		ipmask[msk_i] = ipmask[msk_i+1] << 1;
		//printk("msk_i = %u, msk = %x\n", msk_i, ipmask[msk_i]);
		msk_i--;
	}
	ipmask[0] = 0;
	
	while (msk_i<33)
	{
		ipmask[msk_i] = ntohl( ipmask[msk_i] );
		msk_i++;
	}
	
	printk("ip mask built\n");
	//printk("msk_i = 0, msk = %x\n", ipmask[0]);
	/*while (msk_i < 32)
	 {
	 printk("msk_i = %u, msk_i+1 = %u, ipmask diff = %x\n", msk_i, msk_i+1, (ipmask[msk_i+1]-ipmask[msk_i]));
	 msk_i++;
	 }
	 */
	
	//initiate tcpl tcpo for test
	ini_ind		= 0;
	ini_ind_2	= 0;
	
	//initiate config
	config_1.short_pd	= 10;			//jdg: was 50 before
	config_1.long_pd	= 200;		//jdg: was 1000 before
	config_1.alive		= 60;
	config_1.period	= 20;
	
	/*
	 while (ini_ind < 16)
	 {
	 tcpl[ini_ind][tcpl_sid]=*(unsigned int *)tcpl_sid_1;
	 tcpo[ini_ind][tcpo_sid]=*(unsigned int *)tcpl_sid_1;
	 if (ini_ind < 8)
	 {
	 tcpl[ini_ind][tcpl_did]=*(unsigned int *)tcpl_did_1;
	 tcpo[ini_ind][tcpo_did]=*(unsigned int *)tcpl_did_1;
	 }
	 else
	 {
	 tcpl[ini_ind][tcpl_did]=*(unsigned int *)tcpl_did_2;
	 tcpo[ini_ind][tcpo_did]=*(unsigned int *)tcpl_did_2;
	 }
	 
	 ini_ind_2=((ini_ind/4)%2);
	 if (ini_ind_2 < 1)
	 {
	 tcpl[ini_ind][tcpl_dir]=DIR_U;
	 tcpo[ini_ind][tcpo_dir]=DIR_U;
	 }
	 else
	 {
	 tcpl[ini_ind][tcpl_dir]=DIR_D;
	 tcpo[ini_ind][tcpo_dir]=DIR_D;
	 }
	 
	 tcpl[ini_ind][tcpl_fic]=ini_ind%4;
	 tcpo[ini_ind][tcpo_fic]=ini_ind%4;
	 
	 tcpl[ini_ind][tcpl_fcount] = 0;
	 tcpl[ini_ind][tcpl_fcount1] = 0;
	 tcpl[ini_ind][tcpl_fcount2] = 0;
	 tcpl[ini_ind][tcpl_fburst] = 0;
	 tcpl[ini_ind][tcpl_lcount] = 0;
	 tcpl[ini_ind][tcpl_lcount1] = 0;
	 tcpl[ini_ind][tcpl_lcount2] = 0;
	 tcpl[ini_ind][tcpl_lburst] = 0;
	 tcpl[ini_ind][tcpl_time_sec] = struct_txc.time.tv_sec;
	 tcpl[ini_ind][tcpl_time_usec] = struct_txc.time.tv_usec;
	 if (tcpl[ini_ind][tcpl_fic] == 0)
	 {
	 tcpl[ini_ind][tcpl_arate]=AVG_RT_U;
	 tcpl[ini_ind][tcpl_prate]=PEK_RT_U;
	 tcpl[ini_ind][tcpl_avg_max]=AVG_BK_U;
	 tcpl[ini_ind][tcpl_peak_max]=PEK_BK_U;
	 tcpl[ini_ind][tcpl_abucket]=AVG_BK_U;
	 tcpl[ini_ind][tcpl_pbucket]=PEK_BK_U;
	 tcpl[ini_ind][tcpl_on]=1;
	 }
	 else
	 {
	 tcpl[ini_ind][tcpl_arate]=AVG_RT_R;
	 tcpl[ini_ind][tcpl_prate]=PEK_RT_R;
	 tcpl[ini_ind][tcpl_avg_max]=AVG_BK_R;
	 tcpl[ini_ind][tcpl_peak_max]=PEK_BK_R;
	 tcpl[ini_ind][tcpl_abucket]=AVG_BK_R;
	 tcpl[ini_ind][tcpl_pbucket]=PEK_BK_R;
	 tcpl[ini_ind][tcpl_on]=1;
	 }
	 
	 tcpo[ini_ind][tcpo_fmax] = 0;
	 tcpo[ini_ind][tcpo_favg] = 0;
	 tcpo[ini_ind][tcpo_lmax] = 0;
	 tcpo[ini_ind][tcpo_lavg] = 0;
	 
	 ini_ind++;
	 }
	 */
	// initial flow table parameters
	ini_ind = 0;
	while (ini_ind < MAX_FLOW)
	{
		struct_TCO_1.struct_flows[ini_ind].SID	= 0;
		struct_TCO_1.struct_flows[ini_ind].DID	= 0;
		ini_ind++;
	}
	
	struct_TCO_1.struct_flows[0].SID = *(unsigned int *)f1_sid_ip;
	struct_TCO_1.struct_flows[1].SID = *(unsigned int *)f2_sid_ip;
	struct_TCO_1.struct_flows[2].SID = *(unsigned int *)f3_sid_ip;
	struct_TCO_1.struct_flows[3].SID = *(unsigned int *)f4_sid_ip;
	
	struct_TCO_1.struct_flows[0].DID = *(unsigned int *)f1_did_ip;
	struct_TCO_1.struct_flows[1].DID = *(unsigned int *)f2_did_ip;
	struct_TCO_1.struct_flows[2].DID = *(unsigned int *)f3_did_ip;
	struct_TCO_1.struct_flows[3].DID = *(unsigned int *)f4_did_ip;
	
	struct_TCO_1.struct_flows[0].link = "eth1";
	struct_TCO_1.struct_flows[1].link = "eth1";
	struct_TCO_1.struct_flows[2].link = "eth1";
	struct_TCO_1.struct_flows[3].link = "eth1";
	
	// initial TCI
	ini_ind = 0;
	while (ini_ind < MAX_DAS)
	{
		struct_TCI_1.struct_DASL[ini_ind].DID = 0;
		ini_ind++;
	}
	prx1.network		= *(unsigned int *)p1_nt;
	prx1.netmask		= p1_mk;
	prx2.network		= *(unsigned int *)p2_nt;
	prx2.netmask		= p2_mk;
	prx3.network		= *(unsigned int *)p3_nt;
	prx3.netmask		= p3_mk;
	
	d1.DID				= *(unsigned int *)did_1;
	d1.struct_dest[0]	= prx1;
	d2.DID				= *(unsigned int *)did_2;
	d2.struct_dest[0] = prx2;
	d2.struct_dest[1] = prx3;
	
	plink.name			= "eth1";
	plink.dcap			= MAX_D_C;
	plink.ucap			= MAX_U_C;
	
	struct_TCI_1.struct_links[0]	= plink;
	//struct_TCI_1.struct_links[1]=plink;
	//struct_TCI_1.struct_links[1].name="eth1";
	struct_TCI_1.struct_DASL[0]	= d1;
	struct_TCI_1.struct_DASL[1]	= d2;
	
	//  initiate DASL table
	tc_dasl[2][dasl_prefix]		= *(unsigned int *)p1_nt;
	tc_dasl[2][dasl_mask]		= 0;
	tc_dasl[2][dasl_did]			= *(unsigned int *)did_1;
	
	tc_dasl[1][dasl_prefix]		= *(unsigned int *)p2_nt;
	tc_dasl[1][dasl_mask]		= 29;
	tc_dasl[1][dasl_did]			= *(unsigned int *)did_2;
	
	tc_dasl[0][dasl_prefix]		= *(unsigned int *)p3_nt;
	tc_dasl[0][dasl_mask]		= 29;
	tc_dasl[0][dasl_did]			= *(unsigned int *)did_2;
	
	//creat proc structure
	struct_qiss_entry = proc_mkdir(qiss_dir_name, NULL);
	
	struct_config_entry	= proc_create(config_name, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH, struct_qiss_entry, &config_proc_fops);
	struct_tp_on_entry	= proc_create(tp_on_name,  S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH, struct_qiss_entry, &tpon_proc_fops);
	struct_tcpo_entry		= proc_mkdir(tcpo_dir_name,struct_qiss_entry);
	struct_tci_entry		= proc_mkdir(tci_dir_name,struct_qiss_entry);
	
	struct_flow_count_entry = proc_create(flow_count_name, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH, struct_tcpo_entry, &fcount_proc_fops);
	struct_flow_table_entry = proc_create(flow_table_name, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH, struct_tcpo_entry, &ftable_proc_fops);
	struct_time_stamp_entry = proc_create(time_stamp_name, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH, struct_tcpo_entry, &tstamp_proc_fops);
	
	struct_links_count_entry	= proc_create(links_count_name,	 S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH, struct_tci_entry, &lcount_proc_fops);
	struct_links_table_entry	= proc_create(links_table_name,	 S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH, struct_tci_entry, &ltable_proc_fops);
	struct_das_count_entry		= proc_create(das_count_name,		 S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH, struct_tci_entry, &dcount_proc_fops);
	struct_das_table_entry		= proc_create(das_table_name,		 S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH, struct_tci_entry, &dtable_proc_fops);
	struct_policer_count_entry = proc_create(policer_count_name, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH, struct_tci_entry, &pcount_proc_fops);
	struct_policer_table_entry = proc_create(policer_table_name, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH, struct_tci_entry, &ptable_proc_fops);
	
	printk("proc structure created\n");
	
	//clear time stamp
	time_stamp			= (char *)vmalloc(TIME_LEN);
	memset(time_stamp, 0, TIME_LEN);
	
	//clear config string
	config_str			= (char *)vmalloc(CONFIG_LEN);
	memset(config_str, 0, CONFIG_LEN);
	
	//clear tp on string
	tp_on_str			= (char *)vmalloc(TP_ON_LEN);
	memset(tp_on_str, 0, TP_ON_LEN);
	
	//clear flow table str
	flow_table_str		= (char *)vmalloc(FLOW_TABLE_LEN);
	memset(flow_table_str, 0, FLOW_TABLE_LEN);
	
	// clear links table str
	links_table_str	= (char*)vmalloc(LINKS_TABLE_LEN);
	memset(links_table_str, 0, LINKS_TABLE_LEN);
	
	link_str				= (char*)vmalloc(LINK_STR_LEN);
	memset(link_str, 0, LINK_STR_LEN);
	
	// clear dasl table str
	in_das				= (char*)vmalloc(DASL_TABLE_LEN);
	memset(in_das, 0 , DASL_TABLE_LEN);
	
	// clear policer table str
	in_plc				= (char*)vmalloc(POLICER_LEN);
	memset(in_plc, 0, POLICER_LEN);
	
	printk("string created\n");
	
	// hooks and timers
	nfho.hook		= func_hook_func;// hook function for counting
	nfho.hooknum	= NF_INET_FORWARD;//ipv4 hook position
	//nfho.hooknum = NF_INET_PRE_ROUTING; //ipv4 hook position
	nfho.pf			= PF_INET;//ipv4
	nfho.priority	= NF_IP_PRI_FIRST;//highest priority
	
	nf_register_hook(&nfho);//register to kernel
	printk("hook registered \n");
	
	func_short_timer_set();//setup timer
	func_long_timer_set();
	func_alive_timer_set();
	printk("timer setted up\n");
	
	printk(KERN_ALERT" == =%s == = at HZ=%d \n", __func__, HZ);
	return 0;
	
} // static int __init func_hook_init(void)

//kernel function exit
static void __exit func_hook_exit(void)
{
	nf_unregister_hook(&nfho);//cancel registration
	del_timer(&short_timer);//remove timer
	del_timer(&long_timer);
	del_timer(&alive_timer);
	printk(KERN_ALERT" == =timer removed == =");
	
	proc_remove(struct_flow_count_entry);//remove proc
	proc_remove(struct_flow_table_entry);
	proc_remove(struct_time_stamp_entry);
	proc_remove(struct_links_count_entry);
	proc_remove(struct_links_table_entry);
	proc_remove(struct_das_count_entry);
	proc_remove(struct_das_table_entry);
	proc_remove(struct_policer_count_entry);
	proc_remove(struct_policer_table_entry);
	proc_remove(struct_config_entry);
	proc_remove(struct_tcpo_entry);
	proc_remove(struct_tci_entry);
	proc_remove(struct_tp_on_entry);
	proc_remove(struct_qiss_entry);
	
	printk(KERN_ALERT" == =proc removed == =\n");
	vfree(time_stamp);
	printk(KERN_ALERT" == =time stamp mem removed == =\n");
	vfree(config_str);
	printk(KERN_ALERT" == =config mem removed == =\n");
	vfree(flow_table_str);
	printk(KERN_ALERT" == =flow table mem removed == =\n");
	vfree(links_table_str);
	printk(KERN_ALERT" == =link table str mem removed == =\n");
	vfree(link_str);
	printk(KERN_ALERT" == =link str mem removed == =\n");
	vfree(in_das);
	printk(KERN_ALERT" == =das table mem removed == =\n");
	vfree(in_plc);
	printk(KERN_ALERT" == =policer mem removed == =\n");
	printk(KERN_ALERT" == =free memory finished == =\n");
	printk(KERN_ALERT" == =%s == =ends\n", __func__);
	
} // static void __exit func_hook_exit(void)

module_init(func_hook_init);
module_exit(func_hook_exit);

//# qstcp.c
