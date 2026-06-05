# Plaits M4L Sequencer - Development Guide

## Project Overview

**Plaits M4L Sequencer** is a professional-grade Max for Live synthesizer device with:
- **Dual 16-step sequencers** with independent control
- **Advanced synthesis** (dual OSCs, FM, SVF filter)
- **4× LFOs** with sync and multiple targets
- **Pattern banking** (16 slots per voice)
- **DAW transport sync** for Live integration

Built with Max/MSP and C++ externals using the Max SDK.

---

## Architecture Summary

### Three-Layer Design

```
┌─────────────────────────────────────────┐
│  Layer 1: Max/MSP Patch (GUI/Control)   │  plaits-m4l.maxpat
├─────────────────────────────────────────┤
│  Layer 2: C++ Externals (High-Perf DSP) │  plaits.osc~, filter~, seq
├─────────────────────────────────────────┤
│  Layer 3: Max for Live Integration      │  live.thisdevice, clip launch
└─────────────────────────────────────────┘
```

### External Components

| Component | File | Purpose |
|-----------|------|---------|
| **plaits.osc~** | `plaits-osc.c` | Dual oscillators (4 waveforms + morphing) |
| **plaits.filter~** | `plaits-filter.c` | 12/24dB SVF filter (LP/BP/HP) with drive |
| **plaits.seq** | `plaits-seq.c` | Sequencer logic, quantizer, play modes |

---

## Compilation

### macOS (Intel & ARM)

```bash
cd externals
make -f Makefile.osx ARCH=arm64    # Apple Silicon (M1/M2/M3)
make -f Makefile.osx ARCH=x86_64   # Intel Mac
```

Output: `plaits.osc~.mxo`, `plaits.filter~.mxo`, `plaits.seq.mxo`

### Linux

```bash
cd externals
make -f Makefile.linux
```

Output: `plaits.osc~.so`, `plaits.filter~.so`, `plaits.seq.so`

### Troubleshooting Builds

1. **Max SDK path error**: Update `MAXSDKPATH` in Makefile
2. **Compiler not found**: Install Xcode (macOS) or gcc (Linux)
3. **Architecture mismatch**: Ensure `-arch` matches your system

---

## Installation for Development

### 1. Clone Repository

```bash
git clone https://github.com/therealmarcusofc-wq/plaits-m4l-sequencer.git
cd plaits-m4l-sequencer
```

### 2. Compile Externals

```bash
cd externals
make -f Makefile.osx        # macOS
# OR
make -f Makefile.linux      # Linux
```

### 3. Add to Max Search Path

**macOS:**
```bash
mkdir -p ~/Library/Max\ 8/Packages/plaits-m4l-sequencer/externals
cp externals/*.mxo ~/Library/Max\ 8/Packages/plaits-m4l-sequencer/externals/
cp plaits-m4l.maxpat ~/Library/Max\ 8/Packages/plaits-m4l-sequencer/
```

**Linux:**
```bash
mkdir -p ~/.max/packages/plaits-m4l-sequencer/externals
cp externals/*.so ~/.max/packages/plaits-m4l-sequencer/externals/
cp plaits-m4l.maxpat ~/.max/packages/plaits-m4l-sequencer/
```

### 4. Test in Max

- Open Max
- Create new patch: `[plaits.osc~]`, `[plaits.filter~]`, `[plaits.seq]`
- Should show without errors

### 5. Use in Max for Live

- Copy `plaits-m4l.maxpat` as `.amxd` to Live's device folder:
  ```
  ~/Music/Ableton User Library/Instruments/Plaits M4L Sequencer.amxd
  ```
- Restart Live
- Load device on MIDI track

---

## Development Workflow

### Editing the Max Patch

1. Open `plaits-m4l.maxpat` in Max
2. Unlock patch (Cmd+E)
3. Edit UI/routing as needed
4. Lock patch (Cmd+E)
5. Save (`Cmd+S`)

### Editing C++ Externals

1. Edit `.c` file in `externals/`
2. Rebuild: `make -f Makefile.osx`
3. Restart Max to reload `.mxo`

### Adding New Features

**Example: Add new LFO waveform**

1. In `plaits-osc.c`, add waveform case:
   ```c
   case 4: // Your new waveform
       sample = your_waveform_func(phase);
       break;
   ```

2. Add setter method:
   ```c
   void plaits_osc_waveform(t_plaits_osc *x, long waveform) {
       x->waveform = waveform % 5;  // Adjust max count
   }
   ```

3. Register in ext_main:
   ```c
   class_addmethod(c, (method)plaits_osc_waveform, "waveform", A_LONG, 0);
   ```

4. Recompile and test

---

## Code Structure

### plaits-osc.c - Oscillator

```c
// Core functions
sine_osc()           // Sine wave
saw_osc()            // Sawtooth
square_osc()         // Square with PWM
triangle_osc()       // Triangle
wavetable_morph()    // 4-wavetable blending

// DSP
plaits_osc_perform64()  // Per-sample processing

// Parameters
plaits_osc_freq()       // Set frequency
plaits_osc_pwm()        // Set pulse width
plaits_osc_feedback()   // Self-modulation
```

### plaits-filter.c - Filter

```c
// SVF state-variable filter
svf_process()           // Core SVF algorithm

// Filter modes
LP (Lowpass)            // Cutoff sweep
BP (Bandpass)           // Vocal formants
HP (Highpass)           // Bright tones

// Parameters
plaits_filter_cutoff()  // Frequency
plaits_filter_resonance() // Q/resonance
plaits_filter_drive()   // Pre-saturation
```

### plaits-seq.c - Sequencer

```c
// Core logic
plaits_seq_bang()       // Advance step

// Play modes
Forward, Reverse, Converge, Random

// Quantizer
quantize_note()         // Scale snapping
scales[8][7]           // 8 scales × 7 degrees

// Pattern storage
16 slots × 2 voices × 16 steps
```

---

## Testing Checklist

- [ ] Externals load in Max without errors
- [ ] OSC 1: All 4 waveforms produce audio
- [ ] OSC 2: Wavetable morphing works
- [ ] Filter: LP/BP/HP all pass through correctly
- [ ] Sequencer: Advances through 16 steps
- [ ] Quantizer: Snaps notes to selected scale
- [ ] Play modes: Forward/Reverse/Converge/Random work
- [ ] Ratchets: 1-4× repeats trigger correctly
- [ ] Swing: Offsets odd steps by amount
- [ ] Pattern save/load: Preserves all settings
- [ ] DAW Sync: Resets on Play/Stop

---

## Performance Optimization Tips

### CPU Usage Reduction

1. **Disable unused LFOs**: Set depth to 0
2. **Reduce filter poles**: Use 12dB if 24dB not needed
3. **Limit OSC complexity**: Fewer simultaneous instances
4. **Quantize internally**: Reduces floating-point overhead

### Memory Usage

- Per-instance: ~50 KB
- Pattern bank: ~4 KB × 16 slots
- Total device: <500 KB for full 2-voice setup

### Audio Latency

- Optimized for <5ms roundtrip
- Uses 64-bit double precision
- Vector-based processing (up to 4096 samples)

---

## Debugging

### Max Debugging

```c
// Add debug output
post("Variable value: %f", x->cutoff);  // Float
post("Step: %ld", x->step);             // Integer
```

Compile and check Max window for output.

### GDB Debugging (Linux/macOS)

```bash
gdb /path/to/max
(gdb) run
(gdb) break plaits_osc_perform64
(gdb) continue
(gdb) print x->phase
```

### Audio Issues

- Check for **NaN/Inf**: Use `isnan()`, `isinf()` checks
- Verify **phase wrapping**: `if (phase >= 1.0) phase -= 1.0;`
- Monitor **filter stability**: Use double precision (not float)

---

## Future Enhancements

Priority order for development:

### High Priority
- [ ] MIDI learn for hardware controllers
- [ ] Wavetable import from files
- [ ] Polyphonic expansion (4→8 voices)
- [ ] Step probability/conditional triggers

### Medium Priority
- [ ] Arpeggiator modes (Up/Down/Up+Down)
- [ ] Preset morphing between patterns
- [ ] MIDI CC mapping for all parameters
- [ ] Sidechain envelope for modulation

### Low Priority
- [ ] Custom wavetable synthesis
- [ ] Granular mode for OSC 2
- [ ] Spectral analysis/visualization
- [ ] GPU-accelerated DSP (future)

---

## Contributing

### Workflow

1. **Fork** the repository
2. **Create feature branch**: `git checkout -b feature/my-feature`
3. **Commit changes**: `git commit -am 'Add feature'`
4. **Push to branch**: `git push origin feature/my-feature`
5. **Create Pull Request** with description

### Code Style

- **C**: BSD-style braces, 4-space indent
- **Max**: Use `[patcherargs]` for modularity
- **Comments**: Explain non-obvious logic

### Testing Before PR

- Compile without warnings
- Test on both macOS and Linux (if possible)
- Update relevant docs
- Include examples in commit message

---

## Resources

### Max Documentation
- [Max SDK Guide](https://cycling74.com/max-sdk-guide/)
- [MSP Externals Tutorial](https://cycling74.com/tutorials/msp-external-development)

### Synthesis Theory
- [SVF Filter Design](https://cytomic.com/files/dsp/SvfLinearTrapOptimised2.pdf)
- [FM Synthesis Basics](https://en.wikipedia.org/wiki/Frequency_modulation_synthesis)
- [Phase Accumulator Oscillators](https://www.dsprelated.com/freebooks/sasp/)

### Live API
- [Max for Live Documentation](https://help.ableton.com/articles/Max-for-Live)
- [live.thisdevice Reference](https://cycling74.com/forums/live-thisdevice)

---

## License

**MIT License** - See LICENSE file

---

## Contact & Support

- **Issues**: GitHub Issues
- **Discussions**: GitHub Discussions
- **Email**: therealmarcusofc@gmail.com

---

## Changelog

### v0.1.0 (Initial Release)
- Dual 16-step sequencers
- Dual oscillators with waveform morphing
- 12/24dB SVF filter
- 4× LFOs with sync
- Pattern banking (16 slots)
- DAW transport sync
- All 8 musical scales + quantizer

---

**Happy hacking! 🎛️**
