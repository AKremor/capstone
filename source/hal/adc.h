#include "arm_math.h"

enum AdcChannelId {
    MAN_CH_0 = 0xC0,
    MAN_CH_1 = 0xC4,
    MAN_CH_2 = 0xC8,
    MAN_CH_3 = 0xCC,
    MAN_CH_4 = 0xD0,
    MAN_CH_5 = 0xD4,
    MAN_CH_6 = 0xD8,
    MAN_CH_7 = 0xCC,
};

void init_adc();
void read_adc(float32_t* reading);
float32_t convertAdjustedSingle(int32_t raw_sample);
void adcReadChannels(float* channel_data);
void adcReadAutoSeqEn();
void adcReadChannelPowerDown();
void adcWriteFeatureSelect();
uint8_t adcSetChannel(AdcChannelId channel_id);
