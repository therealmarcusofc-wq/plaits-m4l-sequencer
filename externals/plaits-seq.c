#include "ext.h"
#include "ext_obex.h"
#include <cmath>
#include <cstring>
#include <vector>

typedef struct _plaits_sequencer {
    t_object ob;
    int step;
    int max_steps;
    double tempo;
    double sr;
    int play_mode; // 0=Forward, 1=Reverse, 2=Converge, 3=Random
    double swing;
    int quantize_scale; // 0-7
    int quantize_root; // 0-11
    int gate_mode[16]; // Gate, glide, tie
    int ratchet[16]; // 1-4 ratchets per step
    double nudge; // offset start position
    int pattern_bank;
    int direction; // forward or reverse for converge mode
    t_outlet *outlet_step;
    t_outlet *outlet_gate;
    t_outlet *outlet_note;
} t_plaits_sequencer;

void *plaits_seq_new(t_symbol *s, long argc, t_atom *argv);
void plaits_seq_free(t_plaits_sequencer *x);
void plaits_seq_assist(t_plaits_sequencer *x, void *b, long m, long a, char *s);
void plaits_seq_bang(t_plaits_sequencer *x);
void plaits_seq_note(t_plaits_sequencer *x, long note);
void plaits_seq_step(t_plaits_sequencer *x, long s);
void plaits_seq_mode(t_plaits_sequencer *x, long mode);
void plaits_seq_swing(t_plaits_sequencer *x, double swing);
void plaits_seq_ratchet(t_plaits_sequencer *x, long step, long count);
void plaits_seq_nudge(t_plaits_sequencer *x, double nudge_val);
void plaits_seq_quantize(t_plaits_sequencer *x, long root, long scale);
void plaits_seq_save_pattern(t_plaits_sequencer *x, long bank);
void plaits_seq_load_pattern(t_plaits_sequencer *x, long bank);

static t_class *plaits_seq_class = NULL;

int quantize_note(int note, int root, int scale);

void ext_main(void *r) {
    t_class *c = class_new("plaits.seq", (method)plaits_seq_new, (method)plaits_seq_free, 
                           sizeof(t_plaits_sequencer), 0L, 0);
    
    class_addmethod(c, (method)plaits_seq_bang, "bang", 0);
    class_addmethod(c, (method)plaits_seq_assist, "assist", A_CANT, 0);
    class_addmethod(c, (method)plaits_seq_note, "note", A_LONG, 0);
    class_addmethod(c, (method)plaits_seq_step, "step", A_LONG, 0);
    class_addmethod(c, (method)plaits_seq_mode, "mode", A_LONG, 0);
    class_addmethod(c, (method)plaits_seq_swing, "swing", A_FLOAT, 0);
    class_addmethod(c, (method)plaits_seq_ratchet, "ratchet", A_LONG, A_LONG, 0);
    class_addmethod(c, (method)plaits_seq_nudge, "nudge", A_FLOAT, 0);
    class_addmethod(c, (method)plaits_seq_quantize, "quantize", A_LONG, A_LONG, 0);
    class_addmethod(c, (method)plaits_seq_save_pattern, "save", A_LONG, 0);
    class_addmethod(c, (method)plaits_seq_load_pattern, "load", A_LONG, 0);
    
    class_register(CLASS_BOX, c);
    plaits_seq_class = c;
}

void *plaits_seq_new(t_symbol *s, long argc, t_atom *argv) {
    t_plaits_sequencer *x = (t_plaits_sequencer *)object_alloc(plaits_seq_class);
    
    x->step = 0;
    x->max_steps = 16;
    x->tempo = 120.0;
    x->sr = 44100.0;
    x->play_mode = 0; // Forward
    x->swing = 0.0;
    x->quantize_scale = 0; // Major
    x->quantize_root = 0; // C
    x->nudge = 0.0;
    x->pattern_bank = 0;
    x->direction = 1; // forward
    
    for (int i = 0; i < 16; i++) {
        x->gate_mode[i] = 0;
        x->ratchet[i] = 1;
    }
    
    x->outlet_step = intout((t_object *)x);
    x->outlet_gate = outlet_new((t_object *)x, NULL);
    x->outlet_note = outlet_new((t_object *)x, NULL);
    
    return x;
}

void plaits_seq_free(t_plaits_sequencer *x) {
}

void plaits_seq_assist(t_plaits_sequencer *x, void *b, long m, long a, char *s) {
    if (m == ASSIST_INLET) {
        sprintf(s, "clock in");
    } else {
        if (a == 0) sprintf(s, "step out");
        if (a == 1) sprintf(s, "gate out");
        if (a == 2) sprintf(s, "note out");
    }
}

void plaits_seq_bang(t_plaits_sequencer *x) {
    // Advance sequencer
    int current_step = (int)(x->step + x->nudge) % x->max_steps;
    if (current_step < 0) current_step += x->max_steps;
    
    // Apply play mode
    switch (x->play_mode) {
        case 0: // Forward
            x->step = (x->step + 1) % x->max_steps;
            break;
        case 1: // Reverse
            x->step = (x->step - 1 + x->max_steps) % x->max_steps;
            break;
        case 2: // Converge
            if (x->direction == 1) {
                x->step++;
                if (x->step >= x->max_steps / 2) x->direction = -1;
            } else {
                x->step--;
                if (x->step < 0) {
                    x->step = 0;
                    x->direction = 1;
                }
            }
            break;
        case 3: // Random
            x->step = (int)(rand() % x->max_steps);
            break;
    }
    
    outlet_int(x->outlet_step, x->step);
}

void plaits_seq_note(t_plaits_sequencer *x, long note) {
    int quantized = quantize_note(note, x->quantize_root, x->quantize_scale);
    outlet_int(x->outlet_note, quantized);
}

void plaits_seq_step(t_plaits_sequencer *x, long s) {
    x->step = s % x->max_steps;
}

void plaits_seq_mode(t_plaits_sequencer *x, long mode) {
    x->play_mode = mode % 4;
}

void plaits_seq_swing(t_plaits_sequencer *x, double swing) {
    x->swing = fmax(0.0, fmin(1.0, swing));
}

void plaits_seq_ratchet(t_plaits_sequencer *x, long step, long count) {
    if (step >= 0 && step < 16) {
        x->ratchet[step] = fmax(1, fmin(4, count));
    }
}

void plaits_seq_nudge(t_plaits_sequencer *x, double nudge_val) {
    x->nudge = nudge_val;
}

void plaits_seq_quantize(t_plaits_sequencer *x, long root, long scale) {
    x->quantize_root = root % 12;
    x->quantize_scale = scale % 8;
}

void plaits_seq_save_pattern(t_plaits_sequencer *x, long bank) {
    x->pattern_bank = bank % 16;
    // In a real implementation, save to file or memory
}

void plaits_seq_load_pattern(t_plaits_sequencer *x, long bank) {
    x->pattern_bank = bank % 16;
    // In a real implementation, load from file or memory
}

int quantize_note(int note, int root, int scale) {
    // Scale degrees: 0=Major, 1=Minor, 2=Dorian, etc.
    static const int scales[8][7] = {
        {0, 2, 4, 5, 7, 9, 11},      // Major
        {0, 2, 3, 5, 7, 8, 10},      // Natural Minor
        {0, 2, 3, 5, 7, 9, 10},      // Dorian
        {0, 1, 3, 5, 7, 8, 10},      // Phrygian
        {0, 2, 4, 6, 7, 9, 11},      // Lydian
        {0, 2, 4, 5, 7, 9, 10},      // Mixolydian
        {0, 2, 4, 7, 9},              // Pentatonic Major
        {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11} // Chromatic
    };
    
    int octave = note / 12;
    int pitch = note % 12;
    int scale_size = (scale == 6) ? 5 : (scale == 7) ? 12 : 7;
    
    // Find nearest note in scale
    int nearest = scales[scale][0];
    int min_dist = abs((scales[scale][0] + root) % 12 - pitch);
    
    for (int i = 1; i < scale_size; i++) {
        int candidate = (scales[scale][i] + root) % 12;
        int dist = abs(candidate - pitch);
        if (dist < min_dist) {
            min_dist = dist;
            nearest = scales[scale][i];
        }
    }
    
    return octave * 12 + (nearest + root) % 12;
}
