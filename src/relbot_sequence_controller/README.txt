Package relbot_sequence_controller
==================================

Description
-----------

This package implements the Assignment 6 closed-loop controller for following the detected
green ball. The controller uses the horizontal ball position for steering, the blob area as a
distance proxy, and a search behaviour when the ball is lost.


Inputs
------

/ball_position
Type: geometry_msgs/msg/Point
Detected ball position and area from ball_detector.

/output/moving_camera
Type: sensor_msgs/msg/Image
Used in simulation to update the current image width.

/image
Type: sensor_msgs/msg/Image
Used in the direct camera / hardware workflow.


Outputs
-------

/input/left_motor/setpoint_vel
Type: example_interfaces/msg/Float64

/input/right_motor/setpoint_vel
Type: example_interfaces/msg/Float64


Parameters
----------

image_topic
Type: string
Default: /output/moving_camera
Image topic used to update the current image width.

target_ball_area
Type: double
Default: 9000.0
Blob area at which the robot stops because the ball is close enough.

center_deadband_px
Type: double
Default: 4.0
Deadband around the image centre to reduce oscillation.

ball_timeout
Type: double
Default: 0.25
If no recent detection arrives within this time, the controller switches to search mode.


Run
---

Simulation use:

ros2 run relbot_sequence_controller relbot_sequence_controller --ros-args -p image_topic:=/output/moving_camera -p target_ball_area:=9000.0 -p center_deadband_px:=4.0 -p ball_timeout:=0.35

Direct camera / hardware use:

ros2 run relbot_sequence_controller relbot_sequence_controller --ros-args -p image_topic:=/image -p target_ball_area:=9000.0 -p center_deadband_px:=4.0 -p ball_timeout:=0.35


Core components
---------------

ball_callback()
Stores the latest detected ball position and area and marks the ball as visible.

image_callback()
Reads the image width from the subscribed image topic.

calculate_velocity()
Computes steering and forward motion from the current ball position and blob area.

search_for_ball()
Rotates in place to reacquire the ball when no recent detection is available.

publish()
Publishes the final left and right wheel setpoints.


Location in code
----------------

- include/steering_sim.hpp
- src/relbot_sequence_controller.cpp

