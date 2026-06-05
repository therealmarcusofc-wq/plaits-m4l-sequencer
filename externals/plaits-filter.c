#include "ext.h"
#include "ext_obex.h"
#include <cmath>
#include <cstring>

typedef struct _plaits_filter {
    t_object ob;
    double cutoff;
    double resonance;
    double sr;
    int filter_type; // 0=LP, 1=BP, 2=HP
    int poles; // 12dB or 24dB
    double drive;
    // SVF state variables
    double ic1eq, ic2eq;
    t_outlet *outlet_signal;
} t_plaits_filter;

void *plaits_filter_new(t_symbol *s, long argc, t_atom *argv);
void plaits_filter_free(t_plaits_filter *x);
void plaits_filter_assist(t_plaits_filter *x, void *b, long m, long a, char *s);
void plaits_filter_dsp64(t_plaits_filter *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags);
void plaits_filter_perform64(t_plaits_filter *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vectorsize);

// SVF Filter helper
void svf_process(t_plaits_filter *x, double input, double *lp, double *bp, double *hp);

// Parameter setters
void plaits_filter_cutoff(t_plaits_filter *x, double f);
void plaits_filter_resonance(t_plaits_filter *x, double q);
void plaits_filter_type(t_plaits_filter *x, long type);
void plaits_filter_poles(t_plaits_filter *x, long poles);
void plaits_filter_drive(t_plaits_filter *x, double drive);

static t_class *plaits_filter_class = NULL;

void ext_main(void *r) {
    t_class *c = class_new("plaits.filter~", (method)plaits_filter_new, (method)plaits_filter_free, 
                           sizeof(t_plaits_filter), 0L, 0);
    
    class_addmethod(c, (method)plaits_filter_dsp64, "dsp64", A_CANT, 0);
    class_addmethod(c, (method)plaits_filter_assist, "assist", A_CANT, 0);
    class_addmethod(c, (method)plaits_filter_cutoff, "cutoff", A_FLOAT, 0);
    class_addmethod(c, (method)plaits_filter_resonance, "resonance", A_FLOAT, 0);
    class_addmethod(c, (method)plaits_filter_type, "type", A_LONG, 0);
    class_addmethod(c, (method)plaits_filter_poles, "poles", A_LONG, 0);
    class_addmethod(c, (method)plaits_filter_drive, "drive", A_FLOAT, 0);
    
    class_dspinit(c);
    class_register(CLASS_BOX, c);
    plaits_filter_class = c;
}

void *plaits_filter_new(t_symbol *s, long argc, t_atom *argv) {
    t_plaits_filter *x = (t_plaits_filter *)object_alloc(plaits_filter_class);
    
    x->cutoff = 1000.0;
    x->resonance = 0.5;
    x->sr = 44100.0;
    x->filter_type = 0; // LP
    x->poles = 2; // 24dB
    x->drive = 0.0;
    x->ic1eq = 0.0;
    x->ic2eq = 0.0;
    
    dsp_setup((t_pxobject *)x, 1);
    x->outlet_signal = outlet_new((t_object *)x, "signal");
    
    return x;
}

void plaits_filter_free(t_plaits_filter *x) {
    dsp_free((t_pxobject *)x);
}

void plaits_filter_assist(t_plaits_filter *x, void *b, long m, long a, char *s) {
    if (m == ASSIST_INLET)
        sprintf(s, "audio in");
    else
        sprintf(s, "signal out");
}

void plaits_filter_dsp64(t_plaits_filter *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags) {
    x->sr = samplerate;
    object_method(dsp64, gensym("dsp_add64"), x, plaits_filter_perform64, 0, NULL);
}

void svf_process(t_plaits_filter *x, double input, double *lp, double *bp, double *hp) {
    double g = tan(M_PI * x->cutoff / x->sr);
    double q_inv = 1.0 / (1.0 + x->resonance);
    
    double v3 = input - x->ic2eq;
    double v1 = (x->ic1eq + g * v3) * q_inv;
    double v2 = x->ic2eq + g * v1;
    
    *hp = v3 - (v1 + v2) * x->resonance;
    *bp = v1;
    *lp = v2;
    
    x->ic1eq = 2.0 * v1 - x->ic1eq;
    x->ic2eq = 2.0 * v2 - x->ic2eq;
}

void plaits_filter_perform64(t_plaits_filter *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vectorsize) {
    double *in = ins[0];
    double *out = outs[0];
    
    for (long i = 0; i < vectorsize; i++) {
        double input = in[i];
        
        // Apply drive (soft saturation)
        if (x->drive > 0.0) {
            input = input * (1.0 + x->drive);
            input = tanh(input); // soft clip
        }
        
        double lp, bp, hp;
        svf_process(x, input, &lp, &bp, &hp);
        
        // Select filter type
        double output = 0.0;
        switch (x->filter_type) {
            case 0: // LP
                output = lp;
                break;
            case 1: // BP
                output = bp;
                break;
            case 2: // HP
                output = hp;
                break;
        }
        
        // Apply poles (cascade if 24dB)
        if (x->poles == 4) {
            // Second pass for 24dB slope
            double lp2, bp2, hp2;
            svf_process(x, output, &lp2, &bp2, &hp2);
            switch (x->filter_type) {
                case 0:
                    output = lp2;
                    break;
                case 1:
                    output = bp2;
                    break;
                case 2:
                    output = hp2;
                    break;
            }
        }
        
        out[i] = output;
    }
}

void plaits_filter_cutoff(t_plaits_filter *x, double f) {
    x->cutoff = fmax(20.0, fmin(20000.0, f));
}

void plaits_filter_resonance(t_plaits_filter *x, double q) {
    x->resonance = fmax(0.1, fmin(10.0, q));
}

void plaits_filter_type(t_plaits_filter *x, long type) {
    x->filter_type = type % 3;
}

void plaits_filter_poles(t_plaits_filter *x, long poles) {
    x->poles = (poles > 20) ? 4 : 2; // 24dB or 12dB
}

void plaits_filter_drive(t_plaits_filter *x, double drive) {
    x->drive = fmax(0.0, fmin(1.0, drive));
}
