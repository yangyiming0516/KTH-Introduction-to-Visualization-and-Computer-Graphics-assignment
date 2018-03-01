#include "CollisionScene.hpp"
#include "CollisionGeometry.hpp"

namespace ogl
{

bool CollisionScene::closestIntersection(const Ray &ray, RayIntersection& intersection, float maxLambda) const
{
  float closestLambda = maxLambda;
  RayIntersection tmpIntersection;
  RayIntersection closestIntersection;
  bool hit(false);
  for (size_t i=0;i<mGeometries.size();++i)
  {
    std::shared_ptr<CollisionGeometry> r = mGeometries[i];
    if(r->closestIntersection(ray,closestLambda,tmpIntersection))
    {
      if(tmpIntersection.lambda() < closestLambda)
      {
        hit=true;
        closestLambda = tmpIntersection.lambda();
        closestIntersection = tmpIntersection;
      }
    }
  }
  intersection=closestIntersection;
  return hit;
}

} //namespace ogl