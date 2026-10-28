# JSON Configuration

**Parsing** verifies JSON syntax, field types, and fixed-length arrays.\
**Validation** checks duplicates, completeness, value constraints, references, and scene hierarchy.\
**valid values:** See [`inc/constants.h`](./inc/constants.h).

## Sections

### Version

`version` is **mandatory** and contains exactly three unsigned integers. The currently supported version is **`[0, 0, 3]`**.

### Library

The library contains definitions of **polygons**, **meshes**, and **elements**, with at least one of each type in a valid configuration.

- Each element references one valid 3D mesh.
- Each mesh references at least one valid polygon.
- Each polygon has **3–8 consecutive vectors** describing a **planar, closed shape**.
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
| `vectors` | Array | At least three vectors defining a valid polygon. |

**Vector:** is an unnamed array containing **exactly three floating-point coordinates**: `[x, y, z]`.\
**Coordinate valid values:** [ -1000.0 - 1000.0 ].

#### `library.meshes`

| Field | JSON type | Description |
|---|---|---|
| `name` | String | Non-null, nonempty object identifier. |
| `polygons` | Array of strings | Nonempty names of at least one valid polygon. |

#### `library.elements`

| Field | JSON type | Description |
|---|---|---|
| `name` | String | Non-null, nonempty object identifier. |
| `mesh` | String | Non-null, nonempty name of exactly one valid mesh. |

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

- Each scene contains a list of elements and optional environment and runtime settings.
- The elements form a hierarchy tree with **exactly one root element** and **at least one other child element**.
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
| `parent` | String or null | Nonempty name of a valid parent; `null` identifies the root. |
| `active` | Boolean | Enables or disables the scene element. |
| `ref` | String | Optional Non-null, nonempty name of a valid library element. |
| `origin` | Array | Optional initial position and orientation attributes. (see below) |
| `motion` | Array | Optional continuous movement attributes. (see below) |

##### Attributes

Each attribute is an array containing **exactly seven floating-point values**:
Attribute fields:

```text
[ X Rotation, Y Rotation, Z Rotation, X Translation, Y Translation, Z Translation, Scale factor ]
```

The scale factor is `1.0` for normal size, `0.5` for half size, and `2.0` for twice the size.

**Rotation/Translation valid values:** `[ -1000.0 - 1000.0 ]`.\
**Scale values:** `[ 0.1 - 10.0 ]`.
###### Default: `[ 0, 0, 0, 0, 0, 0, 1 ]`.

#### Environment

`environment` is an **optional JSON object** setting the graphical properties.\
All its fields are optional overrides.

| Field | JSON type | Description |
|---|---|---|
| `resolution` | Array | Contains exactly two unsigned integers defining the window size. |
| `color_idx` | Unsigned integer | Background color index (see above). |
| `light_type` | Unsigned integer | `0`: directional (sunlight); `1`: point light (default: `0`). |
| `light_direction` | Vector | Direction of the light source (default: `[ 0, 0, 1 ]`). |
| `light_position` | Vector | Position of the light source (default: `[ 0, 20, 75 ]`). |
| `camera_position` | Vector | Position of the camera (default: `[ 0, 0, 1000 ]`). |
| `focal_len` | Floating-point number | Perspective projection and magnification (default: `2000`). |
| `near_eps` | Floating-point number | Near-plane/depth tolerance (default: `1`). |

**Minimum supported resolution:** `320 × 200` (16:10).\
**Maximum supported resolution:** `1600 × 1000` (16:10).\
**Default resolution:** `1280 × 800` (16:10).\
**Vectors:** see polygon vectors above.\
**Focal length valid range:** `[ 100.0 - 3000.0 ]`.\
**Near epsilon valid range:** `[ 0.5 - 3.0 ]`.

#### Runtime

`runtime` is an **optional JSON object** setting the runtime properties.\
All its fields are optional overrides.

| Field | JSON type | Description |
|---|---|---|
| `loops` | Integer | Number of loops; `-1` means endless (default: `-1`).|
| `loop_delay` | Unsigned integer | Delay between frames, in milliseconds (default: `20`).|

**Loops valid range:** `[ -1 - 100 ]`.\
**Loop delay valid range:** `[ 1 - 200 ]`.

## JSON configuration example

See [`config/template.json`](./config/template.json).

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
					"origin": [ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0 ],
					"motion": [ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0 ]
				}
			],
			"environment": {
				"resolution": [ 1280, 800 ],
				"color_idx": 0,
				"light_type": 0,
				"light_direction": [ 0.0, 0.0, 1.0 ],
				"light_position": [ 0.0, 20.0, 75.0 ],
				"camera_position": [ 0.0, 0.0, 1000.0 ],
				"focal_len": 2000.0,
				"near_eps": 1.0,
			},
			"runtime": {
				"loops": -1,
				"loop_delay": 20
			}
		}
	]
}
```
