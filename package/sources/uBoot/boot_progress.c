/*
 * boot_progress.c - Display Status Leds on boot status
 *
 * Prepared for BR4-Appliance board mark@astfin.org
 */
 
#include <common.h>
#include <i2c.h>

#ifdef CONFIG_SHOW_BOOT_PROGRESS
# define SHOW_BOOT_PROGRESS(arg)        show_boot_progress(arg)
#else
# define SHOW_BOOT_PROGRESS(arg)
#endif

#define GPIO_CHIP_ADDR	0x74

void led_blue_on();
void led_blue_off();
void led_red_on();
void led_red_off();

enum pca9539_cmd
{
        PCA9539_INPUT_0         = 0,
        PCA9539_INPUT_1         = 1,
        PCA9539_OUTPUT_0        = 2,
        PCA9539_OUTPUT_1        = 3,
        PCA9539_INVERT_0        = 4,
        PCA9539_INVERT_1        = 5,
        PCA9539_DIRECTION_0     = 6,
        PCA9539_DIRECTION_1     = 7,
};

void show_boot_progress(int status)
{
	printf ("Status: %d\n", status);
        led_red_off();
	led_blue_off();

	if (status < -2 ) {
		led_red_on();
		return;
	}
	switch(status){
		case -1: /* Image Header has bad magic number */
			led_red_on();
			break;
						
                case 1:
                        break;
                case 2:
                        break;
                case 3:
                        break;
                case 4:
                        break;
                case 5:
                case 6:
                        break;
                case 7:
                case 8:	/* Image Type check ok */
			led_blue_on();
                        break;
                case 9:
                case 10:
                case 11:
                case 12:
                case 13:
                case 14:
                case 15: /* All preparation done, transferring control to OS */
			led_blue_on();
                        break;
                default:
                        break;
        }
}


/*************************************************/
/* LED Functions 
 ************************************************/
void led_red_on() {
       *pPORTG_FER     &= ~(PG6);
        *pPORTGIO_DIR   |= PG6;
        *pPORTGIO_INEN  &= ~(PG6);
        *pPORTGIO       &= ~(PG6);
        *pPORTGIO       |= PG6;

}

void led_red_off() {
       *pPORTG_FER     &= ~(PG6);
       *pPORTGIO_DIR   |= PG6;
       *pPORTGIO_INEN  &= ~(PG6);
       *pPORTGIO       &= ~(PG6);
}


void led_blue_on() {
       *pPORTG_FER     &= ~(PG7);
        *pPORTGIO_DIR   |= PG7;
        *pPORTGIO_INEN  &= ~(PG7);
        *pPORTGIO       &= ~(PG7);
        *pPORTGIO       |= PG7;
}

void led_blue_off() {
       *pPORTG_FER     &= ~(PG7);
        *pPORTGIO_DIR   |= PG7;
        *pPORTGIO_INEN  &= ~(PG7);
        *pPORTGIO       &= ~(PG7);
}
