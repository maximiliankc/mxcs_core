#include <stdint.h>
#include "Oscillator.h"
#include "Constants.h"

void test_oscillator(const float f, const unsigned int n, float * cosOut, float * sinOut)
{
    // parameters: f: normalised frequency (i.e. fraction of fs)
    //             n: number of samples to iterate over.
    //                 if n is not a multiple of block_size, the last fraction of a block won't be filled in
    //             sinOut/cosOut: sin/cos output of the oscillator
    unsigned int i;

    Oscillator_t osc;
    osc_init(&osc, f);
    for(i=0; i<n; i+= BLOCK_SIZE) {
        osc_step(&osc, cosOut+i, sinOut+i);
    }
}