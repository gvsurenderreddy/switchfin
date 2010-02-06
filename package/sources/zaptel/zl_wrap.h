

#include "zaptel.h"

int echo_can_wrap_init(void);
void echo_can_wrap_shutdown(void);
int echo_can_wrap_tail_length(struct zt_chan *chan, int tail_length);

void echo_can_init(void);
void echo_can_shutdown(void);
int echo_can_create(struct zt_echocanparams *ecp, struct zt_echocanparam *p,
			   struct echo_can_state **ec);
void echo_can_free(struct echo_can_state *ec);
short echo_can_update(struct echo_can_state *ec, short iref, short isig);
int echo_can_traintap(struct echo_can_state *ec, int pos, short val);
void echo_can_identify(char *buf, size_t len);



