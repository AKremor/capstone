#include <src/reference_signal/signal_interface.h>
#include <stdint.h>

class SineWave : public ISignal {
   public:
    static ab0_coord getValue(float32_t time);
    static abc_quantity getValueAbc(float32_t time);
};
