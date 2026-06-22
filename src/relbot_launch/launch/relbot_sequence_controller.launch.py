#=============================================================================
# Authors : Goncalo Pinheiro, Ceyhun Ceylan
# Group : 17
# License : LGPL open source license
#
# Brief : Launches the sequence controller with the simulator-side support nodes.
#
#=============================================================================

from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.conditions import IfCondition
from launch.substitutions import LaunchConfiguration, PythonExpression
from launch_ros.actions import Node
from launch_ros.parameter_descriptions import ParameterValue


def generate_launch_description():
    robotmode = LaunchConfiguration("robotmode")
    adapter_output_rate_hz = LaunchConfiguration("adapter_output_rate_hz")
    command_timeout_sec = LaunchConfiguration("command_timeout_sec")
    max_speed_mps = LaunchConfiguration("max_speed_mps")
    max_speed_rads = LaunchConfiguration("max_speed_rads")

    robotmode_arg = DeclareLaunchArgument(
        "robotmode",
        default_value="sim",
        description='Target robot mode: "sim" or "real".',
    )
    adapter_output_rate_hz_arg = DeclareLaunchArgument(
        "adapter_output_rate_hz",
        default_value="50.0",
        description="Fixed publish rate for relbot_adapter in Hz.",
    )
    command_timeout_sec_arg = DeclareLaunchArgument(
        "command_timeout_sec",
        default_value="0.30",
        description="Time without new commands before relbot_adapter outputs a stop command.",
    )
    max_speed_mps_arg = DeclareLaunchArgument(
        "max_speed_mps",
        default_value="0.25",
        description="Maximum wheel speed in meters per second.",
    )
    max_speed_rads_arg = DeclareLaunchArgument(
        "max_speed_rads",
        default_value="5.0",
        description="Maximum wheel speed in radians per second.",
    )

    relbot_adapter = Node(
        package="relbot_adapter",
        executable="relbot_adapter",
        name="relbot_adapter",
        parameters=[{
            "robotmode": ParameterValue(robotmode, value_type=str),
            "output_rate_hz": ParameterValue(adapter_output_rate_hz, value_type=float),
            "command_timeout_sec": ParameterValue(command_timeout_sec, value_type=float),
            "max_speed_mps": ParameterValue(max_speed_mps, value_type=float),
            "max_speed_rads": ParameterValue(max_speed_rads, value_type=float),
        }],
    )

    relbot_simulator = Node(
        package="relbot_simulator",
        executable="relbot_simulator",
        name="relbot_simulator",
        parameters=[{"use_adapter_cmd": True}],
        condition=IfCondition(PythonExpression(["'", robotmode, "' == 'sim'"]))
    )

    relbot2turtlesim = Node(
        package="relbot2turtlesim",
        executable="relbot2turtlesim",
        name="relbot2turtlesim",
        condition=IfCondition(PythonExpression(["'", robotmode, "' == 'sim'"]))
    )

    relbot_sequence_controller = Node(
        package="relbot_sequence_controller",
        executable="relbot_sequence_controller",
        name="relbot_sequence_controller"
    )

    turtlesim = Node(
        package="turtlesim",
        executable="turtlesim_node",
        name="turtlesim",
        condition=IfCondition(PythonExpression(["'", robotmode, "' == 'sim'"]))
    )

    return LaunchDescription([
        robotmode_arg,
        adapter_output_rate_hz_arg,
        command_timeout_sec_arg,
        max_speed_mps_arg,
        max_speed_rads_arg,
        relbot_adapter,
        relbot_simulator,
        relbot2turtlesim,
        relbot_sequence_controller,
        turtlesim
    ])
