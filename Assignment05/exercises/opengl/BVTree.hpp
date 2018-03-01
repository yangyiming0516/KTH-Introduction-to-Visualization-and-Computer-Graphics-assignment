#ifndef BVTREE_HPP_INCLUDE_ONCE
#define BVTREE_HPP_INCLUDE_ONCE
#include "openglConfig.hpp"

#include "OpenGL.hpp"
#include <vector>
#include <stack>
#include "Collision.hpp"

namespace ogl
{

class BVTree
{
public:
  BVTree();

  //build from indexed triangle set
  OPENGL_EXPORTS void build(const std::vector<Vec3f> &vertexPositions,const std::vector<Vec3i> &triangleIndices);

  //returns a set of triangle indices as candidates for ray-triangle intersection
  OPENGL_EXPORTS const std::vector<int>& intersectBoundingBoxes(const Ray &ray, const float maxLambda) const;

  OPENGL_EXPORTS size_t numNodes() const { return mNodes.size();}
private:

  struct Node
  {
    Node() {left=0;right=0;}
    int left;
    int right;
    BoundingBox bbox;
  };
  void sortTriangles();
  void createNodes(const std::vector<Vec3f> &vertexPositions,
    const std::vector<Vec3i> &triangleIndices);

  void printSortedIndicesStatus(size_t offset, size_t numTriangles);
  void buildHierarchy(size_t rootNodeIndex, size_t numTriangles,size_t offset);

  void computeBoundingBoxAreas(size_t offset, size_t numTriangles);

  std::vector<Node> mNodes;

  std::vector<bool>        mTempMarker;
  std::vector<BoundingBox> mTempTriangleBoxes;
  std::vector<Vec3i>       mTempSortedTriangleIndices;
  std::vector<Vec3i>       mTempBufferTriangleIndices;
  std::vector<Vec3f>        mTempAreasLeft;
  std::vector<Vec3f>        mTempAreasRight;
  mutable std::vector<std::vector<int>> mTempCandidates;   
  mutable std::vector<std::stack<int>>  mTempTraversalJobs;
  
};
} //namespace ogl

#endif //BVTREE_HPP_INCLUDE_ONCE