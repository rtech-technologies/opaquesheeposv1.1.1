# RSL (Rtech Script Language) Guide

RSL is the official scripting and command language of OpaqueSheep OS. It provides a high-level, human-readable interface for interacting with system services.

## Core Commands

### System Information
- `help`: Displays a list of available commands.
- `info`: Shows the current OS name, version, and memory usage status.
- `uptime`: Displays the system's active time in ticks.

### File Operations
- `ls`: Lists all files in the virtual filesystem.
- `cat <path>`: Displays the content of the specified file.
- `touch <path>`: Creates a new, empty file at the specified path.
- `rm <path>`: Deletes the specified file from the filesystem.
- `write <path> <text>`: Writes the provided text into the specified file.
- `format`: Re-initializes the virtual filesystem with the default label.

### UI & Interaction
- `echo <text>`: Prints the provided text directly to the screen.
- `clear`: Clears the display and resets the cursor position.

### System Control
- `run <path>`: Executes an RSL script file line-by-line.
- `panic`: Triggers a fatal kernel error (Red Screen of Death) for testing.

## Scripting Rules
- Commands are case-sensitive (all lowercase).
- Arguments are separated by spaces.
- Lines starting with `\0` or empty lines are ignored.
- Scripts executed via `run` must reside within the OpaqueSheep VFS.
