#include <QMouseEvent>
#include <QGuiApplication>

#include "NGLScene.h"
#include <ngl/NGLInit.h>
#include <ngl/NGLStream.h>
#include <ngl/ShaderLib.h>
#include <ngl/VAOPrimitives.h>
#include <ngl/VAOFactory.h>
#include <ngl/Util.h>
#include <iostream>
#include <vector>

NGLScene::NGLScene()
{
  // re-size the widget to that of the parent (in this case the GLFrame passed in on construction)
  setTitle("VAO Grid");
  std::cout<<m_view<<'\n'<<m_projection<<'\n';
  m_view=ngl::lookAt(ngl::Vec3(12.0f,12.0f,15.0f),
                     ngl::Vec3::zero(),
                     ngl::Vec3::up());

  std::cout<<"view \n"<<m_view<<'\n';


}


NGLScene::~NGLScene()
{
  std::cout<<"Shutting down NGL, removing VAO's and Shaders\n";
}



void NGLScene::resizeGL(int _w , int _h)
{
  m_win.width  = static_cast<int>( _w * devicePixelRatio() );
  m_win.height = static_cast<int>( _h * devicePixelRatio() );
  m_projection=ngl::perspective(45.0f,
                                static_cast<float>(_w)/_h,
                                0.5f,
                                200.0f);
  std::cout<<"projection \n"<<m_projection<<'\n';

}


void NGLScene::buildMesh(ngl::Real _w, ngl::Real _d, size_t _stepsW, size_t _stepsD)
{
  std::vector<ngl::Vec3> data;

  // plane origin =0,0,0
  // extents -W / 2 -> +W / 2
  // -D /2 -> +D/2
  ngl::Real w2=_w / 2.0f;
  ngl::Real d2=_d / 2.0f;
  // now for the steps in w and d
  ngl::Real wStep=_w/_stepsW;
  ngl::Real dStep=_d/_stepsD;

  ngl::Vec3 vert;
  vert.m_y=0.0f;

  for(ngl::Real d=-d2; d<d2; d+=dStep)
  {
    for(ngl::Real w=-w2; w<w2; w+=wStep)
    {
      // first triangle
      vert.m_x=w; vert.m_z=d+dStep; // V1
      data.push_back(vert);
      vert.m_x=w+wStep; vert.m_z=d+dStep; // V2
      data.push_back(vert);
      vert.m_x=w; vert.m_z=d; // V3
      data.push_back(vert);

      vert.m_x=w+wStep; vert.m_z=d+dStep;
      data.push_back(vert);
      vert.m_x=w+wStep; vert.m_z=d;
      data.push_back(vert);
      vert.m_x=w; vert.m_z=d;
      data.push_back(vert);


    }
  }

  m_nVerts=data.size();

  m_vao.reset(ngl::VAOFactory::createVAO(ngl::simpleVAO,GL_TRIANGLES));
  m_vao->bind();
  m_vao->setData(ngl::AbstractVAO::VertexData(data.size()*sizeof(ngl::Vec3),
                                              data[0].m_x));
  m_vao->setVertexAttributePointer(0,3,GL_FLOAT,0,0);
  m_vao->setNumIndices(data.size());
  m_vao->unbind();



}

void NGLScene::timerEvent(QTimerEvent *)
{
  static float time;
  time+=0.1f;
  m_vao->bind();
  ngl::Vec3 *ptr =static_cast<ngl::Vec3 *>
      ( glMapBuffer(GL_ARRAY_BUFFER,GL_READ_WRITE) );

  for(size_t i=0; i<m_vao->numIndices(); ++i)
  {
    ptr[i].m_y=sinf(ptr[i].m_x+time);
  }
  for(size_t i=0; i<m_vao->numIndices(); ++i)
  {
    ptr[i].m_y+=sinf(ptr[i].m_z+time);
  }
  glUnmapBuffer(GL_ARRAY_BUFFER);
  m_vao->unbind();
  update();
}




void NGLScene::initializeGL()
{
  // we need to initialise the NGL lib which will load all of the OpenGL functions, this must
  // be done once we have a valid GL context but before we call any GL commands. If we dont do
  // this everything will crash
  ngl::NGLInit::instance();
  glClearColor(0.4f, 0.4f, 0.4f, 1.0f);			   // Grey Background
  // enable depth testing for drawing
  glEnable(GL_DEPTH_TEST);
  // enable multisampling for smoother drawing
  glEnable(GL_MULTISAMPLE);


  ngl::ShaderLib *shader = ngl::ShaderLib::instance();
  shader->use("nglColourShader");
  shader->setUniform("Colour",1.0f,1.0f,1.0f,1.0f);
  buildMesh(60.0f,60.0f,40,40 );
  startTimer(20);
  glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
}



void NGLScene::paintGL()
{
  // clear the screen and depth buffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glViewport(0,0,m_win.width,m_win.height);
  ngl::ShaderLib *shader = ngl::ShaderLib::instance();
  // Rotation based on the mouse position for our global
  // transform
  ngl::Mat4 rotX;
  ngl::Mat4 rotY;
  ngl::Mat4 m_mouseGlobalTX;
  // create the rotation matrices
  rotX.rotateX(m_win.spinXFace);
  rotY.rotateY(m_win.spinYFace);
  // multiply the rotations
  ngl::Mat4 mouseGlobalTX=rotY*rotX;
  // add the translations
  mouseGlobalTX.m_m[3][0] = m_modelPos.m_x;
  mouseGlobalTX.m_m[3][1] = m_modelPos.m_y;
  mouseGlobalTX.m_m[3][2] = m_modelPos.m_z;

  shader->setUniform("MVP", mouseGlobalTX * m_view*m_projection);
  m_vao->bind();
  m_vao->draw();
  m_vao->unbind();

}

//----------------------------------------------------------------------------------------------------------------------

void NGLScene::keyPressEvent(QKeyEvent *_event)
{
  // this method is called every time the main window recives a key event.
  // we then switch on the key value and set the camera in the GLWindow
  switch (_event->key())
  {
  // escape key to quite
  case Qt::Key_Escape : QGuiApplication::exit(EXIT_SUCCESS); break;
  case Qt::Key_Space :
      m_win.spinXFace=0;
      m_win.spinYFace=0;
      m_modelPos.set(ngl::Vec3::zero());

  break;

  case Qt::Key_W : glPolygonMode(GL_FRONT_AND_BACK,GL_LINE); break;

  case Qt::Key_S : glPolygonMode(GL_FRONT_AND_BACK,GL_FILL); break;

  default : break;
  }
  // finally update the GLWindow and re-draw

    update();
}
