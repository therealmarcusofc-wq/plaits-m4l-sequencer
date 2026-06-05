# Plaits M4L Sequencer - User Guide

## Getting Started

### Installation

1. **Clone or download** the repository:
   ```bash
   git clone https://github.com/yourusername/plaits-m4l-sequencer.git
   cd plaits-m4l-sequencer
   ```

2. **Compile the C++ externals** (choose your OS):
   ```bash
   # macOS
   cd externals
   make -f Makefile.osx
   
   # Linux
   make -f Makefile.linux
   ```

3. **Copy files to Max/M4L**:
   - Copy `plaits-m4l.maxpat` to: `~/Music/Ableton User Library/Instruments/Plaits M4L Sequencer.amxd`
   - Copy `externals/*.mxo` (macOS) or `externals/*.so` (Linux) to Max's search path

4. **Restart Live** and create a new MIDI track

5. **Drop the device** from Max for Live browser onto the track

---

## Basic Operation

### Setting Up Your First Sequence

1. **Create a MIDI track** in Ableton Live
2. **Add Plaits M4L Sequencer** as a MIDI Effect
3. **Play the clip** to trigger the sequencer

### Sequencer Controls

#### Sequencer 1 & 2 (Dual Independent)

Each sequencer has:
- **16-step grid**: Click to enter notes (velocity sensitive in editor)
- **Play Mode**: Forward / Reverse / Converge / Random
- **Swing**: 0-100% swing on odd steps
- **Quantizer Root**: Select root note (C-B)
- **Quantizer Scale**: Choose scale (Major, Minor, Dorian, Phrygian, Lydian, Mixolydian, Pentatonic, Chromatic)

#### Per-Step Controls

Each step has three columns:
1. **Gate** (left): Turns gate on/off
2. **Glide** (middle): 0-1000ms portamento time (0 = no glide)
3. **Tie** (right): ON = legato (no note-off), OFF = normal

#### Ratchets

Right-click any gate cell to set repeats per step:
- **1x**: Single gate (normal)
- **2x**: Gate fires twice
- **3x**: Gate fires three times
- **4x**: Gate fires four times rapidly

Example: Ratchet = 2x with 120 BPM = 8th-note repeats

#### Nudge (Seq Nudge)

Offset the sequence start position:
- **-16 to +16**: Steps to offset
- Useful for polyrhythmic effects between Voice 1 & Voice 2

### Synthesis Engine

#### OSC 1 - Classic Waveforms

```
┌─────────────────────────────┐
│ Waveform: Sine / Saw / Sq / Tri │
├─────────────────────────────┤
│ PWM: 1-99% (Square only)    │
│ Feedback: 0-100% Self-FM    │
│ Drift: 0-100% Instability   │
└─────────────────────────────┘
```

**Tips:**
- Sine for pads, warm basses
- Saw for bright leads
- Square with PWM for evolving timbres
- Increase Drift for analog-sounding variations

#### OSC 2 - 4-Wavetable Morphing

```
┌─────────────────────────────┐
│ Morphing: Sine → Tri → Saw → Sq │
├─────────────────────────────┤
│ FM Ratio: 0.5x - 8x Tuning  │
│ FM Depth: 0-100% Modulation │
│ Cross-Mod: OSC1→OSC2        │
└─────────────────────────────┘
```

**FM Synthesis Tips:**
- FM Ratio 1.0 = OSC2 tunes octave to OSC1
- FM Ratio 2.0 = 1 octave up, 2x harmonic complexity
- FM Ratio 0.5 = 1 octave down, subharmonic character
- Increase FM Depth for metallic/bell tones

#### Filter - State Variable (SVF)

```
┌──────────────────────────────┐
│ Type: LP / BP / HP           │
├──────────────────────────────┤
│ Cutoff: 20 Hz - 20 kHz       │
│ Resonance: 0.1 - 10 (Q)      │
│ Drive: 0-100% (soft saturation) │
│ Poles: 12dB (1-pole) / 24dB (2-pole) │
│ Envelope Depth: -100 to +100% │
└──────────────────────────────┘
```

**Filter Types:**
- **LP (Lowpass)**: Removes highs (classic synth sweep)
- **BP (Bandpass)**: Isolates mid-frequencies (vocal-like effects)
- **HP (Highpass)**: Removes lows (thin/bright tone)

**Tips:**
- High Resonance + low Cutoff = aggressive subs
- BP mode with slow Mod Envelope = formant sweeps
- Drive adds warmth and saturation

### Modulation

#### 4× LFOs (Independent)

Each LFO has:
- **Waveform**: Sine / Triangle / Saw / Square
- **Rate**: 0.1 Hz - 10 kHz (free-running)
- **Sync**: Free / Quarter / Eighth / Beat (tempo-lock)
- **Depth**: 0-100% to assigned target
- **Target**: Any parameter (use M4L macros)

**Typical Uses:**
- **LFO 1**: Filter cutoff modulation (Sine, slow rate)
- **LFO 2**: Pitch modulation (Triangle, 0.5 Hz)
- **LFO 3**: Amplitude modulation (Square, fast)
- **LFO 4**: Pan/stereo effects (Saw, medium)

#### Mod Envelope - ADSR

```
┌─────────────────────────────┐
│ Attack: 1-5000ms            │
│ Decay: 1-5000ms             │
│ Sustain: 0-100%             │
│ Release: 1-5000ms           │
├─────────────────────────────┤
│ Clock Sync:                 │
│ • Free (manual timing)      │
│ • Decay (sync decay time)   │
│ • Release (sync release)    │
└─────────────────────────────┘
```

**Common Envelope Types:**
1. **Quick Pluck**: A=5ms, D=100ms, S=0%, R=50ms
2. **Pad Attack**: A=500ms, D=1000ms, S=100%, R=2000ms
3. **Lead**: A=10ms, D=200ms, S=30%, R=300ms

---

## Pattern Banking

### Save/Load Patterns

**16 Pattern Slots per Voice**

Each pattern stores:
- All 16 step notes and velocities
- Gate/tie/glide per step
- Ratchet settings (1-4×)
- Swing amount
- Play mode
- Quantizer root + scale
- All synthesis parameters

### Using Pattern Bank

1. **Create sequence** in Sequencer 1 or 2
2. **Click "Save Pattern"** button (select bank 1-16)
3. **Switch to different bank** to work on new pattern
4. **Click "Load Pattern"** to recall saved state

**Storage**: All patterns save with Live's project file

---

## DAW Transport Sync

### Automatic Reset on Play/Stop

**On Live Transport Play:**
- Sequencer(s) reset to **Step 1**
- Mod Envelope retriggered (if sync enabled)
- LFOs restart phase (if sync enabled)
- Perfect for intro/fill transitions

**On Live Transport Stop:**
- Sequencers pause
- All notes released
- Device ready for next clip

---

## Workflow Examples

### Example 1: Classic Arpeggio

1. Set **Sequencer 1** to **Forward** mode
2. Enter notes: C4, E4, G4, C5 (repeat across 16 steps)
3. Set **Glide** to 0ms, all **Gate** ON
4. Set **Swing** to 30%
5. Select **Major** scale, root **C**
6. In synthesis: **OSC 1 = Saw**, **Filter = LP**, **Cutoff = 2 kHz**
7. Assign **LFO 1** (Sine) to filter cutoff for evolving motion

### Example 2: Polyrhythmic Tension

1. **Sequencer 1**: Forward mode, 5-note pattern (1/4 note triplets)
2. **Sequencer 2**: Forward mode, 7-note pattern (1/8 notes)
3. Set **Seq 2 Nudge** = +3 steps (offset from Seq 1)
4. Both to same MIDI channel
5. Result: Complex interlocking rhythms

### Example 3: FM Bell Sound

1. **OSC 1** = Sine, 440 Hz
2. **OSC 2** = Sine, FM Ratio = **2.0**, FM Depth = **80%**
3. **Filter** = LP, Cutoff = 3 kHz, Resonance = 2.0
4. **Mod Envelope**: A=5ms, D=2000ms, S=0%, R=500ms
5. **Env Depth**: -100% (negative = closes filter)
6. **Sequencer**: Single note, Gate ON
7. Result: Classic digital bell timbre

### Example 4: Evolving Pad

1. **Sequencer 1** = Whole notes (step 1 ON, rest OFF)
2. **OSC 1** = Triangle, **OSC 2** = Wavetable Morph (at 50%)
3. **FM Depth** = 20%, **Cross-Mod** = 30%
4. **Filter** = LP, Cutoff = 1000 Hz
5. **LFO 1**: Sine, 0.2 Hz, Depth = 40% → Filter Cutoff
6. **LFO 2**: Triangle, 0.1 Hz, Depth = 25% → Wavetable Morph
7. **Mod Envelope**: A=200ms, D=1000ms, S=80%, R=1000ms
8. Result: Rich, evolving ambient texture

---

## Troubleshooting

### No Sound Output

1. Check that **Gate** is ON for at least one step
2. Verify **Volume** in Ableton track is up
3. Ensure **Filter Cutoff** isn't too low
4. Check **Mod Envelope Release** time isn't 0ms

### Sequencer Not Advancing

1. Verify Live's **transport is playing**
2. Check clip is **armed and positioned at start**
3. Ensure **Quantizer** isn't restricting all notes (no scale match)

### Glitchy/Clipping Audio

1. Reduce **Drive** on filter
2. Lower **Feedback** on OSC 1
3. Reduce **FM Depth** on OSC 2
4. Check **Resonance** isn't too extreme

### Externals Not Loading

1. Verify path: `~/Library/Max 8/packages/plaits-m4l-sequencer/externals/`
2. Try restarting Max
3. Check architecture match (Intel vs. ARM on macOS)
4. On Linux, ensure libm is installed: `sudo apt install libm-dev`

---

## Keyboard Shortcuts (in Max Editor)

- **Cmd/Ctrl + E**: Edit mode toggle
- **Cmd/Ctrl + R**: Run patch
- **Cmd/Ctrl + .**:  Stop audio

---

## Performance Notes

- **CPU Usage**: ~8-12% per instance (depends on OSC complexity)
- **RAM**: ~50 KB per device instance
- **Latency**: <5ms with typical buffer settings

For best performance:
- Disable LFO sync on LFOs you don't use
- Lower **Filter Poles** if CPU-constrained
- Reduce number of simultaneous instances

---

## Tips & Tricks

1. **Polyrhythm Setup**: Use two sequences with different lengths via Nudge
2. **Sidechain Effect**: Route filter cutoff to a gate signal externally
3. **Chord Memory**: Save different patterns as chord voicings, load per-clip
4. **Arpeggiator**: Set one sequencer to Converge + fast swing for arpeggios
5. **Cross-voice Modulation**: Quantize both sequencers to same root for harmonic control

---

## Support & Contributing

For issues, feature requests, or contributions:
- GitHub Issues: https://github.com/yourusername/plaits-m4l-sequencer/issues
- Pull Requests welcome!

---

## License

MIT License - See LICENSE file in repository

---

## Credits

Built with the **Max SDK** and inspired by **Plaits** oscillator design principles.
