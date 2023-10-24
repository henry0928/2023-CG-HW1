#include <algorithm>
#include <memory>
#include <vector>
#include <iostream>

#include <GLFW/glfw3.h>
#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#undef GLAD_GL_IMPLEMENTATION
#include <glm/glm.hpp>

#include "camera.h"
#include "opengl_context.h"
#include "utils.h"

#define ANGLE_TO_RADIAN(x) (float)((x)*M_PI / 180.0f) 
#define RADIAN_TO_ANGEL(x) (float)((x)*180.0f / M_PI) 
#define RADUIS 0.5
#define CIRCLE_SEGMENT 64

#define ROTATE_SPEED 1.0f
#define FLYING_SPEED ROTATE_SPEED / 20.f

#define A 0.0f, 0.5f, 2.0f 
#define B -1.0f, 0.5f, 3.0f 
#define C 1.0f, 0.5f, 3.0f 
#define D 0.0f, 0.0f, 3.0f 

#define RED 0.905f, 0.298f, 0.235f
#define BLUE 0.203f, 0.596f, 0.858f
#define GREEN 0.18f, 0.8f, 0.443f

float g_up_move = 0.0;
float g_foword_move = 0.0;
float g_temp;
float g_x_move;
float turn = 0.0;
bool is_fly; 
bool is_turn;
bool left_is_up;
bool is_up;
float right_rotate_speed = 0;
float left_rotate_speed = 0;

void resizeCallback(GLFWwindow* window, int width, int height) {
  OpenGLContext::framebufferResizeCallback(window, width, height);
  auto ptr = static_cast<Camera*>(glfwGetWindowUserPointer(window));
  if (ptr) {
    ptr->updateProjectionMatrix(OpenGLContext::getAspectRatio());
  }
}

void keyCallback(GLFWwindow* window, int key, int, int action, int) {
  // There are three actions: press, release, hold(repeat)
  if (action == GLFW_REPEAT) 
      return;

  // Press ESC to close the window.
  if (key == GLFW_KEY_ESCAPE) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
    return;
  }
  /* TODO#4-1: Detect key-events, perform rotation or fly
   *       1. Use switch && case to find the key you want.
   *       2. Press "SPACE" for fly up, fly forward and wing rotate meanwhile. 
   *       3. Press "GLFW_KEY_LEFT" for turn left.
   *       4. Press "GLFW_KEY_RIGHT " for turn right.
   * Hint:
   *       glfw3's key list (https://www.glfw.org/docs/3.3/group__keys.html)
   *       glfw3's action codes (https://www.glfw.org/docs/3.3/group__input.html#gada11d965c4da13090ad336e030e4d11f)
   * Note:
   *       You should finish rendering your airplane first.
   *       Otherwise you will spend a lot of time debugging this with a black screen.
   */
}

void initOpenGL() {
  // Initialize OpenGL context, details are wrapped in class.
#ifdef __APPLE__
  // MacOS need explicit request legacy support
  OpenGLContext::createContext(21, GLFW_OPENGL_ANY_PROFILE);
#else
  //OpenGLContext::createContext(21, GLFW_OPENGL_ANY_PROFILE);
  OpenGLContext::createContext(43, GLFW_OPENGL_COMPAT_PROFILE);
#endif
  GLFWwindow* window = OpenGLContext::getWindow();
  /* TODO#0: Change window title to "HW1 - `your student id`"
   *        Ex. HW1 - 312550000 
   */
  glfwSetWindowTitle(window, "HW1 - 311551130");
  glfwSetKeyCallback(window, keyCallback);
  glfwSetFramebufferSizeCallback(window, resizeCallback);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
#ifndef NDEBUG
  OpenGLContext::printSystemInfo();
  // This is useful if you want to debug your OpenGL API calls.
  OpenGLContext::enableDebugCallback();
#endif
}

void drawcircle(float _z) { 
  glBegin(GL_TRIANGLE_FAN);
  if ( _z < 0 )
    glNormal3f(0.0f, 0.0f, 1.0f);
 else 
    glNormal3f(0.0f, 0.0f, -1.0f);
  glVertex3f(0.0f, 0.5f, _z);
  float p;
  if (_z < 0) {
    for (int i = CIRCLE_SEGMENT+1; i >= 1; i--) {
      p = (float)((float)i * (ANGLE_TO_RADIAN(360.0f) / (float)CIRCLE_SEGMENT));
      glColor3f(BLUE);
      glVertex3f(RADUIS * cos(p),  0.5 + RADUIS * sin(p), _z );
    }  // for
  } // if 
  else {
    for (int i = 1; i <= CIRCLE_SEGMENT + 1; i++) {
      p = (float)((float)i * (ANGLE_TO_RADIAN(360.0f) / (float)CIRCLE_SEGMENT));
      glColor3f(BLUE);
      glVertex3f(RADUIS * cos(p),  0.5 + RADUIS * sin(p), _z );
    }  // for
  } // else 
  glEnd();
}

void drawrectangle(float _z) { 
   glBegin(GL_QUAD_STRIP);
  float p;
   for (int i = 1; i <= CIRCLE_SEGMENT+1; i++) {
    p = (float)((float)i * (ANGLE_TO_RADIAN(360.0f) / (float)CIRCLE_SEGMENT));
      glColor3f(BLUE);
      glVertex3f(RADUIS * cos(p), 0.5 + RADUIS * sin(p), _z );
      glVertex3f(RADUIS * cos(p), 0.5 + RADUIS * sin(p), -_z );
    } // for 
   glEnd();
}

void drawbody() { 
    glPushMatrix();
    //glTranslatef(g_x_move, g_up_move, g_temp); 
    glTranslatef(0.0f, g_up_move, g_foword_move);
    glRotatef(turn, 0.0f, 1.0f, 0.0f);
    drawcircle(2.0f);
    drawcircle(-2.0f);
    drawrectangle(2.0f);
    glPopMatrix();
}

void drawleftwing() {
  
  glPushMatrix();
  // glTranslatef(g_x_move, g_up_move, g_temp); 
   glTranslatef(0.0f, g_up_move, g_foword_move);
  glRotatef(turn, 0.0f, 1.0f, 0.0f);
  if (!is_fly)
    glRotatef(left_rotate_speed, 0.0f, 0.0f, 1.0f);
  else if (!left_is_up) {
    left_rotate_speed = left_rotate_speed - ROTATE_SPEED;
    glRotatef(left_rotate_speed, 0.0f, 0.0f, 1.0f);
    if (left_rotate_speed <= -20.0) {
      left_is_up = true;
    }  // if
  }    // else if
  else if (left_is_up) {
    left_rotate_speed = left_rotate_speed + ROTATE_SPEED;
    glRotatef(left_rotate_speed, 0.0f, 0.0f, 1.0f);
    if (left_rotate_speed >= 20.0) {
      left_is_up = false;
    } // if
  }  // else if 
  glBegin(GL_TRIANGLE_STRIP);  // 第一面
  glColor3f(RED);
  glNormal3f(0.0f, 1.0f, 0.0f);
  glVertex3f(0.0f, 0.75f , -0.5f );
  glVertex3f(-4.0f, 0.75f , -0.5f );
  glVertex3f(0.0f, 0.75f , 0.5f );
  glVertex3f(-4.0f, 0.75f , 0.5f );
  glEnd();


  glBegin(GL_TRIANGLE_STRIP);  // 第二面
  glColor3f(RED);
  glNormal3f(0.0f, -1.0f, 0.0f);
  glVertex3f(-4.0f, 0.25f, -0.5f);
  glVertex3f(0.0f, 0.25f, -0.5f );
  glVertex3f(-4.0f, 0.25f, 0.5f);
  glVertex3f(0.0f, 0.25f , 0.5f );
  glEnd();

  glBegin(GL_TRIANGLE_STRIP);  // 第三面
  glColor3f(RED);
  glNormal3f(0.0f, 0.0f, 1.0f);
  glVertex3f(-4.0f, 0.75f , -0.5f );
  glVertex3f(0.0f, 0.75f , -0.5f );
  glVertex3f(-4.0f, 0.25f , -0.5f );
  glVertex3f(0.0f, 0.25f , -0.5f );
  glEnd();

  glBegin(GL_TRIANGLE_STRIP);  // 第四面
  glColor3f(RED);
  glNormal3f(0.0f, 0.0f, -1.0f);
  glVertex3f(0.0f, 0.75f , 0.5f );
  glVertex3f(-4.0f, 0.75f , 0.5f );
  glVertex3f(0.0f, 0.25f , 0.5f );
  glVertex3f(-4.0f, 0.25f , 0.5f );
  glEnd();

  glBegin(GL_TRIANGLE_STRIP);  // 第六面
  glColor3f(RED);
  glNormal3f(-1.0f, 0.0f, 0.0f);
  glVertex3f(-4.0f, 0.75f , -0.5f );
  glVertex3f(-4.0f, 0.25f , -0.5f );
  glVertex3f(-4.0f, 0.75f , 0.0f );
  glVertex3f(-4.0f, 0.25f , 0.0f );
  glVertex3f(-4.0f, 0.75f , 0.5f );
  glVertex3f(-4.0f, 0.25f , 0.5f );
  glEnd();

  glPopMatrix();
}

void drawrightwing() {

  glPushMatrix();
  // glTranslatef(g_x_move, g_up_move, g_temp); 
  glTranslatef(0.0f, g_up_move, g_foword_move);
  glRotatef(turn, 0.0f, 1.0f, 0.0f);
  if (!is_fly) 
    glRotatef(right_rotate_speed, 0.0f, 0.0f, 1.0f);
  else if ( !is_up) {
    right_rotate_speed = right_rotate_speed + ROTATE_SPEED;
    glRotatef(right_rotate_speed, 0.0f, 0.0f, 1.0f);
    if (right_rotate_speed >=  20.0) {
      is_up = true;
    } // if 
  } // else if 
   else if ( is_up) {
    right_rotate_speed = right_rotate_speed - ROTATE_SPEED;
    glRotatef(right_rotate_speed, 0.0f, 0.0f, 1.0f);
    if (right_rotate_speed <= -20.0) {
      is_up = false;
    } // if 
  } // else if 
  glBegin(GL_TRIANGLE_STRIP);  // 第一面
  glColor3f(RED);
  glNormal3f(0.0f, 1.0f, 0.0f);
  glVertex3f(4.0f, 0.75f , -0.5f );
  glVertex3f(0.0f, 0.75f , -0.5f );
  glVertex3f(4.0f, 0.75f , 0.5f );
  glVertex3f(0.0f, 0.75f , 0.5f );

  glEnd();
  glBegin(GL_TRIANGLE_STRIP);  // 第二面
  glColor3f(RED);
  glNormal3f(0.0f, -1.0f, 0.0f);
  glVertex3f(0.0f, 0.25f, -0.5f);
  glVertex3f(4.0f, 0.25f , -0.5f );
  glVertex3f(0.0f, 0.25f, 0.5f);
  glVertex3f(4.0f, 0.25f, 0.5f);
  glEnd();

  glBegin(GL_TRIANGLE_STRIP);  // 第三面
  glColor3f(RED);
  glNormal3f(0.0f, 0.0f, 1.0f);
  glVertex3f(0.0f, 0.75f , -0.5f );
  glVertex3f(4.0f, 0.75f , -0.5f );
  glVertex3f(0.0f, 0.25f , -0.5f );
  glVertex3f(4.0f, 0.25f , -0.5f );
  glEnd();

  glBegin(GL_TRIANGLE_STRIP);  // 第四面
  glColor3f(RED);
  glNormal3f(0.0f, 0.0f, -1.0f);
  glVertex3f(4.0f, 0.75f , 0.5f );
  glVertex3f(0.0f, 0.75f , 0.5f );
  glVertex3f(4.0f, 0.25f , 0.5f );
  glVertex3f(0.0f, 0.25f , 0.5f );
  glEnd();

  glBegin(GL_TRIANGLE_STRIP);  // 第五面
  glColor3f(RED);
  glNormal3f(1.0f, 0.0f, 0.0f);
  glVertex3f(4.0f, 0.25f , -0.5f);
  glVertex3f(4.0f, 0.75f , -0.5f );
  glVertex3f(4.0f, 0.25f , 0.5f );
  glVertex3f(4.0f, 0.75f , 0.5f );
  glEnd();

  glPopMatrix();

}

void drawwing() { 
  drawleftwing();
  drawrightwing();
}

 glm::vec3 get_vector(float front_x, float front_y, float front_z, float end_x, float end_y, float end_z) {
  glm::vec3 ans(end_x - front_x, end_y - front_y, end_z - front_z);
  return ans;
}
 void drawtail() {
  glPushMatrix();
   glTranslatef(0.0f, g_up_move, g_foword_move);
  glRotatef(turn, 0.0f, 1.0f, 0.0f);
  glm::vec3 a_vec;
  glm::vec3 b_vec;
  glm::vec3 n_vec;
  glm::vec3 pointA(A);
  glm::vec3 pointB(B);
  glm::vec3 pointC(C);
  glm::vec3 pointD(D);
  /*
  float A0 = pointA[0] * cos(ANGLE_TO_RADIAN(turn)) + (pointA[2] + g_foword_move)* sin(ANGLE_TO_RADIAN(turn));
  float A2 = pointA[0] * (-sin(ANGLE_TO_RADIAN(turn))) + (pointA[2] + g_foword_move) * cos(ANGLE_TO_RADIAN(turn));
  float B0 = pointB[0] * cos(ANGLE_TO_RADIAN(turn)) + (pointB[2] + g_foword_move) * sin(ANGLE_TO_RADIAN(turn));
  float B2 = pointB[0] * (-sin(ANGLE_TO_RADIAN(turn))) + (pointB[2] + g_foword_move) * cos(ANGLE_TO_RADIAN(turn));
  float C0 = pointC[0] * cos(ANGLE_TO_RADIAN(turn)) + (pointC[2] + g_foword_move) * sin(ANGLE_TO_RADIAN(turn));
  float C2 = pointC[0] * (-sin(ANGLE_TO_RADIAN(turn))) + (pointC[2] + g_foword_move) * cos(ANGLE_TO_RADIAN(turn));
  float D0 = pointD[0] * cos(ANGLE_TO_RADIAN(turn)) + (pointD[2] + g_foword_move) * sin(ANGLE_TO_RADIAN(turn));
  float D2 = pointD[0] * (-sin(ANGLE_TO_RADIAN(turn))) + (pointD[2] + g_foword_move) * cos(ANGLE_TO_RADIAN(turn));
  */
  glBegin(GL_TRIANGLES);
  glColor3f(GREEN);
  glNormal3f(0.0f, 1.0f, 0.0f);
  glVertex3f(pointA[0], pointA[1], pointA[2]);
  glVertex3f(pointB[0], pointB[1], pointB[2]);
  glVertex3f(pointC[0], pointC[1], pointC[2]);
  glEnd();

  glBegin(GL_TRIANGLES);
  glColor3f(GREEN);
  a_vec = get_vector(B, D);
  b_vec = get_vector(B, A);
  n_vec = glm::cross(a_vec, b_vec);
  glNormal3f(n_vec[0], n_vec[1], n_vec[2]);
  glVertex3f(pointA[0], pointA[1], pointA[2]);
  glVertex3f(pointD[0], pointD[1], pointD[2]);
  glVertex3f(pointB[0], pointB[1], pointB[2]);
  glEnd();

  glBegin(GL_TRIANGLES);
  glColor3f(GREEN);
  a_vec = get_vector(D, C);
  b_vec = get_vector(D, A);
  n_vec = glm::cross(a_vec, b_vec);
  glNormal3f(n_vec[0], n_vec[1], n_vec[2]);
  glVertex3f(pointA[0], pointA[1], pointA[2]);
  glVertex3f(pointC[0], pointC[1], pointC[2]);
  glVertex3f(pointD[0], pointD[1], pointD[2]);
  glEnd();

  glBegin(GL_TRIANGLES);
  glColor3f(GREEN);
  a_vec = get_vector(D, B);
  b_vec = get_vector(D, C);
  n_vec = glm::cross(a_vec, b_vec);
  glNormal3f(n_vec[0], n_vec[1], n_vec[2]);
  glVertex3f(pointB[0], pointB[1], pointB[2]);
  glVertex3f(pointD[0], pointD[1], pointD[2]);
  glVertex3f(pointC[0], pointC[1], pointC[2]);
  glEnd();
  glPopMatrix();
 }



/* TODO#2: Render body, wing, tail
 * Hint:
 *       glBegin/glEnd (https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/glBegin.xml)
 *       glColor3f (https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/glColor3f.xml)
 *       glVertex3f (https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/glVertex.xml)
 *       glNormal (https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/glNormal.xml)
 *       glScalef (https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/glScale.xml)
 * Note:
 *       You can refer to ppt "Draw Cylinder" page and `CIRCLE_SEGMENT`
 *       You can use color defined above
 */

void light() {
  GLfloat light_specular[] = {0.6, 0.6, 0.6, 1};
  GLfloat light_diffuse[] = {0.6, 0.6, 0.6, 1};
  GLfloat light_ambient[] = {0.4, 0.4, 0.4, 1};
  GLfloat light_position[] = {50.0, 75.0, 80.0, 1.0};
  // z buffer enable
  glEnable(GL_DEPTH_TEST);
  // enable lighting
  glEnable(GL_LIGHTING);
  glShadeModel(GL_SMOOTH);
  glEnable(GL_COLOR_MATERIAL);
  glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
  glEnable(GL_NORMALIZE);
  // set light property
  glEnable(GL_LIGHT0);
  glLightfv(GL_LIGHT0, GL_POSITION, light_position);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
  glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
}

int main() {
  is_up = false;
  left_is_up = false;
  float temp;
  int count = 0;
  initOpenGL();
  GLFWwindow* window = OpenGLContext::getWindow();

  // Init Camera helper
  Camera camera(glm::vec3(0, 5, 10));
  camera.initialize(OpenGLContext::getAspectRatio());
  // Store camera as glfw global variable for callbasks use
  glfwSetWindowUserPointer(window, &camera);

  // Main rendering loop
  while (!glfwWindowShouldClose(window)) {
    // Polling events.
    glfwPollEvents();
    // Update camera position and view
    camera.move(window);
    // GL_XXX_BIT can simply "OR" together to use.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    /// TO DO Enable DepthTest
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    // Projection Matrix
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(camera.getProjectionMatrix());
    // ModelView Matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(camera.getViewMatrix());


#ifndef DISABLE_LIGHT   
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glClearDepth(1.0f);
    light();
#endif

    is_fly = false;
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
      is_fly = true;
      if (g_up_move < 5.0)
        g_up_move = g_up_move + FLYING_SPEED;
      else {
        g_foword_move = g_foword_move - FLYING_SPEED;
      }  // else
    }    // if
    else {
      if ( g_up_move > 0.0 ) 
        g_up_move = g_up_move - (FLYING_SPEED/2);
    } // else 
    is_turn = false;
     if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
      if ( is_fly ) 
          g_foword_move = g_foword_move + FLYING_SPEED;
      turn = turn + ROTATE_SPEED;
      if (turn >= 360.0) 
          turn = 0.0; 
    }      // if 
    else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
       if (is_fly) 
           g_foword_move = g_foword_move + FLYING_SPEED;
      turn = turn - ROTATE_SPEED;
      if (turn <= -360.0) 
          turn = 0.0; 
     } // else if 
    /* TODO#4-2: Update 
     *       You may update position and orientation of airplane here or not.
     *       Feel free to not follow TA's structure. However, don't violate the spec. 
     * 
     * Hint: 
     * Note:
     *       You can use `ROTATE_SPEED` and `FLYING_SPEED` as the speed constant. 
     *       If the rotate/flying speed is too slow or too fast, please change `ROTATE_SPEED` or `FLYING_SPEED` value.
     *       You should finish keyCallback first.
     */

    // Render a white board
    glPushMatrix();
    glScalef(3, 1, 3);
    glBegin(GL_TRIANGLE_STRIP);
    glColor3f(1.0f, 1.0f, 1.0f);
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-5.0f, 0.0f, -5.0f);
    glVertex3f(-5.0f, 0.0f, 5.0f);
    glVertex3f(5.0f,  0.0f, -5.0f);
    glVertex3f(5.0f, 0.0f, 5.0f);
    glEnd();
    glPopMatrix();


   drawbody();
   drawwing();
   drawtail();
   g_x_move = 0.0;
   g_x_move = g_x_move * cos(ANGLE_TO_RADIAN(turn)) + g_foword_move * sin(ANGLE_TO_RADIAN(turn));
   g_temp = 0.0 * (-sin(ANGLE_TO_RADIAN(turn))) + g_foword_move * cos(ANGLE_TO_RADIAN(turn));
    /* TODO#3: Render the airplane    
     *       1. Render the body.
     *       2. Render the wings.(Don't forget to assure wings rotate at the center of body.)
     *       3. Render the tail.
     * Hint:
     *       glPushMatrix/glPopMatrix (https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/glPushMatrix.xml)
     *       glRotatef (https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/glRotate.xml)
     *       glTranslatef (https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/glTranslate.xml) 
     *       glColor3f (https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/glColor.xml)
     *       glScalef (https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/glScale.xml)
     * Note:
     *       You may implement functions for drawing components of airplane first
     *       You should try and think carefully about changing the order of rotate and translate
     */
    

#ifdef __APPLE__
    // Some platform need explicit glFlush
    glFlush();
#endif
    glfwSwapBuffers(window);
  }
  return 0;
}
