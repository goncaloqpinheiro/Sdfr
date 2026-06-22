#=============================================================================
# Authors : Goncalo Pinheiro, Ceyhun Ceylan
# Group : 17
# License : LGPL open source license
#
# Brief : Launches the ROS-side hardware workflow for the physical RELbot.
#
#=============================================================================

from launch import LaunchDescription
from launch_ros.actions import Node
from launch.actions import TimerAction

def generate_launch_description():
    
    adapter_node = Node(
        package="relbot_adapter",
        executable="relbot_adapter",
        name="relbot_adapter",
        parameters=[{
            "robotmode": "real",
            "use_twist_cmd": False,
            "output_rate_hz": 50.0,
            "command_timeout_sec": 0.30,
            "max_speed_mps": 0.25,
            "max_speed_rads": 5.0,
        }],
    )
    
    bridge_node = Node(
        package="ros_xeno_bridge",
        executable="RosXenoBridge",
        name="ros_xeno_bridge",
    )
    
    delayed_bridge = TimerAction(
        period=2.0,
        actions=[bridge_node]
    )

    camera_node = Node(
        package="cam2image_vm2ros",
        executable="cam2image",
        name="cam2image",
        parameters=["src/cam2image_vm2ros/config/cam2image.yaml"]
    )
    
    detector_node = Node(
        package="ball_detector",
        executable="detector_node",
        name="ball_detector",
        parameters=[{"image_topic": "/image"}]
    )
    
    sequence_node = Node(
        package="relbot_sequence_controller",
        executable="relbot_sequence_controller",
        name="relbot_sequence_controller",
        parameters=[
            {"image_topic": "/image"},
            {"target_ball_area": 9000.0},
            {"center_deadband_px": 20.0},
            {"ball_timeout": 0.35}
        ]
    )

    return LaunchDescription([
        adapter_node,
        delayed_bridge,
        camera_node,
        detector_node,
        sequence_node
    ])
