	
// Note that only chip at address 0x74 is used 


#define PCA9539_MASK_0  0xff
#define PCA9539_MASK_1  0xff


// Supported commands:
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

int pca9539_set_byte(u8 val, int cmd);
int pca9539_get_byte(u8 *val, int cmd);
