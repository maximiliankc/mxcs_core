/* MXCS Core Synthesizer implementation
   copyright Maximilian Cornwell 2023
*/
#include "Synth.h"

#define SEMITONE (1.0594630943592953)
#define C_MINUS_1 (8.175798915643707/SAMPLING_FREQUENCY)

Synth_t::Synth_t(): voice(&settings) {
    // calculate the frequency table
    currentNote = 0;
    frequencyTable[0] = C_MINUS_1;
    for(uint8_t i = 1; i < NOTES; i++) {
        frequencyTable[i] = SEMITONE*(frequencyTable[i-1]);
    }
    env_settings_init(&settings);
    mod_init(&mod);
}

void Synth_t::set_attack(float a) {
    env_set_attack(&settings, a);
}

void Synth_t::set_decay(float d) {
    env_set_decay(&settings, d);
}

void Synth_t::set_sustain(float s) {
    env_set_sustain(&settings, s);
}

void Synth_t::set_release(float r) {
    env_set_release(&settings, r);
}

void Synth_t::set_mod_f(float freq) {
    osc_setF(&(mod.lfo), freq/SAMPLING_FREQUENCY);
}

void Synth_t::set_mod_depth(float depth) {
    mod.modRatio = depth;
}

void Synth_t::press(uint8_t note) {
    float f = frequencyTable[note];
    voice.press(f);
    currentNote = note;
}

void Synth_t::release(uint8_t note) {
    if (note==currentNote) {
        voice.release();
    }
}

void Synth_t::step(float * out) {
    voice.step(out);
    mod_step(&mod, out);
}

#ifdef SYNTH_TEST_

float * Synth_t::get_freq_table() {
    return frequencyTable;
}

extern "C" {
    void test_synth(const float a, const float d, const float s, const float r,\
                    const float modDepth, const float modFreq,\
                    const unsigned int presses, unsigned int pressNs[], uint8_t pressNotes[],\
                    const unsigned int releases, unsigned int releaseNs[], uint8_t releaseNotes[],\
                    const unsigned int n, float envOut[]) {
        // parameters:  a: attack time (in samples)
        //              d: decay time (in samples)
        //              s: sustain level (amplitude between 0 and 1)
        //              r: release time (in samples)
        //              modDepth: modulation depth
        //              modFreq: modulation frequency
        //              presses: number of presses
        //              pressNs: times at which to press
        //              notes: MIDI notes to press at each time step
        //              releases: number of releases
        //              releaseNs: times at which to release
        //              n: number of samples to iterate over.
        //                  if n is not a multiple of block_size, the last fraction of a block won't be filled in
        //              envOut: generated envelope
        Synth_t synth;
        unsigned int pressCount = 0;
        unsigned int releaseCount = 0;
        synth.set_attack(a);
        synth.set_decay(d);
        synth.set_sustain(s);
        synth.set_release(r);
        synth.set_mod_depth(modDepth);
        synth.set_mod_f(modFreq);
        for(unsigned int i=0; i+BLOCK_SIZE <= n; i+= BLOCK_SIZE) {
            if(pressCount < presses && i >= pressNs[pressCount]) {
                synth.press(pressNotes[pressCount]);
                pressCount++;
            }
            if(releaseCount < releases && i >= releaseNs[releaseCount]) {
                synth.release(releaseNotes[releaseCount]);
                releaseCount++;
            }
            synth.step(envOut + i);
        }
    }

    void test_frequency_table(float freqs[]) {
        // parameters:
        Synth_t synth;
        for(unsigned int i = 0; i<NOTES; i++) {
            freqs[i] = synth.get_freq_table()[i];
        }
}
}
#endif // SYNTH_TEST_