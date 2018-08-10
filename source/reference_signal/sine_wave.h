#include <source/reference_signal/signal_interface.h>
#include <stdint.h>

class SineWave : public ISignal {
   public:
    static abc_quantity getValueAbc(float32_t time);
};
