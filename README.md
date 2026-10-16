# 3D Software Renderer

A CPU-based software renderer for animated 3D objects.

## Requirements

- C++17-compatible compiler
- SDL2 development libraries
- JsonCpp development libraries
- GNU Make

## Building

Build the project:

```sh
make
```

Rebuild or clean:

```sh
make rebuild
make clean
```

### Optional build parameters

| Parameter | Description |
|---|---|
| `DEBUG=1` | Build in debug mode (GDB). |
| `DBG_ASAN=1` | Enable address sanitizing. |
| `DBG_PRINTS=1` | Enable debug prints. |
| `DBG_POLY=1` | Display polygon meshes and their normals for debugging. |

Pass an option to `make`, for example `make DEBUG=1`.

## Running the engine

```sh
./engine_3d --file ../config/config.json --conf config
./engine_3d --help
```

## Concepts

### Coordinate system

| Axis | Positive | Negative | Zero |
|---|---|---|---|
| X | Right | Left | Screen center |
| Y | Up | Down | Screen center |
| Z | Toward the viewer (outside) | Away from the viewer (inside) | Screen plane |

The camera sits outside the screen plane and looks inward, like the viewer.
The light source also sits outside the screen plane, higher up, illuminating inward.

- Polygons facing the camera are visible; polygons facing away are culled.
- **Focal length** controls perspective projection and magnification.
- Objects closer to the camera appear larger; objects farther from the camera appear smaller.
- Objects behind the camera are not displayed.
- **Epsilon** determines near-plane/depth tolerance.

### Scene hierarchy

Consider an animated airplane scene:

- A movable **trim tab** belongs to and moves with an aileron or elevator.
- An **aileron** and a **flap** belong to and move with a wing.
- A stationary **wing** belongs to and moves with the airplane.
- An **elevator** belongs to and moves with a horizontal stabilizer.
- A stationary **horizontal stabilizer** belongs to and moves with the airplane.
- The movable **airplane** belongs to and moves with a gimbal.
- The movable **gimbal** belongs to the **World**.
- The runway and background may also belong to the World.
- The **World** is the root of the hierarchy.

A child inherits its parent's transformations while retaining its own local transformations.

#### Example hierarchy tree

```text
World
|----Gimbal
|    |----Airplane
|    |    |----Right Wing
|    |    |    |----Aileron
|    |    |    |    \----Trim
|    |    |    \----Flap
|    |    |----Left Wing
|    |    |    |----Aileron
|    |    |    |    \----Trim
|    |    |    \----Flap
|    |    |----Right Horizontal Stabilizer
|    |    |    |----Elevator
|    |    |    \    \----Trim
|    |    |----Left Horizontal Stabilizer
|    |    |    |----Elevator
|    |    |    \    \----Trim
|    |    |----Vertical Stabilizer
|    |    |    |----Rudder
|    |    |    \    \----Trim
|    |    |----Propeller
|    |    |    |    |----Blade 1
|    |    |    |    |----Blade 2
|    |    |    |    |----Blade 3
|    |    |    |    \----Blade 4
|    |    |    |----Right Landing Gear
|    |    |    |    \----Wheel
|    |    |    |----Left Landing Gear
|    |    |    |    \----Wheel
|    |    |    |----Front Landing Gear
|    |    \    \    \----Wheel
|    |----Land
|    |    |----Background
|    |    |    |----Control Tower
|    |    |    |----Buildings
|    |    |    \----Mountains
\    \    \----Landing Strip
```

## JSON configuration

The scene hierarchy is described in a JSON configuration file.

**Parsing** verifies JSON syntax, field types, and fixed-length arrays.
**Validation** checks duplicates, completeness, value constraints, references, and scene hierarchy.

### Version

`version` is **mandatory** and contains exactly three unsigned integers. The currently supported version is **`[0, 0, 3]`**.

### Library

The library contains definitions of **polygons**, **meshes**, and **elements**, with at least one of each type in a valid configuration.

- Each element references one valid 3D mesh.
- Each mesh references at least one valid polygon.
- Each polygon has **3–8 consecutive vertices** describing a **planar, closed shape**.
- Vertex winding order (**CW/CCW**) determines which side of a polygon is visible.
- Multiple elements can reuse the same mesh.
- Object and field order is not significant.
- Field names must be lowercase.
- Duplicate object names are not allowed.

#### `library.polygons`

| Field | JSON type | Description |
|---|---|---|
| `name` | String | Non-null, nonempty object identifier. |
| `force` | Boolean | Render the polygon regardless of its facing direction. |
| `color_idx` | Unsigned integer | Color palette index (see below). |
| `vectors` | Array | At least three vectors defining the polygon. |

Each vector is an unnamed array containing **exactly three floating-point coordinates**: `[x, y, z]`.

#### `library.meshes`

| Field | JSON type | Description |
|---|---|---|
| `name` | String | Non-null, nonempty object identifier. |
| `polygons` | Array of strings | Names of at least one valid polygon. |

#### `library.elements`

| Field | JSON type | Description |
|---|---|---|
| `name` | String | Non-null, nonempty object identifier. |
| `mesh` | String | Name of exactly one valid mesh. |

#### Color palette

| Index | Color | Index | Color |
|---:|---|---:|---|
| 0 | Black | 8 | Lime |
| 1 | Grey | 9 | Green |
| 2 | Silver | 10 | Aqua |
| 3 | White | 11 | Teal |
| 4 | Red | 12 | Blue |
| 5 | Maroon | 13 | Navy |
| 6 | Yellow | 14 | Magenta |
| 7 | Olive | 15 | Purple |

### Scenes

`scenes` is an array containing **at least one scene** in a valid configuration.

- Each scene contains a list of elements and optional environment settings.
- The elements form a hierarchy tree with **exactly one root element** and at least one child element.
- Multiple children may share a parent (siblings).
- The root is identified by a `null` parent.
- Child elements may reference a library element, and multiple scene elements may reuse the same library definition.
- Scene and element ordering is not significant.
- Field names must be lowercase.
- Duplicate object names are not allowed.

#### Scene elements

| Field | JSON type | Description |
|---|---|---|
| `name` | String | Non-null, nonempty scene-element identifier. |
| `parent` | String or null | Name of a valid parent; `null` identifies the root. |
| `active` | Boolean | Enables or disables the scene element. |
| `ref` | String | Optional name of a valid library element. |
| `origin` | Attributes | Optional initial position and orientation. (see below) |
| `motion` | Attributes | Optional continuous movement. (see below) |

##### Attributes

Each attribute is an array containing **exactly seven floating-point values**:
Attribute fields:

```text
[ X Rotation, Y Rotation, Z Rotation, X Translate, Y Translate, Z Translate, Scale factor ]
```

The scale factor is `1.0` for normal size, `0.5` for half size, and `2.0` for twice the size.

#### Environment

`environment` is an **optional JSON object**. All its fields are optional overrides.

| Field | JSON type | Description |
|---|---|---|
| `light_type` | Unsigned integer | `0`: directional (sunlight); `1`: point (spotlight). |
| `light_direction` | Vector | Direction of the light. |
| `light_position` | Vector | Position of the light. |
| `camera_position` | Vector | Position of the camera. |
| `focal_len` | Floating-point number | Perspective projection and magnification. |
| `near_eps` | Floating-point number | Near-plane/depth tolerance. |
| `min_pos`, `max_pos` | Array | Each contains exactly two unsigned integers defining the screen viewport. |
| `color_idx` | Unsigned integer | Background color index. |
| `loops` | Integer | Number of loops; `-1` means endless. |
| `loop_delay` | Unsigned integer | Delay between frames, in milliseconds. |

**Maximum supported viewport dimensions:** 1280 × 800 (16:10).

## JSON configuration example

See [`config/template.json`](config/template.json).

```json
{
	"encoding": "UTF-8",
	"indent":
	{
		"length": 4,
		"use_space": false
	},

	"version": [ 0, 0, 3 ],

	"library":
	{
		"polygons": [
			{
				"name": "c_front",
				"color_idx": 4,
				"force": false,
				"vectors": [
					[ 100.0, 100.0, -100.0 ],
					[ 100.0, -100.0, -100.0 ],
					[ -100.0, -100.0, -100.0 ],
					[ -100.0, 100.0, -100.0 ]
				]
			}
		],
		"meshes": [
			{
				"name": "cube_mesh",
				"polygons": [ "c_front" ]
			}
		],
		"elements": [
			{
				"name": "cube_body",
				"mesh": "cube_mesh"
			}
		]
	},
	"scenes": [
		{
			"name": "cube",
			"elements": [
				{
					"name": "world",
					"parent": null,
					"active": true
				},
				{
					"name": "cube_1",
					"parent": "world",
					"active": true,
					"ref": "cube_body",
					"origin": [ 0, 0, 0, 0, 0, 0, 1 ],
					"motion": [ 0, 0, 0, 0, 0, 0, 1 ]
				}
			],
			"environment": {
				"color_idx": 0,
				"light_type": 0,
				"light_direction": [ 0.0, 0.0, 1.0 ],
				"light_position": [ 0.0, 20.0, 75.0 ],
				"camera_position": [ 0.0, 0.0, 1000.0 ],
				"focal_len": 2000.0,
				"near_eps": 1.0,
				"min_pos": [ 0, 0 ],
				"max_pos": [ 1280, 800 ],
				"loops": -1,
				"loop_delay": 20
			}
		}
	]
}
```
