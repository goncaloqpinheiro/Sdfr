Assignment 6 submission - top-level README(Definetely)
=========================================

This submission contains the following Assignment 6 packages created or extended for the
green-ball tracking workflow:

- ball_detector
- relbot_sequence_controller
- relbot_launch

Provided packages are NOT included in this submission zip. The following provided packages
must already be available in the ROS2 workspace:

- cam2image_vm2ros
- relbot_adapter
- relbot_simulator
- relbot2turtlesim
- relbot_msgs
- turtlesim

For the Raspberry Pi / physical RELbot runs, the following provided software is also needed:

- ros_xeno_bridge
- xrf2_msgs
- the provided Xenomai loop-controller demo on the RELbot Raspberry Pi

All commands below are intended to be run from the base of the ROS2 workspace after the
workspace has been built and sourced.


Assignment 6.2.2 - Detect a green object
----------------------------------------

Run the detector on the simulator moving-camera image:

ros2 run ball_detector detector_node --ros-args -p image_topic:=/output/moving_camera

If testing directly on the camera stream used for the hardware workflow:

ros2 run ball_detector detector_node --ros-args -p image_topic:=/image

Location in code:
- ball_detector/src/ball_detector.cpp


Assignment 6.3 - Closed-loop tracking in simulation
---------------------------------------------------

Run the complete simulation setup:

ros2 launch relbot_launch full_tracking_system.launch.py

This launch file starts:
- cam2image_vm2ros/cam2image
- ball_detector
- relbot_adapter in sim mode
- relbot_simulator
- relbot2turtlesim
- relbot_sequence_controller
- turtlesim

Locations in code:
- relbot_launch/launch/full_tracking_system.launch.py
- ball_detector/src/ball_detector.cpp
- relbot_sequence_controller/src/relbot_sequence_controller.cpp


Assignment 6.4.1 - Controller stack on the Raspberry Pi with simulator
----------------------------------------------------------------------

On the Raspberry Pi, run the same simulation launch file:

ros2 launch relbot_launch full_tracking_system.launch.py

Locations in code:
- relbot_launch/launch/full_tracking_system.launch.py
- ball_detector/src/ball_detector.cpp
- relbot_sequence_controller/src/relbot_sequence_controller.cpp


Assignment 6.4.2 and 6.5 - Physical RELbot workflow used in the lab
-------------------------------------------------------------------

The combined hardware launch file is included:

ros2 launch relbot_launch lab_hardware.launch.py

During the lab this combined launch file did not reliably start all nodes on the Raspberry Pi.
For the final demonstration, the system was therefore started manually in separate terminals as
follows.

Terminal 1 - provided FRT loop controller on the RELbot Raspberry Pi:

sudo ./build/demo/demo

Terminal 2 - adapter:

ros2 run relbot_adapter relbot_adapter --ros-args -p robotmode:="real" -p use_twist_cmd:=false -p output_rate_hz:=50.0 -p command_timeout_sec:=0.30 -p max_speed_mps:=0.25 -p max_speed_rads:=5.0

Terminal 3 - provided ROS/Xenomai bridge:

ros2 run ros_xeno_bridge RosXenoBridge

Terminal 4 - camera:

ros2 run cam2image_vm2ros cam2image --ros-args --params-file src/cam2image_vm2ros/config/cam2image.yaml

Terminal 5 - ball detector on the camera stream:

ros2 run ball_detector detector_node --ros-args -p image_topic:=/image

Terminal 6 - sequence controller on the camera stream:

ros2 run relbot_sequence_controller relbot_sequence_controller --ros-args -p image_topic:=/image -p target_ball_area:=9000.0 -p center_deadband_px:=4.0 -p ball_timeout:=0.35

After all nodes are running, initialise and start the controller:

ros2 topic pub --once /XenoCmd std_msgs/msg/Int32 "{data: 1}"
ros2 topic pub --once /XenoCmd std_msgs/msg/Int32 "{data: 2}"

Locations in code:
- relbot_launch/launch/lab_hardware.launch.py
- ball_detector/src/ball_detector.cpp
- relbot_sequence_controller/src/relbot_sequence_controller.cpp

