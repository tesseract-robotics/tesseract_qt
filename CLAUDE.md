# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Overview

Qt5-based GUI framework for the Tesseract motion planning framework. Provides 3D visualization, interactive robot manipulation, trajectory playback, and a dockable plugin-based studio application.

## Architecture

Plugin-based Qt5 studio application. Components communicate via a Qt event system. Studio dock widgets are loaded as plugins via `boost_plugin_loader`.

### Modules

- **common** — Event system, entity management, standard item model types
- **rendering** — 3D visualization via Gazebo/Ignition (OGRE2), `RenderWidget`, camera control, ray-cast picking
- **plot** — Time-series plotting using Qwt
- **studio** — Main `Studio` application using Qt Advanced Docking System
- **scene_graph/environment/collision/acm** — Scene graph, environment, collision, and ACM editing UIs
- **joint_state_slider/joint_trajectory/tool_path/manipulation/planning** — Robot interaction and planning UIs
- **workbench** — Integrated workspace combining the above widgets

### Dependencies

Qt5 (Core, Gui, Widgets, OpenGL, Xml, Svg), Qt Advanced Docking System, Qwt, Gazebo rendering libraries (gz-common5, gz-rendering8, gz-transport11, gz-msgs8, gz-math7). Rendering and Studio are optionally built (`BUILD_RENDERING`, `BUILD_STUDIO` CMake options).
