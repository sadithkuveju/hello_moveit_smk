#include <memory>

#include <rclcpp/rclcpp.hpp>
#include <moveit/move_group_interface/move_group_interface.h>

int main(int argc, char * argv[])
{
  // Initialize ROS and create the Node
  // Everything in ROS works through a ROS Node. We call this node 'hello_moveit'. You can give
  // any name you want. When you type "$ros2 node list"  in the terminal, you will see all the
  // ROS nodes currently running in the systems.
  rclcpp::init(argc, argv);
  auto const node = std::make_shared<rclcpp::Node>(
    "hello_moveit",
    rclcpp::NodeOptions().automatically_declare_parameters_from_overrides(true)
  );

  // Create a ROS logger. This is to log messages in the system. similar to a print statement.
  auto const logger = rclcpp::get_logger("hello_moveit");

  // Next step goes here
  // Create the MoveIt MoveGroup Interface.
  // move_group_interface ia an important class. This provides access to all the movement related functions.
  // Whenever we want to plan a new path or execute a path, we do it through move_group_interface.
  // move_group_interface take two arguments(or parameters), First onw is the ros2 node you created in line 13.
  // Second is the name of the 'planning group'. Planning group is set when we create a robot using URDF files.
  // 'ur_manipulator is the default name given to the UR robots. See the same name will be there in the
  // RViz window under MotionPlanning.
  using moveit::planning_interface::MoveGroupInterface;
  auto move_group_interface = MoveGroupInterface(node, "ur_manipulator");

  // There are two ways of moving the robot. Below is the first way. It tells the robot's 'end effector' to
  // go to a certain point in the x-y-z coordinate space. these are relevant to the robot's position.
  // All it does is to move the end effector to the set point, it doesn't care how the whole robot arm is aligned
  // Change these xyz values and see. If it can't reach a position, it will throw an error.
  // These poses are communicated to the robot visa a message type called 'geometry_msgs::Pose'
  // Here's the link to the message's full definition: https://docs.ros2.org/latest/api/geometry_msgs/msg/Pose.html
  auto const target_pose = [] {
      geometry_msgs::msg::Pose msg;
      msg.orientation.w = 1.0;
      msg.position.x = -0.28;
      msg.position.y = 0.2;
      msg.position.z = 0.5;
      return msg;
    }();

  // Note here we use move_group_interface to set the target pose
  move_group_interface.setPoseTarget(target_pose);

  // Create a plan to that target pose via move_group_interface.
  // Forget how the below function is written. It's called a lambda function, and they are complex.
  // All you need to know that, if move_group_interface can generate a plan, it will be assigned to the variable 'plan'
  auto const [success, plan] = [&move_group_interface] {
      moveit::planning_interface::MoveGroupInterface::Plan msg;
      auto const ok = static_cast<bool>(move_group_interface.plan(msg));
      return std::make_pair(ok, msg);
    }();

  // Next we execute the plan via move_group_interface.
  if (success) {
    move_group_interface.execute(plan);
  } else {
    RCLCPP_ERROR(logger, "Planing failed!");
  }

  // Here is the second way of moving the robot.
  // We define an array of six elements that corresponds to robot's six joints, first element-> base to last element -> end effector.
  // These values are in radians.
  // Change these values and try to rotate the robot to the desired angels.

  std::vector<double> joint_goal = {-0.11536, -1.383732, 0.68816, -1.05492, 0.17484, 3.14159};
  // We set our joint values via move_group_interface
  move_group_interface.setJointValueTarget(joint_goal);

  // Again we create a new plan via move_group_interface
  auto const [success2, plan2] = [&move_group_interface] {
      moveit::planning_interface::MoveGroupInterface::Plan msg;
      auto const ok = static_cast<bool>(move_group_interface.plan(msg));
      return std::make_pair(ok, msg);
    }();

  // Next we execute the new plan via move_group_interface.
  if (success2) {
    move_group_interface.execute(plan2);
  } else {
    RCLCPP_ERROR(logger, "Planing failed!");
  }

  // Shutdown ROS
  rclcpp::shutdown();
  return 0;
}
