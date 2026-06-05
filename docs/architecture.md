# Plaits M4L Sequencer - Architecture

## Overview

The Plaits M4L Sequencer is a Max for Live device combining dual 16-step sequencers with advanced synthesis and modulation. The architecture is split into three layers:

### Layer 1: Max/MSP Patch (plaits-m4l.maxpat)
- User interface and parameter control
- MIDI/note input handling
- Pattern banking and DAW transport sync
- Live parameter mapping

### Layer 2: C++ Externals (High-Performance DSP)
- **plaits.osc~**: Dual oscillators with waveform morphing
- **plaits.filter~**: 12/24dB SVF filter with drive
- **plaits.seq**: Sequencer logic and quantization

### Layer 3: M4L Integration
- Live API connection via `live.thisdevice`
- Clip launching and pattern recall
- Transport sync for step reset

---

## Component Architecture

### Sequencer (plaits.seq)

```
┌─────────────────────────────────┐
│      Sequencer Logic            │
├─────────────────────────────────┤
│ • 16-step pattern               │
│ • Play modes (Fwd/Rev/Conv/Rnd) │
│ • Swing application             │
│ • Ratchet per-step              │
│ • Nudge offset                  │
└──────┬──────────────────────────┘
       │
       ├─→ Quantizer
       │   ├─ Root note (C-B)
       │   └─ Scale (8 modes)
       │
       ├─→ Gate Output
       │   ├─ Gate (on/off)
       │   ├─ Glide time
       │   └─ Tie (legato)
       │
       └─→ Note Output
           └─ MIDI note (0-127)
```

#### Play Modes

1. **Forward**: Step 0→1→2...→15→0
2. **Reverse**: Step 15→14→13...→0→15
3. **Converge**: Step 0→8, 1→7, 2→6... (bounces to center)
4. **Random**: Step jumps randomly

#### Ratchets

Right-click any gate step to set 1-4× repeats:
- 1x: Normal gate
- 2x: Gate triggers twice per step
- 3x: Gate triggers 3× per step
- 4x: Gate triggers 4× per step

#### Quantizer Scales

```
Major:        0 2 4 5 7 9 11
Minor:        0 2 3 5 7 8 10
Dorian:       0 2 3 5 7 9 10
Phrygian:     0 1 3 5 7 8 10
Lydian:       0 2 4 6 7 9 11
Mixolydian:   0 2 4 5 7 9 10
Pentatonic:   0 2 4 7 9
Chromatic:    0 1 2 3 4 5 6 7 8 9 10 11
```

---

### Oscillators (plaits.osc~)

#### OSC 1 - Classic Waveforms

```
┌──────────────────────────┐
│     OSC 1                │
├──────────────────────────┤
│ Waveform Selection       │
│ • Sine                   │
│ • Sawtooth              │
│ • Square (with PWM)     │
│ • Triangle              │
├──────────────────────────┤
│ Modulation              │
│ • PWM (Pulse Width)     │
│ • Self-FM (Feedback)    │
│ • Analogue Drift        │
└──────────────────────────┘
```

#### OSC 2 - Wavetable Morphing

```
┌──────────────────────────┐
│     OSC 2                │
├──────────────────────────┤
│ 4-Wavetable Morphing     │
│ Sine → Tri → Saw → Sq   │
├──────────────────────────┤
│ FM Modulation           │
│ • FM Ratio (coarse)     │
│ • FM Depth              │
│ • Cross-voice FM        │
└──────────────────────────┘
```

#### Phase Accumulator (DSP Core)

```
phase += (freq / sr) per sample
output = wavefunc(phase)

FM Modulation:
phase += (freq * (1 + FM_depth * input) / sr)
```

---

### Filter (plaits.filter~)

**State Variable Filter (SVF)** - 12dB or 24dB slopes

```
┌────────────────────────────────┐
│    SVF Filter                  │
├────────────────────────────────┤
│ Type Selection                 │
│ • Lowpass (LP)                 │
│ • Bandpass (BP)                │
│ • Highpass (HP)                │
├────────────────────────────────┤
│ Parameters                     │
│ • Cutoff (20Hz - 20kHz)        │
│ • Resonance (Q: 0.1 - 10)      │
│ • Drive (soft saturation)      │
│ • Poles (12dB = 1st, 24dB = 2nd) │
└────────────────────────────────┘
```

#### SVF State Update

```
g = tan(π * cutoff / sr)
v3 = input - ic2eq
v1 = (ic1eq + g*v3) * q_inv
v2 = ic2eq + g*v1

lp = v2
bp = v1
hp = v3 - (v1+v2)*q

ic1eq = 2*v1 - ic1eq
ic2eq = 2*v2 - ic2eq
```

---

### Modulation Section

#### 4× LFO (Each with)
- Waveform: Sine, Triangle, Saw, Square
- Rate: 0.1 Hz - 10 kHz
- Sync: Free-running or tempo-locked
- Target: Any parameter (FM depth, filter cutoff, etc.)

#### Mod Envelope - ADSR with Clock Sync
```
Attack    Decay    Sustain    Release
├─────┬─────┬──────────────┬─────┤
│     │     │              │     │
└─────┴─────┴──────────────┴─────┘
      (optional clock sync for tempo-locked decay/release)
```

---

## DSP Signal Flow

```
┌─────────────────┐
│   Sequencer     │ (plaits.seq)
│  16-step        │
└────────┬────────┘
         │ Note + Gate
         │
         ▼
┌─────────────────┐       ┌──────────────┐
│    OSC 1        │◄──────┤    OSC 2     │
│ (4 waveforms)   │       │ (wavetable)  │
└────────┬────────┘       └──────┬───────┘
         │                       │
         │ FM Cross-Mod          │
         └───────────┬───────────┘
                     │
         ┌───────────▼────────────┐
         │  Mix/FM Depth         │
         └───────────┬────────────┘
                     │
         ┌───────────▼────────────┐
         │   Filter (SVF)        │
         │  LP/BP/HP + Drive     │
         └───────────┬────────────┘
                     │
         ┌───────────▼────────────┐
         │   LFO 1-4 Modulation   │
         │   (assignable targets) │
         └───────────┬────────────┘
                     │
         ┌───────────▼────────────┐
         │   Mod Envelope (ADSR)  │
         │   + Clock Sync         │
         └───────────┬────────────┘
                     │
                     ▼
                 Output
```

---

## Pattern Banking

**16 Pattern Slots per Voice**

Each pattern stores:
- All 16 step notes
- Gate/tie/glide per step
- Ratchet settings
- Swing amount
- Play mode
- Quantizer settings

Saved to Live's device state for session persistence.

---

## DAW Transport Sync

On **Play**:
- Sequencer resets to step 0
- Mod envelope retriggered (if sync enabled)
- LFOs restart phase (if sync enabled)

On **Stop**:
- Sequencer pauses
- All notes released

---

## Parameters Overview

### Global
| Parameter | Range | Notes |
|-----------|-------|-------|
| Tempo | 20-300 BPM | Synced to Live tempo |
| Play Mode | Forward/Reverse/Converge/Random | Per-voice |
| Swing | 0-100% | Per-voice swing amount |
| Quantizer Root | C-B | Per-voice |
| Quantizer Scale | Major/Minor/Dorian/... | Per-voice |

### Sequencer (per voice)
| Parameter | Range | Notes |
|-----------|-------|-------|
| Step | 1-16 | Current step |
| Gate | 0-100 | Gate on/off |
| Glide | 0-1000ms | Pitch portamento |
| Tie | On/Off | Legato/note-off control |
| Ratchet | 1-4x | Right-click gate |
| Nudge | -16 to +16 steps | Offset start position |

### Oscillator 1
| Parameter | Range | Notes |
|-----------|-------|-------|
| Waveform | Sine/Saw/Sq/Tri | - |
| PWM | 1-99% | Square wave only |
| Feedback | 0-100% | Self-FM |
| Drift | 0-100% | Analogue instability |

### Oscillator 2
| Parameter | Range | Notes |
|-----------|-------|-------|
| Wavetable | 0-100% | Morphs Sine→Tri→Saw→Sq |
| FM Ratio | 0.5-8 | Coarse FM tuning |
| FM Depth | 0-100% | FM modulation amount |
| Cross-mod | 0-100% | OSC1→OSC2 feedback |

### Filter
| Parameter | Range | Notes |
|-----------|-------|-------|
| Cutoff | 20-20000 Hz | - |
| Resonance (Q) | 0.1-10 | - |
| Type | LP/BP/HP | - |
| Drive | 0-100% | Saturation before filter |
| Poles | 12dB/24dB | Slope (1st/2nd order) |
| Envelope | -100 to +100% | Modulation amount |

### LFO (×4)
| Parameter | Range | Notes |
|-----------|-------|-------|
| Waveform | Sine/Tri/Saw/Sq | - |
| Rate | 0.1-10 kHz | - |
| Sync | Free/Quarter/Eighth/Beat | Tempo-lock modes |
| Depth | 0-100% | To assigned target |
| Target | Any parameter | M4L macro control |

### Mod Envelope
| Parameter | Range | Notes |
|-----------|-------|-------|
| Attack | 1-5000ms | - |
| Decay | 1-5000ms | - |
| Sustain | 0-100% | - |
| Release | 1-5000ms | - |
| Clock Sync | Free/Decay/Release | Optional tempo-lock |

---

## Compilation

### macOS / Linux

```bash
cd externals
make -f Makefile.osx  # macOS (Intel/ARM)
make -f Makefile.linux # Linux
```

Outputs: `plaits.osc~.mxo`, `plaits.filter~.mxo`, `plaits.seq.mxo`

### Windows

```bash
cd externals
msbuild plaits.sln /p:Configuration=Release /p:Platform=x64
```

Outputs: `plaits.osc~.mxe64`, `plaits.filter~.mxe64`, `plaits.seq.mxe64`

---

## Installation

1. **Copy compiled externals** to Max search path:
   - macOS: `~/Library/Max 8/Packages/plaits-m4l-sequencer/externals/`
   - Windows: `%APPDATA%\Cycling '74\Max 8\packages\plaits-m4l-sequencer\externals\`
   - Linux: `~/.max/packages/plaits-m4l-sequencer/externals/`

2. **Copy the patch** to Max for Live packages:
   - `~/Music/Ableton User Library/Instruments/Plaits M4L Sequencer.amxd`

3. **Open in Live** as MIDI Effect or Audio Effect on a track

---

## Development Notes

### Adding New Externals

1. Create `.c` file in `externals/`
2. Update `ext_main()` with class registration
3. Implement DSP methods (`perform64`)
4. Update Makefile with new target
5. Reference in patch via `[plaits.newname~]`

### Performance Considerations

- All DSP runs at 64-bit double precision
- Externals use vector-based processing (up to 4096 samples/vector)
- Filter uses state variables for numerical stability
- Oscillators use efficient polynomial approximations

### Memory Usage

- Per-instance external: ~2 KB overhead
- 16-pattern bank per sequencer: ~4 KB
- Total per device: ~50 KB average

---

## Future Enhancements

- [ ] Wavetable import/export
- [ ] MIDI learn for external controller mapping
- [ ] Preset morphing/interpolation
- [ ] Polyphonic voice expansion (4→8 voices)
- [ ] Step probability and conditional logic
- [ ] Arp modes (Up/Down/Up+Down/As-played)
- [ ] Retrigger envelope per ratchet hit

