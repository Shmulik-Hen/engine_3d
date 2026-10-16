# JSON Configuration

**Parsing** verifies JSON syntax, field types, and fixed-length arrays.
**Validation** checks duplicates, completeness, value constraints, references, and scene hierarchy.

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
| `origin` | Array | Optional initial position and orientation attributes. (see below) |
| `motion` | Array | Optional continuous movement attributes. (see below) |

##### Attributes

Each attribute is an array containing **exactly seven floating-point values**:
Attribute fields:

```text
[ X Rotation, Y Rotation, Z Rotation,
  X Translation, Y Translation, Z Translation,
  Scale factor ]
```

The scale factor is `1.0` for normal size, `0.5` for half size, and `2.0` for twice the size.

#### Environment

`environment` is an **optional JSON object**. All its fields are optional overrides.

| Field | JSON type | Description |
|---|---|---|
| `light_type` | Unsigned integer | `0`: directional (sunlight); `1`: point light. |
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
