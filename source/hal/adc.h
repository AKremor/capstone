#include "arm_math.h"

void init_adc();
void read_adc(float32_t *reading);
float32_t convertAdjustedSingle(int32_t raw_sample);
void adcReadChannels(float* channel_data);
