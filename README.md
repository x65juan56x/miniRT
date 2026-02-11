# 🌟 miniRT

<p align="center">
  <img src="https://img.shields.io/badge/Language-C-blue?style=for-the-badge&logo=c" alt="C"/>
  <img src="https://img.shields.io/badge/Graphics-MLX42-orange?style=for-the-badge" alt="MLX42"/>
  <img src="https://img.shields.io/badge/42-Project-green?style=for-the-badge" alt="42 Project"/>
</p>

<p align="center">
  <strong>A minimal ray tracing engine written in C</strong>
</p>

<p align="center">
  <em>Part of the 42 Common Core curriculum - Rank 04</em>
</p>

---

## 📖 About

**miniRT** is a ray tracing engine built from scratch in C. This project introduces the fundamentals of computer graphics by implementing a realistic rendering engine capable of generating images through ray tracing techniques. The engine parses scene description files (`.rt`) and renders 3D scenes with lighting, shadows, and various geometric primitives.

Ray tracing simulates the way light interacts with objects by tracing rays from a virtual camera through each pixel of the image, calculating intersections with objects, and determining the final color based on material properties and lighting.

---

## ✨ Features

### Core Features (Mandatory)
- **Geometric Primitives:**
  - 🔴 **Spheres** - Perfectly round objects with configurable size and position
  - 📦 **Planes** - Infinite flat surfaces with customizable orientation
  - 🛢️ **Cylinders** - Finite cylinders with configurable height and diameter

- **Lighting System:**
  - 💡 Point light sources with adjustable brightness
  - 🌅 Ambient lighting for global illumination
  - 🌑 Hard shadow computation for realistic shadows

- **Camera System:**
  - 📷 Configurable camera position, direction, and field of view (FOV)
  - Full viewport calculation for perspective projection

- **Shading:**
  - Lambert (diffuse) shading model
  - RGB color support with proper light color mixing

### Bonus Features
- **Additional Primitives:**
  - 🔺 **Triangles** - Using Möller–Trumbore intersection algorithm
  - 🌊 **Hyperbolic Paraboloids** - Saddle-shaped quadric surfaces

- **Advanced Lighting:**
  - 💡 Multiple point light sources (linked list implementation)
  - Colored light sources

- **Material System:**
  - ✨ **Specular Highlights** - Both Phong and Blinn-Phong reflection models
  - 🪞 **Reflections** - Recursive ray tracing with configurable depth limit
  - 🎨 **Checker Patterns** - Procedural texturing for all primitives

- **Bump Mapping:**
  - 🗺️ Load PNG images as height maps
  - Perturb surface normals for detailed surface appearance
  - Support for bump mapping on all object types

- **Debug Mode:**
  - 🎨 Normal visualization mode (toggle with `N` key)

---

## 🏗️ Architecture

```
miniRT/
├── include/              # Header files (mandatory)
│   ├── minirt.h          # Main includes and constants
│   ├── scene.h           # Scene data structures
│   ├── vec3.h            # 3D vector math
│   ├── ray.h             # Ray structure
│   ├── hit.h             # Intersection records
│   ├── camera.h          # Camera system
│   ├── render.h          # Rendering pipeline
│   ├── shading.h         # Lighting calculations
│   ├── parser.h          # Scene file parser
│   └── app.h             # Application state
│
├── include_bonus/        # Header files (bonus)
│   ├── material_bonus.h  # Material properties (specular, reflectivity)
│   ├── bump_bonus.h      # Bump mapping system
│   └── ...
│
├── src/                  # Source files (mandatory)
│   ├── minirt.c          # Entry point
│   ├── core/             # Ray-object intersection logic
│   ├── geom/             # Geometric primitives (sphere, plane, cylinder)
│   ├── math/             # Vector operations and utilities
│   ├── parse/            # Scene file parser
│   ├── render/           # Framebuffer and rendering loop
│   ├── shading/          # Lambert shading and shadows
│   ├── camera/           # Camera frame calculation
│   └── app/              # Window management and input
│
├── src_bonus/            # Source files (bonus)
│   ├── geom/             # + Triangles, Hyperbolic Paraboloids
│   ├── shading/          # + Specular (Phong/Blinn-Phong), bump mapping
│   └── ...
│
├── libraries/
│   ├── MLX42/            # Graphics library (OpenGL-based)
│   ├── libft/            # Custom C standard library
│   └── get_next_line/    # File reading utility
│
└── examples/
    ├── scenes/           # Example .rt scene files
    └── bump_maps/        # PNG textures for bump mapping
```

---

## 🔧 Technical Implementation

### Ray Tracing Pipeline

```
1. Camera Setup → Build camera frame from scene definition
                  ↓
2. Pixel Loop   → For each pixel (x, y) in the viewport:
                  ↓
3. Ray Generation → Calculate ray direction through pixel
                    ↓
4. Intersection   → Test ray against all objects in scene
                    ↓
5. Hit Record     → Store closest intersection data (t, point, normal)
                    ↓
6. Shading        → Calculate color using lighting model:
                    - Ambient component
                    - Diffuse (Lambert): max(0, N·L)
                    - Specular (Phong/Blinn): pow(spec_angle, shininess)
                    ↓
7. Shadow Ray     → Cast ray toward each light; skip diffuse if blocked
                    ↓
8. Reflection     → (Bonus) Recursively trace reflected rays
                    ↓
9. Framebuffer    → Store final pixel color (RGBA)
                    ↓
10. Display       → Upload framebuffer to window
```

### Mathematical Foundations

#### Vector Operations (`vec3.h`)
- Addition, subtraction, scalar multiplication
- Dot product (angle calculation, shading)
- Cross product (normals, tangent space)
- Normalization, length calculations

#### Ray-Sphere Intersection
Solves the quadratic equation:
```
|P - C|² = r²   where P = O + tD
```
Uses the discriminant to determine 0, 1, or 2 intersections.

#### Ray-Plane Intersection
```
t = (point - origin) · normal / (direction · normal)
```

#### Ray-Cylinder Intersection
Projects the ray onto the cylinder's axis plane, solving a 2D quadratic for the lateral surface, plus separate cap intersection tests.

#### Ray-Triangle Intersection (Möller–Trumbore)
Efficient algorithm using barycentric coordinates to test point-in-triangle without precomputing the plane equation.

### Shading Models

| Model | Formula | Use Case |
|-------|---------|----------|
| **Lambert** | `I = kd * (N · L)` | Matte, diffuse surfaces |
| **Phong** | `I = ks * (R · V)^n` | Shiny, specular highlights |
| **Blinn-Phong** | `I = ks * (N · H)^n` | Optimized specular (uses halfway vector) |

---

## 📄 Scene File Format (`.rt`)

Scene files use a simple text-based format:

### Environment Elements

```bash
# Ambient light: A <ratio> <R,G,B>
A 0.2 255,255,255

# Camera: C <pos> <direction> <FOV>
C 0,0,0 0,0,-1 70

# Light: L <pos> <brightness> [R,G,B]
L -10,10,10 0.7 255,255,255
```

### Geometric Objects

```bash
# Sphere: sp <center> <diameter> <R,G,B> [options]
sp 0,0,-5 2.0 255,0,0

# Plane: pl <point> <normal> <R,G,B> [options]
pl 0,-1,0 0,1,0 200,200,200

# Cylinder: cy <center> <axis> <diameter> <height> <R,G,B> [options]
cy 0,0,-5 0,1,0 1.0 2.0 0,255,0
```

### Bonus Elements

```bash
# Triangle: tr <vertex1> <vertex2> <vertex3> <R,G,B> [options]
tr -1,-1,0 1,-1,0 0,1,0 255,255,0

# Hyperbolic Paraboloid: hp <center> <axis> <rx> <ry> <height> <R,G,B> [options]
hp 0,0,-5 0,1,0 2.0 2.0 1.5 120,200,255

# Multiple lights
L 5,5,5 0.8 255,255,255
L -5,5,5 0.6 255,200,150

# Reflection depth limit
R 4
```

### Material Options

```bash
# Specular: ks <strength> <shininess> [phong|blinn]
sp 0,0,-5 2.0 255,255,255 ks 0.8 500 phong

# Checker pattern: cb <scale>
pl 0,-1,0 0,1,0 200,200,200 cb 1.0

# Bump mapping: bm <path> <strength>
sp 0,0,-5 2.0 255,255,255 bm textures/stone.png 0.001

# Reflectivity: kr <value>
sp 0,0,-5 2.0 200,200,200 kr 0.7
```

---

## 🚀 Getting Started

### Prerequisites

- **GCC** or compatible C compiler
- **CMake** (for MLX42)
- **GLFW3** library
- **Make**

On Ubuntu/Debian:
```bash
sudo apt-get update
sudo apt-get install build-essential cmake libglfw3-dev
```

On macOS (Homebrew):
```bash
brew install cmake glfw
```

### Installation

```bash
# Clone the repository
git clone https://github.com/YOUR_USERNAME/miniRT.git
cd miniRT

# Build mandatory version
make

# Build bonus version
make bonus
```

### Usage

```bash
# Run with a scene file
./miniRT examples/scenes/example1_centered_sphere.rt

# Bonus version
./miniRT_bonus examples/scenes/SHININGBALLS.rt
```

### Controls

| Key | Action |
|-----|--------|
| `ESC` | Close window |
| `N` | Toggle normal visualization mode |

---

## 🎨 Example Scenes

The project includes many example scenes demonstrating various features:

| Scene | Description |
|-------|-------------|
| `example1_centered_sphere.rt` | Basic sphere rendering |
| `example6_many_things.rt` | Multiple objects scene |
| `example13_checkers_all_bonus.rt` | Checker patterns on all primitives |
| `example15_bumpmap.rt` | Bump mapping demonstration |
| `SHININGBALLS.rt` | Grid of spheres showing specular variation |
| `reflective_rat.rt` | Complex mesh with reflections |
| `solar_system.rt` | Space-themed scene |

---

## 📚 What You Learn

This project teaches fundamental concepts in:

### Computer Graphics
- Ray tracing algorithms and implementation
- 3D geometry and spatial mathematics
- Light transport and shading models
- Texture mapping and procedural texturing

### Mathematics
- Vector algebra (dot product, cross product)
- Solving quadratic equations for intersections
- Matrix transformations and coordinate systems
- Barycentric coordinates

### Software Engineering
- Modular C programming with clear separation of concerns
- Memory management and resource cleanup
- File parsing and error handling
- Performance optimization techniques

### Rendering Concepts
- Camera models and projection
- Surface normals and tangent space
- Reflection and recursive algorithms
- Shadow computation

---

## 🔬 Optimization Techniques

The project implements several performance optimizations:

- **Compiler flags:** `-Ofast -O3 -ffast-math -march=native`
- **Precomputed values:** Radius², inverse matrices stored in object structures
- **Early ray termination:** Skip calculations for rays pointing away from objects
- **Efficient intersection tests:** Optimized quadratic solvers, Möller–Trumbore algorithm

---

## 👥 Authors

<table>
  <tr>
    <td align="center">
      <a href="https://github.com/BelGarrido">
        <img src="https://github.com/BelGarrido.png" width="100px;" alt="Ana Garrido"/><br />
        <sub><b>Ana Garrido</b></sub>
      </a><br />
      <sub>anagarri @ 42</sub>
    </td>
    <td align="center">
      <a href="https://github.com/x65juan56x">
        <img src="https://github.com/x65juan56x.png" width="100px;" alt="Juan Mondón"/><br />
        <sub><b>Juan Mondón</b></sub>
      </a><br />
      <sub>jmondon @ 42</sub>
    </td>
  </tr>
</table>

---

## 📜 License

This project is part of the 42 curriculum. Please refer to 42's academic integrity guidelines.

---

## 🙏 Acknowledgments

- **42 Network** for the project subject and learning environment
- **MLX42** library by Codam for the graphics backend
- [Ray Tracing in One Weekend](https://raytracing.github.io/) series for ray tracing fundamentals

---

<p align="center">
  <img src="https://img.shields.io/badge/Made%20with-❤️-red?style=flat-square" alt="Made with love"/>
  <img src="https://img.shields.io/badge/42-Málaga-purple?style=flat-square" alt="42 Málaga"/>
</p>
