# xxprofile Viewer

`xxprofileViewer` is a desktop tool for opening and inspecting `.xxprofile` capture files.

## Opening Captures

- Open a `.xxprofile` capture file from the command line:

  ```bash
  xxprofile_viewer_cmake path/to/file.xxprofile
  ```

- On supported desktop platforms, double-clicking a `.xxprofile` file can also open it in the viewer.
- Dragging a `.xxprofile` file onto the viewer window opens it.
- When no capture is open, the viewer prompts you to open a `.xxprofile` file.
- After a capture is loaded, the window title shows useful capture information:
  - file name
  - total captured time
  - frame count
  - compression ratio

## View Modes

Use `View -> View Type` to switch between two ways of exploring the capture:

- `Frame`: inspect frame costs and drill into one selected frame.
- `Timeline`: inspect work by real time across all threads.

## Frame View

The `Frame` view is useful when you want to find expensive frames and understand what happened inside a selected frame.

### Frame Overview

- Shows recorded frames for each thread.
- Each row represents one thread.
- Frame bars show relative frame cost, making spikes easy to spot.
- The thread overlay shows:
  - thread id
  - thread name
  - visible frame range
  - maximum frame time
- Hovering a frame shows:
  - frame id
  - frame start time
  - frame end time
  - frame duration
  - number of recorded nodes
- Clicking a frame selects it for detailed inspection.

### Frame Range Navigation

- When there are too many frames to fit on screen, a compact overview appears above the frame row.
- The compact overview lets you move through large captures quickly.
- Clicking or dragging in the overview changes the visible frame range across all threads.
- Hovering the overview shows:
  - represented frame range
  - represented time range
  - maximum frame cost in that range

### Frame Detail

- Shows the selected frame as a call tree.
- Each entry shows:
  - percentage of parent time
  - total time
  - scope/function name
- Expensive entries are highlighted:
  - red for very expensive entries
  - yellow for moderately expensive entries
  - white for lower-cost entries
- Hovering an entry shows:
  - percentage of the whole frame
  - percentage of the parent entry
  - total time
  - self time
  - start and end time inside the selected frame
  - scope/function name

### Combined Mode

- When repeated work exists in the selected frame, a `Combined` option is available.
- Combined mode groups repeated entries together.
- Grouped entries show:
  - call count
  - total time
  - percentage of parent time
  - scope/function name
- Hovering a grouped entry shows:
  - call count
  - frame percentage
  - parent percentage
  - total time
  - self time
  - average time when there is more than one call

## Timeline View

The `Timeline` view is useful when you want to understand how work is distributed over time across threads.

### Time Ruler

- Shows a shared time ruler across the top of the capture.
- Tick spacing adjusts as you zoom.
- Time labels automatically use appropriate units.
- Vertical guide lines make it easier to compare work across threads.

### Thread Tracks

- Shows each recorded thread as a separate track.
- Thread headers show:
  - thread id
  - thread name when available
- Each thread includes a `Frames` row.
- Frame bars are placed by their real start and end time.
- Frame labels appear when there is enough space.
- Hovering a frame bar shows:
  - thread id and name
  - frame id
  - start time
  - end time
  - frame duration
  - number of recorded nodes
- Clicking a frame selects it.

### Expanded Thread Details

- Use the arrow button on a thread to expand or collapse it.
- Expanded threads show nested work by call depth.
- Each depth row shows the work that ran at that nesting level.
- Wider bars show their scope/function names directly.
- Hovering a bar shows:
  - scope/function name
  - frame id
  - start and end time inside that frame
  - duration

### Timeline Navigation

- `Ctrl + mouse wheel`: zoom in or out around the mouse position.
- Middle-button drag: pan horizontally.
- Right-button drag: pan horizontally.
- Horizontal mouse wheel: pan the visible time range.
- The visible range stays within the loaded capture.

## Build and Run

From the repository root:

```bash
bash build.sh cmake_viewer
```

The viewer executable is copied to:

```text
out/xxprofile_viewer_cmake
```
