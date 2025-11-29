cat <<EOF > README.md
# GraphicsLab 🖥️

A modular C++ graphics sandbox built with **OpenGL 3.3**, **GLFW**, and **GLEW**. 

This project serves as a workbench for experimenting with computer graphics concepts, rendering pipelines, and game engine architecture on Linux.

![Demo](screenshots/demo.png)
*(Note: Screenshot will appear here once added to the screenshots/ folder)*

## ✨ Architecture
The project utilizes a polymorphic **Scene System**, allowing for hot-swapping between different rendering logic without restarting the application.

*   **Scene.h:** Abstract base class defining the lifecycle (\`OnAttach\`, \`OnUpdate\`, \`OnRender\`).
*   **Scene01:** Dynamic Color Buffering (Testing basic GL clear states).
*   **Scene02:** 2D Kinematics (WASD Input handling).
*   **Scene03:** Vector Math (Mouse tracking and coordinate mapping).

## 🛠️ Technology Stack
*   **Language:** C++17
*   **Graphics API:** OpenGL 3.3 (Core Profile)
*   **Windowing:** GLFW
*   **Extension Loader:** GLEW
*   **Build System:** CMake
*   **OS:** Arch Linux (Wayland/Hyprland)

## 🚀 Building & Running

### Dependencies
**Arch Linux:**
```bash
sudo pacman -S base-devel cmake glfw-wayland glew
```

**Debian/Ubuntu:**
```bash
sudo apt install build-essential cmake libglfw3-dev libglew-dev
```

### Compile and Run
I have included a helper script \`run.sh\` to automate the CMake build process.

```bash
# 1. Clone the repository
git clone git@github.com:YOUR_USERNAME/GraphicsLab.git

# 2. Enter directory
cd GraphicsLab

# 3. Build and Launch
./run.sh
```

## 🎮 Controls

| Input | Action |
| :--- | :--- |
| **1, 2, 3** | Switch active Scenes |
| **W, A, S, D** | Move Player (Scene 2) |
| **Mouse** | Control Cursor Target (Scene 3) |
| **ESC** | Close Application |

## 💻 Development Workflow (Hyprland)
This project includes a custom orchestration script, \`lab.sh\`, designed for **Hyprland** users. It configures the workspace layout automatically:

1.  **Neovim:** Opens source code (Master Window).
2.  **Alacritty (Runner):** Compiles code (Top-Right).
3.  **Yazi:** File management (Bottom-Right).

To start the dev environment:
```bash
./lab.sh
```

---
*Created by Hugo Schenegg*
EOF
