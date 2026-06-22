Package relbot_launch
=====================

Description
-----------

This package contains launch files used for the Assignment 6 workflows.
For Assignment 6, the main custom launch files are:

- full_tracking_system.launch.py
- lab_hardware.launch.py
- a modified relbot_sequence_controller.launch.py

The package depends on provided packages such as relbot_adapter, relbot_simulator,
relbot2turtlesim, and cam2image_vm2ros.


Launch files
------------

full_tracking_system.launch.py
Starts the complete closed-loop simulation setup for Assignment 6.3.

lab_hardware.launch.py
Starts the hardware-facing ROS nodes for the physical RELbot workflow.
This file is included for completeness, but during the lab the final demonstration was run
with manual multi-terminal startup because the combined launch did not reliably start all
nodes on the Raspberry Pi.

relbot_sequence_controller.launch.py
Starts the sequence controller together with the simulator-side support nodes.


Run
---

Complete Assignment 6.3 simulation:

ros2 launch relbot_launch full_tracking_system.launch.py

Simulator-side controller stack:

ros2 launch relbot_launch relbot_sequence_controller.launch.py

Hardware-facing launch file:

ros2 launch relbot_launch lab_hardware.launch.py


Dependencies expected in the workspace
--------------------------------------

- cam2image_vm2ros
- relbot_adapter
- relbot_simulator
- relbot2turtlesim
- relbot_sequence_controller
- turtlesim

For the hardware workflow:
- ros_xeno_bridge
- xrf2_msgs


Location in code
----------------

- launch/full_tracking_system.launch.py
- launch/lab_hardware.launch.py
- launch/relbot_sequence_controller.launch.py
