#include <linux/kernel.h>       
#include <linux/module.h>      

#include "zaptel.h"
#include "zl_wrap.h"

extern int echocan_tail_length(int channel, int tail_ms);
static int(*ec_tail_length)(int,int) = NULL;

EXPORT_SYMBOL(echo_can_wrap_init);
EXPORT_SYMBOL(echo_can_wrap_shutdown);
EXPORT_SYMBOL(echo_can_wrap_tail_length);
EXPORT_SYMBOL(echo_can_init);
EXPORT_SYMBOL(echo_can_shutdown);
EXPORT_SYMBOL(echo_can_create);
EXPORT_SYMBOL(echo_can_free);
EXPORT_SYMBOL(echo_can_traintap);
EXPORT_SYMBOL(echo_can_update);
EXPORT_SYMBOL(echo_can_identify);

#define EC_MODULE_NAME		"PR1 Echo Canceller: "
#define DEBUG(args...)		printk(KERN_DEBUG EC_MODULE_NAME args);
#define ERROR(args...)		printk(KERN_ERR EC_MODULE_NAME args);
#define WARNING(args...)	printk(KERN_WARNING EC_MODULE_NAME args);
#define INFO(args...)		printk(KERN_INFO EC_MODULE_NAME args);


/* This is an interface of software echo canceler. We don't need from it. */
void echo_can_init(void)
{
}


/* This is an interface of software echo canceler. We don't need from it. */
void echo_can_shutdown(void)
{
}


/* This is an interface of software echo canceler. We don't need from it. */
int echo_can_create(struct zt_echocanparams *ecp, struct zt_echocanparam *p,
			   struct echo_can_state **ec)
{
  	return 0;
}


/* This is an interface of software echo canceler. We don't need from it. */
void echo_can_free(struct echo_can_state *ec)
{
}


/* This is an interface of software echo canceler. We don't need from it. */
int echo_can_traintap(struct echo_can_state *ec, int pos, short val)
{

	ERROR("This function couldn't be called");
	return 1;
}


/* This is an interface of software echo canceler. We don't need from it. */
short echo_can_update(struct echo_can_state *ec, short iref, short isig)
{
	
	ERROR("This function couldn't be called");
	return 0;
}


/* This is an interface of software echo canceler. */
void echo_can_identify(char *buf, size_t len){
	
	strncpy(buf, "Zarlink LEC hardware", len);
}


int echo_can_wrap_init(void)
{
	INFO("module open\n");
	ec_tail_length = symbol_get(echocan_tail_length);
	if (!ec_tail_length) {
		WARNING( "%s: missing echo canceller module!\n",
		       __FUNCTION__);
		return -ENODEV;
	}
	//ec_tail_length(0,0);	/* Zaptel doesn't init channel 0 */
	return 0;	
}


void echo_can_wrap_shutdown(void)
{
	INFO("module close\n");
	if (ec_tail_length){
		symbol_put(echocan_tail_length);	
	}
}


int echo_can_wrap_tail_length(struct zt_chan *chan, int tail_length)
{
	DEBUG("tap length: %u\n", tail_length);
	if( ec_tail_length ){
		ec_tail_length(chan->channo , tail_length);
		//ec_tail_length(chan->channo - 1, tail_length);
	}else{
		return -ENODEV;
	}
  	return 0;
}


