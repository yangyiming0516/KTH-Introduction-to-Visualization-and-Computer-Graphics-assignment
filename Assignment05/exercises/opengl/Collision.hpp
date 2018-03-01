#ifndef COLLISION_HPP_INCLUDE_ONCE
#define COLLISION_HPP_INCLUDE_ONCE

#include "OpenGL.hpp"

namespace ogl
{

  class Ray
  {
  public:
    OPENGL_EXPORTS Ray() : mOrigin(0.0,0.0,0.0), mDirection(0.0,0.0,0.0) {}
    OPENGL_EXPORTS Ray(const Vec3f &origin, const Vec3f &direction) :
      mOrigin(origin), mDirection(Vec3f(direction).normalize())  {}

    OPENGL_EXPORTS Vec3f pointOnRay(float lambda) const { return mOrigin + mDirection*lambda; }

    OPENGL_EXPORTS const Vec3f& origin()    const { return mOrigin; }
    OPENGL_EXPORTS const Vec3f& direction() const { return mDirection; }

    OPENGL_EXPORTS void setOrigin(const Vec3f& origin)       { mOrigin=origin; }
    OPENGL_EXPORTS void setDirection(const Vec3f& direction) { mDirection=Vec3f(direction).normalize(); }

    OPENGL_EXPORTS void transform(const Mat4x4f &transform)
    {
      mOrigin    = transform*mOrigin;
      mDirection = (transform.as3x3()*mDirection).normalize();
    }

    OPENGL_EXPORTS Ray transformed(const Mat4x4f &transform) const
    {
      return Ray(transform*mOrigin,
        transform.as3x3()*mDirection);
    }

  private:
    Vec3f mOrigin;
    Vec3f mDirection;
  };

  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////

  /// Container class for intersection between a renderable and a ray
  class RayIntersection
  {
  public:

    OPENGL_EXPORTS RayIntersection(){}

    OPENGL_EXPORTS RayIntersection(const Ray &ray,
      const float lambda, const Vec3f &normal) :
    mRay(ray), mLambda(lambda), mNormal(normal)
    {
      mPosition=ray.pointOnRay(mLambda);
    }

    OPENGL_EXPORTS virtual ~RayIntersection() {}

    OPENGL_EXPORTS const Ray& ray()                               const { return mRay; }
    OPENGL_EXPORTS float lambda()                                  const { return mLambda; }
    OPENGL_EXPORTS const Vec3f& position()                         const { return mPosition; }
    OPENGL_EXPORTS const Vec3f& normal()                           const { return mNormal; }

    OPENGL_EXPORTS virtual void transform(const Mat4x4f &transform,
      const Mat4x4f &transformInvTransp)
    {

      //transform local intersection information to global system
      mPosition = transform*mPosition;
      mNormal   = (transformInvTransp.as3x3()*mNormal).normalize();
      mRay.transform(transform);
      mLambda   = (mPosition - mRay.origin()).length();
    }

  protected:
    Ray mRay;
    float mLambda;
    Vec3f mPosition;
    Vec3f mNormal;
  };

/**
 * Represents an axis-aligned bounding box.
 */
class BoundingBox
{
public:
  OPENGL_EXPORTS BoundingBox() : 
    mMin( std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity()),
    mMax(-std::numeric_limits<float>::infinity(),-std::numeric_limits<float>::infinity(),-std::numeric_limits<float>::infinity())
  { } 
  OPENGL_EXPORTS BoundingBox(const Vec3f &min, const Vec3f &max) : mMin(min), mMax(max) { }
        
  // Returns true in case of any intersection
  OPENGL_EXPORTS inline bool anyIntersection(const Ray &ray, float maxLambda) const
  {
    //   float tnear = -std::numeric_limits<float>::max(); 
    //   float tfar = std::numeric_limits<float>::max();
    float tnear = 0; 
    float tfar = maxLambda;
    float t1;
    float t2;

    Vec3f direction = ray.direction();
    Vec3f origin = ray.origin();

    for (int i=0; i<3; i++)
    {
      if (direction[i]) // this is numerically unstable for small values!
      {
        t1 = (mMin[i]-origin[i])/direction[i];
        t2 = (mMax[i]-origin[i])/direction[i];
        if (t1 > t2){
          float tmp = t2;
          t2 = t1;
          t1 = tmp;
        }
        if (t1 > tnear)
          tnear = t1;
        if (t2 < tfar)
          tfar = t2;
        if (tnear > tfar)
          return false;
        if (tfar < 0)
          return false;
      }else{
        if (origin[i]<mMin[i] || origin[i]>mMax[i]) //is this right? we might not reject some trivial cases
          return false;
      }
    }

    return true;
  }

  OPENGL_EXPORTS void merge(const BoundingBox& bb) // merge with a given bounding box
  {
    mMin = Vec3f(std::min(bb.min()[0],mMin[0]),std::min(bb.min()[1],mMin[1]),std::min(bb.min()[2],mMin[2]));
    mMax = Vec3f(std::max(bb.max()[0],mMax[0]),std::max(bb.max()[1],mMax[1]),std::max(bb.max()[2],mMax[2]));
  }      

  OPENGL_EXPORTS const Vec3f& min() const { return mMin; }
  OPENGL_EXPORTS const Vec3f& max() const { return mMax; }

  OPENGL_EXPORTS void setMin(const Vec3f &min) { mMin = min; }
  OPENGL_EXPORTS void setMax(const Vec3f &max) { mMax = max; }

  OPENGL_EXPORTS void expandByPoint(const Vec3f& p)
  {
    for (int i=0;i<3;++i)
    {
      mMin[i]=std::min(mMin[i],p[i]);
      mMax[i]=std::max(mMax[i],p[i]);
    }
  }

  OPENGL_EXPORTS float computeArea() const
  {
    Vec3f d = mMax-mMin;
    return 2*(d[0]*d[1]+d[0]*d[2]+d[1]*d[2]);
  }

private:
    Vec3f mMin; // Coordinate of min corner
    Vec3f mMax; // Coordinate of max corner
};

} //namespace ogl

#endif //COLLISION_HPP_INCLUDE_ONCE
