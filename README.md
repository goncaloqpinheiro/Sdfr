# Green-Ball Tracking RELbot (Software Development for Robotics)

This repository contains a ROS 2 green-ball tracking system for the **RELbot** differential-drive
robot, developed as **Assignment 6** of the *Software Development for Robotics* (SDfR) course in the
MSc Robotics programme at the **University of Twente**.

The robot detects a green ball in its camera stream, steers to keep the ball centred, drives forward
until the ball is close enough (estimated from the blob area), and rotates to search for the ball when
it is lost. The same controller stack runs both in **simulation** (RELbot simulator + turtlesim) and on
the **physical RELbot** over the Raspberry Pi / Xenomai bridge.

## Key Features

- **HSV green-ball detector** built with OpenCV (`cv_bridge` + thresholding + morphological cleanup +
  image moments) publishing the ball centroid and blob area.
- **Closed-loop tracking controller** with proportional steering, area-based forward speed, a centre
  deadband to reduce oscillation, and a timeout-triggered **search mode**.
- **Single codebase for sim and hardware** — the same nodes run against the RELbot simulator or the
  real robot, selected through launch arguments and image topics.
- **Ready-to-use launch configurations**:
  * `full_tracking_system.launch.py` — complete closed-loop simulation stack (Assignment 6.3 / 6.4.1)
  * `lab_hardware.launch.py` — ROS-side nodes for the physical RELbot workflow (Assignment 6.4.2 / 6.5)
  * `relbot_sequence_controller.launch.py` — controller + simulator-side support nodes
  * plus the provided `relbot_system`, `relbot_system_joystick`, and `relbot_to_turtle` launch files
- **Group 17 report** included under [`docs/`](docs/) documenting the full design and lab results.

## Project Context

Developed for educational purposes in robot software architecture and vision-based control. The system
demonstrates a complete perception → control → actuation pipeline built from independent ROS 2 nodes:

```
 camera (cam2image_vm2ros)
        │  sensor_msgs/Image      (/output/moving_camera  in sim,  /image  on hardware)
        ▼
 ball_detector                    →  HSV threshold + moments
        │  geometry_msgs/Point    (/ball_position :  x,y = centroid px,  z = blob area px)
        ▼
 relbot_sequence_controller       →  steering + forward speed + search behaviour
        │  example_interfaces/Float64   (/input/left_motor/setpoint_vel, /input/right_motor/setpoint_vel)
        ▼
 relbot_adapter  ──►  relbot_simulator ─► relbot2turtlesim ─► turtlesim   (simulation)
                 └─►  ros_xeno_bridge  ─► Xenomai loop controller          (physical RELbot)
```

The detector reports the ball centroid in pixels plus the blob area, which the controller uses as a
distance proxy: as the ball gets closer the area grows, the forward command shrinks, and the robot
stops once a target area is reached.

<!-- Add a demo screenshot or GIF here, e.g.:
![Tracking demo](docs/demo.gif)
-->

## Repository Structure

```
Sdfr/
├── src/
│   ├── ball_detector/                # green-ball detector node
│   ├── relbot_sequence_controller/   # closed-loop tracking controller
│   └── relbot_launch/                # launch files for sim and hardware
├── docs/
│   ├── SDfR_Assignment6_Report_Group17.pdf
│   └── original_submission_README.txt
├── LICENSE
└── README.md
```

> **Note on dependencies.** Only the packages **created/extended for this assignment** are included
> here. The following *provided* packages must already be present in your ROS 2 workspace before
> building: `cam2image_vm2ros`, `relbot_adapter`, `relbot_simulator`, `relbot2turtlesim`,
> `relbot_msgs`, `turtlesim`. For the physical RELbot you also need `ros_xeno_bridge`, `xrf2_msgs`,
> and the provided Xenomai loop-controller demo running on the RELbot Raspberry Pi.

## Build

Clone into the `src/` of a ROS 2 workspace (or use this repo as the workspace root), then build and
source:

```bash
# from the workspace root
colcon build --packages-select ball_detector relbot_sequence_controller relbot_launch
source install/setup.bash
```

## Run

### Detect a green object (Assignment 6.2.2)

```bash
# against the simulator moving-camera image
ros2 run ball_detector detector_node --ros-args -p image_topic:=/output/moving_camera

# against the direct camera stream (hardware workflow)
ros2 run ball_detector detector_node --ros-args -p image_topic:=/image
```

### Closed-loop tracking in simulation (Assignment 6.3 / 6.4.1)

```bash
ros2 launch relbot_launch full_tracking_system.launch.py
```

This starts `cam2image`, `ball_detector`, `relbot_adapter` (sim mode), `relbot_simulator`,
`relbot2turtlesim`, `relbot_sequence_controller`, and `turtlesim`.

### Physical RELbot (Assignment 6.4.2 / 6.5)

A combined hardware launch file is provided:

```bash
ros2 launch relbot_launch lab_hardware.launch.py
```

During the lab this combined launch did not reliably start every node on the Raspberry Pi, so the
final demonstration was run manually across separate terminals (FRT loop controller → adapter →
ros_xeno_bridge → camera → detector → sequence controller), then initialised with:

```bash
ros2 topic pub --once /XenoCmd std_msgs/msg/Int32 "{data: 1}"
ros2 topic pub --once /XenoCmd std_msgs/msg/Int32 "{data: 2}"
```

See [`docs/original_submission_README.txt`](docs/original_submission_README.txt) for the exact
per-terminal commands used in the lab.

## Packages

| Package | Description |
| --- | --- |
| **ball_detector** | Subscribes to an image topic, thresholds green pixels in HSV, cleans the mask with morphological operations, and publishes the ball centroid and blob area on `/ball_position`. |
| **relbot_sequence_controller** | Consumes `/ball_position`, computes steering from the horizontal error and forward speed from the blob area, applies a centre deadband, and searches in place when no recent detection arrives. Publishes left/right wheel setpoints. |
| **relbot_launch** | Launch files tying the detector, controller, adapter, and (in simulation) the RELbot simulator + turtlesim together for each assignment workflow. |

Each package also contains a `README.txt` with its inputs, outputs, parameters, and core components.

## Key Parameters

**ball_detector** — `image_topic` (default `/image`), `point_topic` (default `/ball_position`). HSV
thresholds and minimum blob area are compile-time constants in `ball_detector.cpp`.

**relbot_sequence_controller** — `image_topic`, `target_ball_area` (stop distance proxy, default
`9000.0`), `center_deadband_px` (default `4.0`), `ball_timeout` (search trigger, default `0.25 s`),
`sim_sign_fix` (wheel-sign handling for simulation).

## Authors

- **Gonçalo Pinheiro** (s3741680)
- **Ceyhun Ceylan** (s3514609)

Group 17 — MSc Robotics, University of Twente, April 2026.

## License

Released under the Apache License 2.0. See [`LICENSE`](LICENSE).
