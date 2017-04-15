#include <ros/ros.h>

#include <geometry_msgs/Pose.h>
#include <eigen_conversions/eigen_msg.h>
#include <visual_slam_msgs/scene.h>

#include <visual_slam/definitions.h>
#include <visual_slam/Map_Optimization/g2omapoptimizer.h>
class MappingNodeHandler
{
public:
  MappingNodeHandler();
  void OdometryCallback(const geometry_msgs::Pose::ConstPtr& msg);
  void SceneDataCallback(const visual_slam_msgs::scene::ConstPtr& msg);

private:
  ros::NodeHandle node;
  ros::Subscriber odom_sub, scene_sub;
  std::unique_ptr<G2OMapOptimizer> mapOptimizer;
  Pose_6D prevPose;
  int prevNodeId;
  short numberOfNode;
  std::vector<int> NodeIds;

  void updateGraph();
  void publishOptomizedPath();
  void detectLoopClosure(cv::Mat currentFeature, Pose_6D Pose, int nodeId);
  int addToMap(Pose_6D newPose);
};

MappingNodeHandler::MappingNodeHandler()
{
  odom_sub = node.subscribe("robot_pose",50,&MappingNodeHandler::OdometryCallback , this);
  scene_sub = node.subscribe("scene_data",50,&MappingNodeHandler::SceneDataCallback,this);
  mapOptimizer = std::unique_ptr<G2OMapOptimizer>(new G2OMapOptimizer);
  prevPose = Pose_6D::Identity();
  mapOptimizer->addPoseToGraph(prevPose, prevNodeId);
  numberOfNode = 0;
  NodeIds.push_back(prevNodeId);
}


void MappingNodeHandler::OdometryCallback(const geometry_msgs::Pose::ConstPtr &msg)
{

}

void MappingNodeHandler::SceneDataCallback(const visual_slam_msgs::scene::ConstPtr &msg)
{
  // add new node
  Pose_6D newPose ;
  tf::poseMsgToEigen(msg->pose,newPose);
  int newNodeId;
  mapOptimizer->addPoseToGraph(newPose, newNodeId);
  NodeIds.push_back(newNodeId);
  mapOptimizer->addEdge(newPose ,newNodeId ,prevNodeId);
  prevPose = newPose;
  prevNodeId = newNodeId;
  if(numberOfNode ==10)
  {
    numberOfNode = 0;
    updateGraph();
  }


}

void MappingNodeHandler::detectLoopClosure(cv::Mat currentFeature, Pose_6D Pose, int nodeId)
{
  if(FeatureMap.size()==0)
  {
    std::pair<cv::Mat,int> tmp;
    tmp.first = currentFeature;
    tmp.second = nodeId;
  }
  else
  {
    int sceneNumber = searchForSimilerScene(currentFeature);
    if(sceneNumber > 0)
    {
      mapOptimizer->addEdge(Pose ,nodeId ,sceneNumber);
    }
    else
    {
      std::pair<cv::Mat,int> tmp;
      tmp.first = currentFeature;
      tmp.second = nodeId;
    }
  }
}
int MappingNodeHandler::addToMap(Pose_6D newPose)
{
  int newNodeId;
  mapOptimizer->addPoseToGraph(newPose, newNodeId);
  NodeIds.push_back(newNodeId);
  mapOptimizer->addEdge(newPose ,newNodeId ,prevNodeId);
  prevPose = newPose;
  prevNodeId = newNodeId;
  if(numberOfNode ==10)
  {
    numberOfNode = 0;
    updateGraph();
  }
  return newNodeId;
}
void MappingNodeHandler::updateGraph()
{
  mapOptimizer->optimize();
}

int main(int argc, char** argv){
  ros::init(argc, argv, "Mapping");

  MappingNodeHandler handler;

  ros::spin();

    return 0;
}
