# xxprofile Viewer

`xxprofileViewer` is the desktop viewer for `.xxprofile` capture files. It is built with the existing Dear ImGui + GLFW + OpenGL2 stack and uses the project loader to read profile archives.

## Launch and File Loading

- Open a capture by passing a file path on the command line:

  ```bash
  xxprofile_viewer_cmake path/to/file.xxprofile
  ```

- On supported desktop platforms, double-clicking a `.xxprofile` document can also open it through the GLFW document-open callback.
- When no capture is loaded, the main window shows `double click a .xxprofile file to open.`
- After a file is loaded, the window title includes:
  - capture file name
  - total captured time
  - maximum frame count across threads
  - archive compression ratio

## Main Window

- The viewer uses a full-window ImGui dock space.
- `View -> View Type` switches between:
  - `Frame`
  - `Timeline`
- The layout is rebuilt automatically when switching view types.

## Frame View

The `Frame` view is the original two-pane analysis mode.

### Left Pane: Per-Thread Frame Lines

- Shows one frame histogram per recorded thread.
- Each thread row displays frame cost over time.
- Frame values are scaled from each thread's maximum frame cost so spikes are visible.
- The overlay text shows:
  - thread id
  - thread name
  - visible frame id range
  - maximum frame time for that thread
- Hovering a frame bar shows:
  - frame id
  - frame start and end time relative to process start
  - frame duration
  - node count
- Left-clicking a frame selects it and updates the right pane.

### Thumbnail Navigation

- If a thread has more frames than can fit horizontally, a thumbnail histogram appears above that thread's frame histogram.
- Each thumbnail bucket represents a range of frames.
- Dragging/clicking on the thumbnail changes the visible frame window for all threads, keeping the timelines aligned by frame index.
- Thumbnail tooltips show:
  - represented frame id range
  - represented time range
  - maximum frame cost in that bucket

### Right Pane: Frame Detail Tree

- Shows the selected frame as a call tree.
- Each node displays:
  - percentage of parent time
  - total time
  - function/profile scope name
- Node text is color coded by relative cost:
  - red for nodes above 50% of parent time
  - yellow for nodes above 30%
  - white for lower-cost nodes
- Hovering a node shows:
  - percentage of the whole frame
  - percentage of the parent
  - total time
  - self time and self percentage
  - start and end time relative to the selected frame
  - node name

### Combined Mode

- If the selected frame contains repeated call paths that can be combined, a `Combined` checkbox is shown.
- Combined mode groups matching nodes by name/path.
- Combined nodes show:
  - call count
  - total combined time
  - percentage of parent time
  - node name
- Combined tooltips show:
  - call count
  - frame and parent percentages
  - total time
  - self time
  - average time and average percentages when the call count is greater than one

## Timeline View

The `Timeline` view is a Chrome Performance-style time-based view over the loaded capture.

### Global Time Ruler

- Displays a time ruler across the top of the timeline.
- Tick spacing adapts to the current zoom level.
- Time labels use seconds, milliseconds, microseconds, or nanoseconds depending on scale.
- Vertical grid lines align all threads to the same process-relative time axis.

### Thread Tracks

- Each recorded thread is shown as a separate track.
- Thread headers display:
  - thread id
  - thread name when available
- Each thread always includes a `Frames` row.
- Frame bars are positioned by real start/end timestamps, not by frame index.
- Frame bars show `F<frameId>` when there is enough horizontal space.
- Hovering a frame bar shows:
  - thread id and name
  - frame id
  - start time
  - end time
  - frame duration
  - node count
- Left-clicking a frame selects it and updates the shared frame selection state.

### Expanded Node Tracks

- The arrow button on each thread expands or collapses that thread.
- When expanded, the thread shows one row per call depth:
  - `Depth 0`
  - `Depth 1`
  - ...
- Profile nodes are drawn on their depth rows using real begin/end timestamps.
- Node colors are stable per profile scope name.
- Node labels are shown when the bar is wide enough.
- Hovering a node shows:
  - profile scope name
  - frame id
  - start and end time relative to the frame
  - node duration

### Timeline Navigation

- `Ctrl + mouse wheel` zooms in or out around the mouse position.
- Middle-button drag pans horizontally.
- Right-button drag also pans horizontally.
- Horizontal mouse wheel pans the visible time range.
- The visible range is clamped to the capture's process start/end time.
- The timeline keeps a minimum zoom span so the view remains usable on very large captures.

## Rendering and UI Stack

- Uses Dear ImGui for all UI.
- Uses ImGui docking for the main layout.
- Uses GLFW for the window, input, and document-open callback.
- Uses OpenGL2 through `imgui_impl_opengl2`.
- Uses the viewer's local ImGui helpers in `viewer/imgui` for histogram plotting with hover and selection hit testing.

## Build

From the repository root, the CMake viewer build command is:

```bash
bash build.sh cmake_viewer
```

The resulting executable is copied to:

```text
out/xxprofile_viewer_cmake
```

