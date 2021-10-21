// qstcp.h

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>			
#include <linux/netdevice.h>
// #include <linux/if.h>				//jdg: add for '#define IFNAMSIZ 16' --  already included indirectly
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/ip.h>				//for ip header
#include <linux/init.h>  
#include <linux/version.h>  
#include <linux/timer.h>			//for timer
#include <linux/delay.h>
#include <linux/export.h>			//for parameters be assessed by forward module
#include <linux/string.h>
#include <linux/timex.h>
#include <linux/rtc.h>
#include <linux/string.h>
#include <linux/inet.h>				//jdg: used for in_aton()

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Zhang Xiaojing Xeon");

#define MAX_FLOW 512					// max number of flows that can be managed
#define MAX_DEST 512					// max number of prefix per DAS
#define MAX_LINK 16					// max number of links (eth0, eth1, wlan0, ...etc) can be managed
#define MAX_DAS 64					// max number of DAS can be managed
#define MAX_D_C 500000000			// default downstream capability
#define MAX_U_C 500000000			// default upstream capability

static unsigned int icap = MAX_U_C;
static unsigned int ecap = MAX_D_C;

//proc names and entries
const char * qiss_dir_name			= "qiss";
const char * tcpo_dir_name			= "tcpo";
const char * tci_dir_name			= "tci";
const char * flow_count_name		= "flows.count";
const char * flow_table_name		= "flows.table";
const char * config_name			= "config";
const char * links_count_name		= "links.count";
const char * links_table_name		= "links.table";
const char * das_count_name		= "das.count";
const char * das_table_name		= "das.table";
const char * policer_count_name	= "policer.count";
const char * policer_table_name	= "policer.table";
const char * time_stamp_name		= "timestamp";
const char * tp_on_name				= "small.tp.on";

static struct proc_dir_entry * struct_qiss_entry;
static struct proc_dir_entry * struct_tcpo_entry;
static struct proc_dir_entry * struct_tci_entry;
static struct proc_dir_entry * struct_flow_count_entry;
static struct proc_dir_entry * struct_flow_table_entry;
static struct proc_dir_entry * struct_config_entry;
static struct proc_dir_entry * struct_links_count_entry;
static struct proc_dir_entry * struct_links_table_entry;
static struct proc_dir_entry * struct_das_count_entry;
static struct proc_dir_entry * struct_das_table_entry;
static struct proc_dir_entry * struct_policer_count_entry;
static struct proc_dir_entry * struct_policer_table_entry;
static struct proc_dir_entry * struct_time_stamp_entry;
static struct proc_dir_entry * struct_tp_on_entry;

// string for time stamp
#define TIME_LEN 128 //max length of time stamp string
static struct timex struct_txc;
//static struct rtc_time tm;
static char * time_stamp;

// string for config
#define CONFIG_LEN 1024
static char * config_str;

// string for tp_on
#define TP_ON_LEN 10
static char * tp_on_str;

// string for flow table
#define FLOW_TABLE_LEN 51200
static char * flow_table_str;

// string for links
#define LINKS_TABLE_LEN 512
#define LINK_STR_LEN 64
static char * links_table_str;
static char * link_str;

// string for dasl table
#define DASL_TABLE_LEN 1024
static char * in_das;

// string for policer

//#define POLICER_LEN 512		//jdg: enlarged from 512 to 10240 -- did not see hang-bug since this enlargement for 2 hours of testing..
#define POLICER_LEN 10240
static char * in_plc;
static unsigned plc_rec[8];

//interface under control
//static char main_interface[IFNAMSIZ] = "br1";			//jdg
static const char main_interface[IFNAMSIZ] = "br1";

//TCPL TCPO TCI index
static unsigned int tcpl[MAX_FLOW][32];
static unsigned int tcpo[MAX_FLOW][8];

const unsigned int tcpl_sid			= 0;
const unsigned int tcpl_did			= 1;
const unsigned int tcpl_dir			= 2;
const unsigned int tcpl_fic			= 3;
const unsigned int tcpl_fcount		= 4;
const unsigned int tcpl_fcount1		= 5;
const unsigned int tcpl_fcount2		= 6;
const unsigned int tcpl_fburst		= 7;
const unsigned int tcpl_lcount		= 8;
const unsigned int tcpl_lcount1		= 9;
const unsigned int tcpl_lcount2		= 10;
const unsigned int tcpl_lburst		= 11;
const unsigned int tcpl_time_sec		= 12;
const unsigned int tcpl_time_usec	= 13;
const unsigned int tcpl_arate			= 14;
const unsigned int tcpl_prate			= 15;
const unsigned int tcpl_avg_max		= 16;
const unsigned int tcpl_peak_max		= 17;
const unsigned int tcpl_abucket		= 18;
const unsigned int tcpl_pbucket		= 19;
const unsigned int tcpl_on				= 20;
const unsigned int tcpl_lfmax			= 21;
const unsigned int tcpl_llmax			= 22;
const unsigned int tcpl_fsum			= 23;
const unsigned int tcpl_lsum			= 24;
const unsigned int tcpl_reg			= 25;

const unsigned int tcpo_sid			= 0;
const unsigned int tcpo_did			= 1;
const unsigned int tcpo_dir			= 2;
const unsigned int tcpo_fic			= 3;
const unsigned int tcpo_fmax			= 4;
const unsigned int tcpo_favg			= 5;
const unsigned int tcpo_lmax			= 6;
const unsigned int tcpo_lavg			= 7;

static unsigned int DIR_U = 0x1;				//jdg: was before: 0xaaaa;
static unsigned int DIR_D = 0x2;				//jdg: was before: 0x5555;

//jdg
//static unsigned char *tcpl_sid_1 = "\x0a\x08\x01\x01";	//flow 1 sid ip address 10.8.1.1
//static unsigned char *tcpl_did_1 = "\x59\x6a\xa6\x1f";	//flow 1 did ip address 89.106.167.255
//static unsigned char *tcpl_did_2 = "\xdf\xff\xff\xff";	//flow 2 did ip 223.255.255.255

//policer table
static unsigned int plc_sid	= 0;
static unsigned int plc_did	= 1;
static unsigned int plc_dir	= 2;
static unsigned int plc_avg	= 3;
static unsigned int plc_abkt	= 4;
static unsigned int plc_peak	= 5;
static unsigned int plc_pbkt	= 6;

//jdg: looks like 'tp_plc' a global (external) variable that is used concurrently in multiple functions (potential bug):
// static bool func_add_flow ()
// static ssize_t func_ptable_proc_write()
// static int func_ptable_proc_show()

//jdg: not only 'tp_plc', also 'this_flow_*' are globals use in multiple functions

//jdg: compile-time warning: missing braces around initializer [-Wmissing-braces]
//static unsigned int tp_plc[MAX_DAS][8]={0};
static unsigned int tp_plc[MAX_DAS][8] = { { 0 } } ;

//flow structure
struct flow {
	unsigned int SID;
	unsigned int DID;
	char *       link;
	unsigned int ucount;
	unsigned int dcount;
	unsigned int ucount1;
	unsigned int dcount1;
	unsigned int ucount2;
	unsigned int dcount2;
	unsigned int uburst;
	unsigned int dburst;
	unsigned int umax;
	unsigned int dmax;
	unsigned int uavg;
	unsigned int davg;
};

// TCO struct
struct TCO {
	struct flow struct_flows[MAX_FLOW];
};

static struct TCO struct_TCO_1;

// initial flow table parameters
static unsigned char *f1_sid_ip = "\x0a\x08\x01\x01";	//flow 1 sid ip address 10.8.1.1
static unsigned char *f2_sid_ip = "\x0a\x08\x01\x01";	//flow 2 sid ip address 10.8.1.1
static unsigned char *f3_sid_ip = "\x0a\x08\x01\x32";	//flow 3 sid ip address 10.8.1.50
static unsigned char *f4_sid_ip = "\x0a\x08\x01\x32";	//flow 4 sid ip address 10.8.1.50

static unsigned char *f1_did_ip = "\x59\x6a\xa7\xff";	//flow 1 did ip address 89.106.167.255
static unsigned char *f2_did_ip = "\xdf\xff\xff\xff";	//flow 2 did ip address 223.255.255.255
static unsigned char *f3_did_ip = "\x59\x6a\xa7\xff";	//flow 3 did ip address 89.106.167.255
static unsigned char *f4_did_ip = "\xdf\xff\xff\xff";	//flow 4 did ip address 223.255.255.255

// TCI architecture
struct ipprefix {
	unsigned int network;
	unsigned int netmask;
};

struct DAS {
	unsigned int DID;
	struct ipprefix struct_dest[MAX_DEST];
};

struct phy_link {
	char * name;
	unsigned int dcap;
	unsigned int ucap;
};

struct TCI {
	struct phy_link struct_links[MAX_LINK];
	struct DAS struct_DASL[MAX_DAS];
};

// initial parameter for TCI

//jdg: removed '89.106.166' by renaming to '89.106.0' so we only has DAS with DID '223.255.255.255'

static unsigned char *did_1 = "\xdf\xff\xff\xff";	// did 1 = 223.255.255.255
static unsigned char *did_2 = "\x59\x6a\x00\x1f";	// did 2 = 89.106.0.31		//jdg: was 89.106.166.31 before

static unsigned char *p1_nt = "\x00\x00\x00\x00";	//prefix 1 0.0.0.0/0
static unsigned char *p2_nt = "\x59\x6a\x00\x00";	//prefix 2 89.106.0.0/29	//jdg: was 89.106.166.0/29 before
static unsigned char *p3_nt = "\x59\x6a\x00\x18";	//prefix 3 89.106.0.24/29	//jdg: was 89.106.166.24/29 before

static unsigned int p1_mk = 0;
static unsigned int p2_mk = 29;
static unsigned int p3_mk = 29;

static struct ipprefix prx1, prx2, prx3;
static struct DAS d1, d2;
static struct phy_link plink;
static struct TCI struct_TCI_1;

//DASL table
//jdg: compile-time warning: missing braces around initializer [-Wmissing-braces]
//static unsigned int tc_dasl[MAX_DAS][3]={0};
static unsigned int tc_dasl[MAX_DAS][3] = { { 0 } };
const unsigned int dasl_prefix = 0;
const unsigned int dasl_mask = 1;
const unsigned int dasl_did = 2;

// default policer parameters
unsigned int AVG_RT_U = 11750000;		// default value for unregistered
unsigned int AVG_BK_U = 8000000;
unsigned int PEK_RT_U = 11750000;
unsigned int PEK_BK_U = 8000000;

#define AVG_RT_R 11750000		// default value for registered
#define AVG_BK_R 8000000
#define PEK_RT_R 11750000
#define PEK_BK_R 8000000

// time config architecture
struct config {
	unsigned int short_pd;
	unsigned int long_pd;
	unsigned int alive;		// in seconds
	unsigned int period;
};
#define MIN_SHORT 5			//jdg: was 20 before
#define MIN_LONG 100
#define MIN_ALIVE 5			// in seconds
#define MIN_PRD 1

// small packet throughput ignored
#define SMALL_TP 1000000					// small packet throughput less than SMALL_TP Bps ignored in a long interval
#define SMALL_LEN 100						// small packet means packet less than SMALL_LEN bytes length
static unsigned int small_bucket = 0;	// bucket of small packet in a long interval

static struct config config_1;

//hook structure entity
static struct nf_hook_ops nfho;
//timer struct entity
struct timer_list short_timer;
struct timer_list long_timer;
struct timer_list alive_timer;

//export for another kernel module
EXPORT_SYMBOL(struct_TCO_1);
EXPORT_SYMBOL(struct_TCI_1);
//EXPORT_SYMBOL(iptb_loc);

// common flow parameters
static unsigned int this_flow_sid	= 0;
static unsigned int this_flow_did	= 0;
static unsigned int this_flow_dir	= 0;
static unsigned int this_flow_fic	= 0;
static unsigned int this_flow_ind	= 0;
static unsigned int this_flow_len	= 0;
static unsigned int this_flow_color	= 0;
static unsigned int this_flow_sec	= 0;
static unsigned int this_flow_usec	= 0;
static bool this_flow_pol = true;
static struct iphdr *ip;
#define FLOW_GREEN 0
#define FLOW_YELLOW 1
#define FLOW_RED 2

// time interval access
static unsigned int spd = 0;
static unsigned int lpd = 0;
static unsigned int prd = 0;
static unsigned int alv = 0;

// tp effect switch on/off
static bool bool_small_tp_effect = false;

//number string length
#define NUM_LEN 20


//jdg: js0n from: https://github.com/quartzjer/js0n

// ============================for json======================================
// ============================for jason======================================
//
// key = string to match or null
// klen = key length (or 0), or if null key then len is the array offset value
// json = json object or array
// jlen = length of json
// vlen = where to store return value length
// returns pointer to value and sets len to value length, or 0 if not found
// any parse error will set vlen to the position of the error
//

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// jdg: no need for prototype:
//
//	//const char * js0n(								//jdg: renamed
//	const char * func_json(
//		const char *key,	size_t klen		,
//		const char *json, size_t jlen		,
//								size_t *vlen
//	);

#ifdef __cplusplus
} /* extern "C" */
#endif

// one strncmp() is used to do key comparison, and a strlen(key) if no len passed in

//jdg: START of https://github.com/quartzjer/js0n/blob/master/src/js0n.c

// gcc started warning for the init syntax used here, is not helpful so don't generate the spam, supressing the warning is really inconsistently supported across versions
#if defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6))
#pragma GCC diagnostic push
#endif
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Winitializer-overrides"
#pragma GCC diagnostic ignored "-Woverride-init"

// only at depth 1, track start pointers to match key/value
#define PUSH(i) if(depth == 1) { if(!index) { val = cur+i; }else{ if(klen && index == 1) start = cur+i; else index--; } }

// determine if key matches or value is complete
#define CAP(i) if(depth == 1) { if(val && !index) {*vlen = (size_t)((cur+i+1) - val); return val;}; if(klen && start) {index = (klen == (size_t)(cur-start) && strncmp(key,start,klen)==0) ? 0 : 2; start = 0;} }

// this makes a single pass across the json bytes, using each byte as an index into a jump table to build an index and transition state

//const char * js0n(								//jdg: renamed
const char * func_json(
	const char *key, size_t klen,
	const char *json, size_t jlen, size_t *vlen
)
{
	const char *val = 0;
	const char *cur, *end, *start;
	size_t index;
	int depth;
	int utf8_remain;
	
	index = 1;
	depth = 0;
	utf8_remain = 0;
	
	static void *gostruct[] =
	{
		[0 ... 255] = &&l_bad,
		['\t'] = &&l_loop, [' '] = &&l_loop, ['\r'] = &&l_loop, ['\n'] = &&l_loop,
		['"'] = &&l_qup,
		[':'] = &&l_loop,[','] = &&l_loop,
		['['] = &&l_up, [']'] = &&l_down,				// tracking [] and {} individually would allow fuller validation but is really messy
		['{'] = &&l_up, ['}'] = &&l_down,
		['-'] = &&l_bare, [48 ... 57] = &&l_bare,		// 0-9
		[65 ... 90] = &&l_bare,								// A-Z
		[97 ... 122] = &&l_bare								// a-z
	};
	static void *gobare[] =
	{
		[0 ... 31] = &&l_bad,
		[32 ... 126] = &&l_loop,							// could be more pedantic/validation-checking
		['\t'] = &&l_unbare, [' '] = &&l_unbare, ['\r'] = &&l_unbare, ['\n'] = &&l_unbare,
		[','] = &&l_unbare, [']'] = &&l_unbare, ['}'] = &&l_unbare, [':'] = &&l_unbare,
		[127 ... 255] = &&l_bad
	};
	static void *gostring[] =
	{
		[0 ... 31] = &&l_bad, [127] = &&l_bad,
		[32 ... 126] = &&l_loop,
		['\\'] = &&l_esc, ['"'] = &&l_qdown,
		[128 ... 191] = &&l_bad,
		[192 ... 223] = &&l_utf8_2,
		[224 ... 239] = &&l_utf8_3,
		[240 ... 247] = &&l_utf8_4,
		[248 ... 255] = &&l_bad
	};
	static void *goutf8_continue[] =
	{
		[0 ... 127] = &&l_bad,
		[128 ... 191] = &&l_utf_continue,
		[192 ... 255] = &&l_bad
	};
	static void *goesc[] =
	{
		[0 ... 255] = &&l_bad,
		['"'] = &&l_unesc, ['\\'] = &&l_unesc, ['/'] = &&l_unesc, ['b'] = &&l_unesc,
		['f'] = &&l_unesc, ['n'] = &&l_unesc, ['r'] = &&l_unesc, ['t'] = &&l_unesc, ['u'] = &&l_unesc
	};
	
	void **go = gostruct;
	
	if(!json || jlen <= 0 || !vlen) return 0;
	*vlen = 0;
	
	// no key is array mode, klen provides requested index
	if(!key)
	{
		index = klen;
		klen = 0;
	}else{
		if(klen <= 0) klen = strlen(key);	// convenience
	}
	
	for(start=cur=json,end=cur+jlen; cur<end; cur++)
	{
		goto *go[(unsigned char)*cur];
	l_loop:;
	}
	
	if(depth) *vlen = jlen;						// incomplete
	return 0;
	
l_bad:
	*vlen = cur - json;						// where error'd
	return 0;
	
l_up:
	PUSH(0);
	++depth;
	goto l_loop;
	
l_down:
	--depth;
	CAP(0);
	goto l_loop;
	
l_qup:
	PUSH(1);
	go=gostring;
	goto l_loop;
	
l_qdown:
	CAP(-1);
	go=gostruct;
	goto l_loop;
	
l_esc:
	go = goesc;
	goto l_loop;
	
l_unesc:
	go = gostring;
	goto l_loop;
	
l_bare:
	PUSH(0);
	go = gobare;
	goto l_loop;
	
l_unbare:
	CAP(-1);
	go = gostruct;
	goto *go[(unsigned char)*cur];
	
l_utf8_2:
	go = goutf8_continue;
	utf8_remain = 1;
	goto l_loop;
	
l_utf8_3:
	go = goutf8_continue;
	utf8_remain = 2;
	goto l_loop;
	
l_utf8_4:
	go = goutf8_continue;
	utf8_remain = 3;
	goto l_loop;
	
l_utf_continue:
	if (!--utf8_remain)
		go=gostring;
	goto l_loop;
	
} // const char *func_json()

#if defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6))
#pragma GCC diagnostic pop
#endif

//jdg: END of https://github.com/quartzjer/js0n/blob/master/src/js0n.c

//# qstcp.h
