# mog

This is a port of the sound library written by Minoru Akao for Final Fantasy VII.<br>
This library was later expanded for further SquareSoft games.

Several parts of the library were stripped due to being irrelevant to playing single songs.<br>
These include: cross-song fading, sound effects, streaming, and song caching.

The driver code was sourced from the [ff7-decomp](https://github.com/Xeeynamo/ff7-decomp) reverse engineering project.

The library includes a PlayStation SPU emulator for audio synthesis.<br>
This was adapted from [rpsx](https://github.com/KieronJ/rpsx)'s SPU implementation in order to port Metal Gear Solid's sound engine with the [raven](https://github.com/KieronJ/raven) project. The emulator has been expanded to support pitch modulation, which Metal Gear Solid's sound driver does not use.<br>

The following sources were referenced to improve rpsx's SPU implementation and fix bugs:
- nocash's [psx-spx](https://psx-spx.consoledev.net) hardware documentation.
- [DuckStation](https://github.com/stenzek/duckstation)'s SPU implementation.
- [Mednafen](https://mednafen.github.io)'s reverb implementation.

This project supports both FLAC and Opus Ogg output.

## required packages
`libflac-dev libopusenc-dev`

## building
`make`

## usage

Mog depends on the instrument files `INSTR.DAT` and `INSTR.ALL` existing in the `data` directory.
Additionally, `INSTR2.DAT` and `INSTR2.ALL` must also exist in the `data directory` in order to play One-Winged Angel.

`mog [-d] [-t time] -o output -e encoder seq_file`

The following options are supported:
```
-d          Enables debug output.
-t time     Specifies the time in seconds to sequence. (default 60s)
-o output   Specifies the name of the output wave file.
-e encoder  Specifies the encoder to use. ("flac" | "opus")
```
