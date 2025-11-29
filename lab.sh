#!/bin/bash
# lab.sh - The GraphicsLab Workbench Orchestrator

# 1. Go to the project directory
PROJECT_DIR="$HOME/dev/projects/GraphicsLab"
cd "$PROJECT_DIR"

# 2. Launch The "Visualizer" (Runner) - The Master Window
# We give it a specific class name: 'gl_runner'
alacritty --class gl_runner --title "GL_Runner" --working-directory "$PROJECT_DIR" -e bash -c "echo 'Ready to build...'; exec bash" &
sleep 0.2

# 3. Launch Neovim - The Editor
# Class name: 'gl_editor'
alacritty --class gl_editor --title "GL_Editor" --working-directory "$PROJECT_DIR" -e nvim src/scenes/Scene02_Input.h &
sleep 0.2

# 4. Launch Yazi - The File Manager
# Class name: 'gl_yazi'
alacritty --class gl_yazi --title "GL_Yazi" --working-directory "$PROJECT_DIR" -e yazi &

# 5. Tell Hyprland to arrange them (Optional, but ensures focus)
# You usually rely on window rules for this, but we can force focus to nvim
sleep 0.5
hyprctl dispatch focuswindow title:GL_Editor
