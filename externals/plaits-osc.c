#include "ext.h"
#include "ext_obex.h"
#include <cmath>
#include <vector>
#include <cstring>

typedef struct _plaits_osc {
    t_object ob;
    double phase;
    double freq;
    double sr;
    int waveform; // 0=sine, 1=saw, 2=square, 3=triangle
    int osc_type; // 0=osc1, 1=osc2
    double pwm; // pulse width modulation
    double feedback; // self-FM feedback
    double drift; // analogue drift
    double wavetable_pos; // for morphing wavetable
    t_outlet *outlet_signal;
} t_plaits_osc;

void *plaits_osc_new(t_symbol *s, long argc, t_atom *argv);
void plaits_osc_free(t_plaits_osc *x);
void plaits_osc_assist(t_plaits_osc *x, void *b, long m, long a, char *s);
void plaits_osc_dsp64(t_plaits_osc *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags);
void plaits_osc_perform64(t_plaits_osc *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vectorsize);

// DSP object setup
void plaits_osc_dsp_setup(t_plaits_osc *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags);

// Helper functions
double sine_osc(double phase);
double saw_osc(double phase);
double square_osc(double phase, double pwm);
double triangle_osc(double phase);
double wavetable_morph(double phase, double morph_pos);

// Static class setup
static t_class *plaits_osc_class = NULL;

void ext_main(void *r) {
    t_class *c = class_new("plaits.osc~", (method)plaits_osc_new, (method)plaits_osc_free, 
                           sizeof(t_plaits_osc), 0L, 0);
    
    class_addmethod(c, (method)plaits_osc_dsp64, "dsp64", A_CANT, 0);
    class_addmethod(c, (method)plaits_osc_assist, "assist", A_CANT, 0);
    
    // Add parameters as messages
    class_addmethod(c, (method)plaits_osc_freq, "freq", A_FLOAT, 0);
    class_addmethod(c, (method)plaits_osc_waveform, "waveform", A_LONG, 0);
    class_addmethod(c, (method)plaits_osc_pwm, "pwm", A_FLOAT, 0);
    class_addmethod(c, (method)plaits_osc_feedback, "feedback", A_FLOAT, 0);
    
    class_dspinit(c);
    class_register(CLASS_BOX, c);
    plaits_osc_class = c;
}

void *plaits_osc_new(t_symbol *s, long argc, t_atom *argv) {
    t_plaits_osc *x = (t_plaits_osc *)object_alloc(plaits_osc_class);
    
    x->phase = 0.0;
    x->freq = 440.0;
    x->sr = 44100.0;
    x->waveform = 0; // sine
    x->osc_type = 0;
    x->pwm = 0.5;
    x->feedback = 0.0;
    x->drift = 0.0;
    x->wavetable_pos = 0.0;
    
    dsp_setup((t_pxobject *)x, 1); // 1 audio inlet
    x->outlet_signal = outlet_new((t_object *)x, "signal");
    
    return x;
}

void plaits_osc_free(t_plaits_osc *x) {
    dsp_free((t_pxobject *)x);
}

void plaits_osc_assist(t_plaits_osc *x, void *b, long m, long a, char *s) {
    if (m == ASSIST_INLET)
        sprintf(s, "audio in (modulation)");
    else
        sprintf(s, "signal out");
}

void plaits_osc_dsp64(t_plaits_osc *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags) {
    x->sr = samplerate;
    object_method(dsp64, gensym("dsp_add64"), x, plaits_osc_perform64, 0, NULL);
}

void plaits_osc_perform64(t_plaits_osc *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vectorsize) {
    double *in = ins[0];
    double *out = outs[0];
    double phase = x->phase;
    double freq = x->freq;
    double sr = x->sr;
    
    for (long i = 0; i < vectorsize; i++) {
        // FM modulation
        double modulated_freq = freq * (1.0 + in[i] * 0.1);
        double inc = modulated_freq / sr;
        
        // Generate oscillator output
        double sample = 0.0;
        switch (x->waveform) {
            case 0: // sine
                sample = sine_osc(phase);
                break;
            case 1: // saw
                sample = saw_osc(phase);
                break;
            case 2: // square
                sample = square_osc(phase, x->pwm);
                break;
            case 3: // triangle
                sample = triangle_osc(phase);
                break;
            case 4: // wavetable morph
                sample = wavetable_morph(phase, x->wavetable_pos);
                break;
        }
        
        // Add feedback (self-FM)
        if (x->feedback > 0.0) {
            sample += sample * x->feedback * 0.5;
            sample = fmax(-1.0, fmin(1.0, sample)); // soft clip
        }
        
        out[i] = sample;
        phase += inc;
        
        // Wrap phase
        if (phase >= 1.0) phase -= 1.0;
        if (phase < 0.0) phase += 1.0;
    }
    
    x->phase = phase;
}

// Oscillator waveforms
double sine_osc(double phase) {
    return sin(2.0 * M_PI * phase);
}

double saw_osc(double phase) {
    return 2.0 * (phase - floor(phase + 0.5));
}

double square_osc(double phase, double pwm) {
    return (phase < pwm) ? 1.0 : -1.0;
}

double triangle_osc(double phase) {
    if (phase < 0.25)
        return 4.0 * phase;
    else if (phase < 0.75)
        return 2.0 - 4.0 * phase;
    else
        return 4.0 * phase - 4.0;
}

double wavetable_morph(double phase, double morph_pos) {
    // Simple 4-wavetable morphing: sine -> tri -> saw -> square
    double morphed = 0.0;
    if (morph_pos < 0.333) {
        double t = morph_pos / 0.333;
        morphed = sine_osc(phase) * (1.0 - t) + triangle_osc(phase) * t;
    } else if (morph_pos < 0.667) {
        double t = (morph_pos - 0.333) / 0.334;
        morphed = triangle_osc(phase) * (1.0 - t) + saw_osc(phase) * t;
    } else {
        double t = (morph_pos - 0.667) / 0.333;
        morphed = saw_osc(phase) * (1.0 - t) + square_osc(phase, 0.5) * t;
    }
    return morphed;
}

void plaits_osc_freq(t_plaits_osc *x, double f) {
    x->freq = f;
}

void plaits_osc_waveform(t_plaits_osc *x, long waveform) {
    x->waveform = waveform % 5;
}

void plaits_osc_pwm(t_plaits_osc *x, double pwm) {
    x->pwm = fmax(0.01, fmin(0.99, pwm));
}

void plaits_osc_feedback(t_plaits_osc *x, double feedback) {
    x->feedback = fmax(0.0, fmin(1.0, feedback));
}
