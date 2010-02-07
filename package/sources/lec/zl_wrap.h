

#include <dahdi/kernel.h>

int echo_can_wrap_init(void);
void echo_can_wrap_shutdown(void);
int echo_can_wrap_tail_length(struct dahdi_chan *chan, int tail_length);

void echo_can_init(void);
void echo_can_shutdown(void);
int echo_can_create(struct dahdi_echocanparams *ecp, struct dahdi_echocanparam *p, struct dahdi_echocan_state **ec);
void echo_can_free(struct dahdi_echocan_state *ec);
short echo_can_update(struct dahdi_echocan_state *ec, short iref, short isig);
int echo_can_traintap(struct dahdi_echocan_state *ec, int pos, short val);
void echo_can_identify(char *buf, size_t len);



