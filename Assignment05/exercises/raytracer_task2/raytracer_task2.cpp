#include <raytracer/Image.hpp>
#include <raytracer/PerspectiveCamera.hpp>
#include <raytracer/Scene.hpp>
#include <raytracer/Sphere.hpp>
#include <raytracer/Raytracer.hpp>
#include <raytracer/Light.hpp>
#include <raytracer/DiffuseMaterial.hpp>
#include <raytracer/Plane.hpp>
#include <raytracer/Math.hpp>
#include <raytracer/Triangle.hpp>

#include <raytracer/BezierPatchMesh.hpp>
#include <raytracer/CheckerMaterial.hpp>

#include <raytracer/TriangleMesh.hpp>
#include <raytracer/PhongMaterial.hpp>
#include <opengl/RaytracerWindow.hpp>

bool gShowWindow=true;
int imageWidth=512;
int imageHeight=512;
std::string gDataPath= ""; ///< The path pointing to the resources (OBJ, shader)

std::shared_ptr<rt::Scene> makeTask2Scene()
{
  std::shared_ptr<rt::Camera>   camera    = std::make_shared<rt::PerspectiveCamera>();
  camera->setPosition(rt::Vec3d(5.0,0.0,5.0));
  camera->setFOV(60.0,60.0);

  std::shared_ptr<rt::Scene>    scene     = std::make_shared<rt::Scene>();

  std::shared_ptr<rt::Material> material1 = std::make_shared<rt::PhongMaterial>(rt::Vec3d(1.0,0.4,0.1),0.8,1000.0);
  std::shared_ptr<rt::Material> material2 = std::make_shared<rt::PhongMaterial>(rt::Vec3d(0.0,0.0,0.0),0.2,1000.0);
  std::shared_ptr<rt::Material> material3 = std::make_shared<rt::PhongMaterial>(rt::Vec3d(0.2,0.3,0.8),0.1,  10.0);
  std::shared_ptr<rt::Material> material4 = std::make_shared<rt::PhongMaterial>(rt::Vec3d(0.5,0.0,0.0),0.2,  50.0);
  std::shared_ptr<rt::Material> material5 = std::make_shared<rt::PhongMaterial>(rt::Vec3d(0.5,0.5,0.5),0.1, 100.0);

  std::shared_ptr<rt::Sphere>   sphere1   = std::make_shared<rt::Sphere>();
  std::shared_ptr<rt::Sphere>   sphere2   = std::make_shared<rt::Sphere>();
  std::shared_ptr<rt::Sphere>   sphere3   = std::make_shared<rt::Sphere>();
  std::shared_ptr<rt::Sphere>   sphere4   = std::make_shared<rt::Sphere>();
  sphere1->transform().scale(rt::Vec3d(1,1,1)).rotate(0.0,rt::Vec3d(0,0,1)).translate(rt::Vec3d( 1.1, 1.1,1.1));
  sphere2->transform().scale(rt::Vec3d(1,1,1)).rotate(0.0,rt::Vec3d(0,0,1)).translate(rt::Vec3d(-1.1, 1.1,1.1));
  sphere3->transform().scale(rt::Vec3d(1,1,1)).rotate(0.0,rt::Vec3d(0,0,1)).translate(rt::Vec3d( 0.0,-1.1,1.1));
  sphere4->transform().scale(rt::Vec3d(1,1,1)).rotate(0.0,rt::Vec3d(0,0,1)).translate(rt::Vec3d( 0.0, 0.0,2.0));

  //if your result image is too bright, you can dampen the spectral intensity
  double intensityFactor=1.0;
  std::shared_ptr<rt::Light>    light1     = std::make_shared<rt::Light>(rt::Vec3d(  5.0, 2.0,6.0), rt::Vec3d(200,170,150)*intensityFactor);
  std::shared_ptr<rt::Light>    light2     = std::make_shared<rt::Light>(rt::Vec3d(  5.0,-7.0,3.0), rt::Vec3d(200,170,150)*intensityFactor);
  std::shared_ptr<rt::Light>    light3     = std::make_shared<rt::Light>(rt::Vec3d(-10.0, 4.0,5.0), rt::Vec3d(130,160,200)*intensityFactor);

  std::shared_ptr<rt::Plane>    plane     = std::make_shared<rt::Plane>();

  sphere1->setMaterial(material1);
  sphere2->setMaterial(material2);
  sphere3->setMaterial(material3);
  sphere4->setMaterial(material4);
  plane  ->setMaterial(material5);

  scene->addRenderable(sphere1);
  scene->addRenderable(sphere2);
  scene->addRenderable(sphere3);
  scene->addRenderable(sphere4);
  scene->addRenderable(plane);

  scene->addLight(light1);
  scene->addLight(light2);
  scene->addLight(light3);

  scene->setCamera(camera);
  return scene;
}

std::shared_ptr<rt::Scene> makeMeshScene(std::string fileName,bool addArrows)
{
  std::shared_ptr<rt::Camera>   camera    = std::make_shared<rt::PerspectiveCamera>();
  camera->setPosition(rt::Vec3d(2.0,1,1.2));
  camera->setLookAt(rt::Vec3d(0,0,0.4));
  camera->setFOV(60.0,60.0);

  std::shared_ptr<rt::Scene>    scene     = std::make_shared<rt::Scene>();
  scene->setBackgroundColor(rt::Vec4d(0,0,0,1));


  if(addArrows)
  {
    std::shared_ptr<rt::Material> materialRed = std::make_shared<rt::PhongMaterial>   (rt::Vec3d(0.8,0.1,0.1),0.2,1000.0);
    std::shared_ptr<rt::Material> materialGreen = std::make_shared<rt::PhongMaterial> (rt::Vec3d(0.1,0.8,0.1),0.2,1000.0);
    std::shared_ptr<rt::Material> materialBlue = std::make_shared<rt::PhongMaterial>  (rt::Vec3d(0.1,0.1,0.8),0.2,1000.0);

    std::shared_ptr<rt::BVHIndexedTriangleMesh> meshXAxis = std::make_shared<rt::BVHIndexedTriangleMesh>();
    meshXAxis->loadFromOBJ(gDataPath+"assets/arrowZ.obj");
    meshXAxis->transform().rotate(90.f,0,1,0);
    meshXAxis->setMaterial(materialRed);
    scene->addRenderable(meshXAxis);

    std::shared_ptr<rt::BVHIndexedTriangleMesh> meshYAxis = std::make_shared<rt::BVHIndexedTriangleMesh>();
    meshYAxis->loadFromOBJ(gDataPath+"assets/arrowZ.obj");
    meshYAxis->transform().rotate(-90.f,1,0,0);
    meshYAxis->setMaterial(materialGreen);
    scene->addRenderable(meshYAxis);

    std::shared_ptr<rt::BVHIndexedTriangleMesh> meshZAxis = std::make_shared<rt::BVHIndexedTriangleMesh>();
    meshZAxis->loadFromOBJ(gDataPath+"assets/arrowZ.obj");
    meshZAxis->setMaterial(materialBlue);
    scene->addRenderable(meshZAxis);
  }

  std::shared_ptr<rt::Material> materialPlane = std::make_shared<rt::PhongMaterial>
    (rt::Vec3d(0.5,0.5,0.5),0.2, 10.0);

  //if your result image is too bright, you can dampen the spectral intensity
  double intensityFactor=1.0;
  std::shared_ptr<rt::Light>    light1     = std::make_shared<rt::Light>(
    rt::Vec3d(  5.0, 2.0,6.0), rt::Vec3d(200,170,150)*intensityFactor);
  std::shared_ptr<rt::Light>    light2     = std::make_shared<rt::Light>(
    rt::Vec3d(  5.0,-7.0,3.0), rt::Vec3d(200,170,150)*intensityFactor);
  std::shared_ptr<rt::Light>    light3     = std::make_shared<rt::Light>(
    rt::Vec3d(-10.0, 4.0,5.0), rt::Vec3d(130,160,200)*intensityFactor);

  std::shared_ptr<rt::Plane>    plane     = std::make_shared<rt::Plane>();
  plane  ->setMaterial(materialPlane);

  scene->addRenderable(plane);

  scene->addLight(light1);
  scene->addLight(light2);
  scene->addLight(light3);

  scene->setCamera(camera);

  std::shared_ptr<rt::BVHIndexedTriangleMesh> mesh = std::make_shared<rt::BVHIndexedTriangleMesh>();
  if(mesh->loadFromOBJ(gDataPath+fileName))
  {
    std::cout<<"Loaded BVHMesh with "<<mesh->triangleIndices().size()/3<<
      " triangles and "<< mesh->vertexPositions().size()<<" vertices"<<std::endl;

    std::shared_ptr<rt::Material> materialObject = std::make_shared<rt::PhongMaterial>(rt::Vec3d(1,0.4,0.1),0.2,  50.0);
    mesh->setMaterial(materialObject);
    mesh->transform().rotate(180,0,0,1).scale(1,1,1);
    scene->addRenderable(mesh);
  }



  return scene;
}

int main(int argc, char** argv)
{
  std::string pathToThisFile=__FILE__;

  if(gDataPath.empty())
    gDataPath=rt::Math::getParentDirectoryFromFilePath(pathToThisFile);
  std::cerr<<"Using data path: "<<gDataPath<<std::endl;

  std::shared_ptr<rt::Image> image = std::make_shared<rt::Image>(imageWidth,imageHeight);

  //comment the line below for the bonus task
  std::shared_ptr<rt::Scene> scene = makeTask2Scene(); //Task2 with four spheres

  //Your triangle mesh in OBJ format should be oriented around the origin, with side lengths ~= 1 units
  //You can hide/show the world coordinate system axes as arrows
  bool showArrows=true;
  //uncomment the line below for the bonus task
  //std::shared_ptr<rt::Scene> scene = makeMeshScene("assets/8.obj",showArrows);

  std::shared_ptr<rt::Raytracer> raytracer = std::make_shared<rt::Raytracer>();
  //set maxDepth = 50 like this
  //std::shared_ptr<rt::Raytracer> raytracer = std::make_shared<rt::Raytracer>(50);
  raytracer->setScene(scene);

  // Note the fastest way to execute your program using Visual Studio is pressing Ctrl+F5
  // in Release mode. This will detach the rendering process from the debugger.

  rt::Chrono before = std::chrono::high_resolution_clock::now();
  raytracer->renderToImage(image);
  rt::ChronoDuration timeToRender = std::chrono::duration_cast<rt::ChronoDuration>(std::chrono::high_resolution_clock::now()-before);
  std::cout<<"Rendering took "<<timeToRender.count()<<" seconds"<<std::endl;

  image->saveToTGA(std::string(argv[0])+"_result");

  if(gShowWindow)
  {
    rt::RaytracerWindow win(imageWidth,imageHeight);
    if(win.init())
      win.drawOnce(image);
    else
      return -1;
  }

  return 0;
}
