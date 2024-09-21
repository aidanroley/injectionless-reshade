# (WIP) DirectX11 Screen Capture and Shader Application

## Overview
This project captures the entire monitor screen using the **DirectX11 Desktop Duplication API**. It then processes the captured screen data through a **shader** using DirectX11 rendering pipelines and outputs the result in a separate window.

## How It Works
The program captures the entire monitor screen in real-time using the Desktop Duplication API and sends the captured data to a DirectX11 graphics pipeline. Custom shaders can then applied and final processed image is rendered and displayed in a separate output window.

## Input Limitations
The entire reason I made this program was to apply custom shaders to games like Destiny 2 without the risk of getting banned, so this program is not an overlay or transparent in any way. Therefore, it is best suited for setups with **at least 2 monitors** best mouse-clicks cannot be passed through the window without breaking anti-cheat rules.

### TODO
- Add functionality to choose the monitor input if multiple monitors are available.
- Fix bug where if resolution is too big it cascades
- Make it more user friendly

