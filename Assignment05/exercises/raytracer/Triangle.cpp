#include "Triangle.hpp"
#include "Ray.hpp"
#include "Math.hpp"
#include <memory>

namespace rt
{
Triangle::Triangle(const Vec3d &v0, const Vec3d &v1, const Vec3d &v2,
                   const Vec3d &uvw0, const Vec3d &uvw1, const Vec3d &uvw2)
{
  mVertices[0] = v0;
  mVertices[1] = v1;
  mVertices[2] = v2;
  mUVW[0] = uvw0;
  mUVW[1] = uvw1;
  mUVW[2] = uvw2;
}

bool
Triangle::closestIntersectionModel(const Ray &ray, double maxLambda, RayIntersection& intersection) const
{    
	//Programming TASK 1: implement this method
	//Your code should compute the intersection between a ray and a triangle.

	//If you detect an intersection, the return type should look similar to this:
	//if(rayIntersectsTriangle)
	//{
	//  intersection = RayIntersection(ray,shared_from_this(),lambda,ray.normal,uvw);
	//  return true;
	//} 

  // Hints :
  // As explained in Lecture 7 - Raytracing
  // 1. Ray origin p_r : ray.origin()
  //    Ray direction t_r : ray.direction()
  // 2. For a triangle we have t1 = v1 - v0, t2 = v2 - v0, n = cross(t1, t2)
  // 3. First you need to check if the ray : p = p_r + lambda * t_r intersects the Plane : (p - v0).n = 0
  //    solve for lambda. Let this value be lambda'
  //    If lambda' is within [0, maxLambda] the ray intersects the plane
  // 4. If the ray does not intersect the plane, return false. Otherwise, you need to check if the 
  //    point p = p_r + lambda' * t_r (Use ray.pointOnRay(lambda') for this)
  //    lies within the triangle (v0, v1, v2). Using barycentric coordinates is one way of solving this.
	Vec3d t1 = mVertices[1] - mVertices[0];
	Vec3d t2 = mVertices[2] - mVertices[0];
	Vec3d mNormal = cross(t1, t2);
	double a = dot((mVertices[0]-ray.origin()), (mNormal));
	double d = dot(ray.direction(), (mNormal));

	if (fabs(d)<Math::safetyEps())
		return false;

	double lambda = a / d;

	if (lambda<0.0 || lambda>maxLambda)
		return false;
	
	const Vec3d p = ray.pointOnRay(lambda);
	const Vec3d uvw(double(0), double(0), double(0));

	Vec3d P = p - mVertices[0];
	
	double lambda1 = (dot(t2, t2)*dot(t1, P) - dot(t2, t1)*dot(P, t2)) / (dot(t1, t1)*dot(t2, t2) - dot(t1, t2)*dot(t2, t1));
	double lambda2 = (dot(t1, t1)*dot(t2, P) - dot(t1, t2)*dot(t1, P)) / (dot(t1, t1)*dot(t2, t2) - dot(t1, t2)*dot(t2, t1));

	if (lambda1*lambda2 >= 0 && lambda1*lambda2 <= 1 && lambda1 + lambda2 <= 1)
		intersection = RayIntersection(ray, shared_from_this(), lambda, mNormal, uvw); 
	else
		return false;
	
	return true;

}

BoundingBox Triangle::computeBoundingBox() const
{
  BoundingBox bbox;
  bbox.expandByPoint(mVertices[0]);
  bbox.expandByPoint(mVertices[1]);
  bbox.expandByPoint(mVertices[2]);
  return bbox;
}

} //namespace rt
