# Audio Editor Application

This is a desktop application for creating, editing, and processing audio files.  
The application allows users to work with common audio formats, visualize waveforms, edit audio fragments, apply effects, and export the final result in different formats.

---

## Features

- Create new audio projects with custom parameters  
- Import audio files (WAV, MP3, FLAC)  
- Waveform visualization with zooming and navigation  
- Basic audio editing:
  - Cut
  - Copy
  - Paste
  - Trim
  - Delete selected fragments  
- Audio processing:
  - Equalizer (based on biquad filters)
  - Noise reduction (FFT-based spectral subtraction)
  - Volume normalization
  - Fade In/Fade Out effect
  - Reverse effect 
- Audio recording from microphone  
- Undo / Redo system  
- Export audio files to supported formats  
- Input validation and robust error handling  

---

## Technologies Used

- **Programming Language:** C++  
- **Framework:** Qt Framework  
- **Audio Processing:**  
  - Fast Fourier Transform (FFT)  
  - Biquad digital filters
  - Qt Multimedia  
- **GUI:** Qt Widgets, Qt Gui   
- **Architecture:** Modular, signal-slot based design  

---

## Supported Audio Formats

- WAV  
- MP3  
- FLAC

---

## Installation

The AudioEditor application is distributed as a portable Windows build created using
Qt Creator and compiled with the MSVC toolchain.

Due to the monolithic architecture of the application, the installation process is
limited to deploying the compiled executable together with all required dynamic
libraries.

### System Requirements
- Windows x64

### Installation Steps
1. Download the deployment archive from the
   [Releases](https://github.com/IrynaMatuliak/AudioEditor/releases) section.
2. Create a directory for the application (it is recommended to use a user directory
   that does not require administrator privileges).
3. Extract the contents of the archive into the created directory.
4. Ensure that `audio_editor.exe` and all `.dll` files are located in the same root
   directory.
5. Run `audio_editor.exe` to launch the application.
