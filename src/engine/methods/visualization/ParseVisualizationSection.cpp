#ifndef ANT_CONFIG_H
#include "config.h"
#endif

#if ANT_HAS_VIS
#include "../../../antvis/axis3d.h"
#include "../../../antvis/axis2d.h"
#include "../../../antvis/window2d.h"
#include "../../../antvis/mesh2d.h"
#include "../../../antvis/mesh3d.h"
#include "../../../antvis/text.h"

#include "../../../antvis/samples.cpp"
#endif

#include "ParseVisualizationSection.hpp"
#include <cstdio>

// #include "../../../utils/debug/Oops.hpp"
#define debug(s)
// #define debug(s) oops(s)


// window type corresponds to the dimensions
const int ANTVIS_3D_WINDOW = 3;
const int ANTVIS_2D_WINDOW = 2;
const int ANTVIS_TL_WINDOW = 1;

char STRBUFF[101];

#if ANT_HAS_VIS

//------------------------------ Forward declaration --------------------------------------

void addObjectsToScene (Configuration& objectDescription, 
			TWindow* win, 
			TScene* scene, 
			int windowType, 
			TArrayBuffer* buffer);


//--------------------------------  Implementation --------------------------------------------


void getNumberOfPlottingMethod(Configuration& db, int& plottingMethod)
{
  debug("getNumberOfPlottingMethod");
  if (db.checkForEnumValue("PLOTTING_METHOD_KEY","PLOTTING_METHOD_POINTS_KEY"))
    {
      plottingMethod = 1;
    }
  else if (db.checkForEnumValue("PLOTTING_METHOD_KEY","PLOTTING_METHOD_LINE_KEY"))
    plottingMethod = 0;
}

/**
 * a simple check, whether the given index of a variable
 * is between zero and stateSpaceDim.
 * If the check failed, Error::Exit will be called.
 * @param index index of a variable
 */
void checkVariableIndex (int index, TArrayBuffer* buffer)
{
  if ( (index < -1 ) ||
       (index >=  buffer->getColCount()) )
    cerr << "ERROR: ParseVisualizationSection::checkVariableIndex():  "
 	 << "Do you really want to plot the"
 	 << index
 	 << "-th variable? Only values between -1 and "
 	 << buffer->getColCount() - 1
 	 << " admissible for this field."
 	 << endl << Error::Exit;
}


/**
 * a simple check, whether the given array can be interpreted
 * as a color description. It must contain three values between
 * 0.0 and 1.0.
 */

void
checkColor (Array<real_t>& color)
{
  if (color.getTotalSize () != 3)
    cerr << "ParseVisualizationConnector.cpp internal error: "
	 << "wrong usage of 'checkColor' routine"
	 << endl << Error::Exit;

  for (int i = 0; i < 3; ++i)
    if ( (color[i] < 0.0) ||
	 (color[i] > 1.0) )
      cerr << "ParseVisualizationConnector.cpp error: "
	   << "wrong value ("
	   << color[i]
	   << ") of the ("
	   << i+1
	   << ")-th component for a color description. "
	   << "only values between 0.0 and 1.0 are admissible."
	   << endl << Error::Exit;
}

void getFloatsFromDB(Configuration& db, const string& name, float* f, int dims)
{
  debug(name);
  assert (! name.empty ());
  if (db.checkForKey (name))
    {
      Array<real_t> vec (dims);
      db.getArray(name, vec);
      for(int i=0;i<dims;i++) f[i] = vec[i];
    }
}

void getVector3FromDB(Configuration& db, const string& name, TVec3& v3)
{
  debug(name);
  assert (! name.empty ());
  if (db.checkForKey (name) )
    {
      Array<real_t> vec3real (3);
      db.getArray(name, vec3real);
      v3[0] = vec3real[0];
      v3[1] = vec3real[1];
      v3[2] = vec3real[2];
    }
}

void getVector2FromDB(Configuration& db, const string& name, TVec2& v2)
{
  debug(name);
  assert (! name.empty ());
  if (db.checkForKey (name) )
    {
      Array<real_t> vec2real (2);
      db.getArray(name, vec2real);
      v2[0] = vec2real[0];
      v2[1] = vec2real[1];
    }
}

void getIntVector2FromDB(Configuration& db, const string& name, int* v2)
{
  debug(name);
  assert (! name.empty ());
  if (db.checkForKey (name) )
    {
      Array<long> vec2int (2);
      db.getArray(name, vec2int);
      v2[0] = vec2int[0];
      v2[1] = vec2int[1];
    }
}

void getIntVector3FromDB(Configuration& db, const string& name, int* v3)
{
  debug(name);
  assert (! name.empty ());
  if (db.checkForKey (name) )
    {
      Array<long> vec3int (3);
      db.getArray(name, vec3int);
      v3[0] = vec3int[0];
      v3[1] = vec3int[1];
      v3[2] = vec3int[2];
    }
}



void getColorFromDB(Configuration& db, const string& name, TVec3& color)
{
  debug(name);
  assert (! name.empty ());
  if (db.checkForKey (name) )
    {
      Array<real_t> myColor (3);
      db.getArray(name, myColor);
      checkColor(myColor);
      color[0] = myColor[0];
      color[1] = myColor[1];
      color[2] = myColor[2];
    }
}

void getFloatFromDB(Configuration& db, const string& name, float& value)
{
  debug(name);
  assert (! name.empty ());
  if (db.checkForKey (name) )
    {
      value = db.getReal(name);
    }
}

void getIntFromDB(Configuration& db, const string& name, int& value)
{
  debug(name);
  assert (! name.empty ());
  if (db.checkForKey (name) )
    {
      value = db.getInteger(name);
    }
}

void getBoolFromDB(Configuration& db, const string& name, int& value)
{
  debug(name);
  assert (! name.empty ());
  if (db.checkForKey (name) )
    {
      value = db.getBool(name);
    }
}

string // RL;VA
getStringFromDB(Configuration& db, const string& name)
{
  debug(name);
  assert (! name.empty ());
  if (db.checkForKey (name)) {
    return db.getString (name);
  }
  else {
    return "";
  }
}

void getAlignFromDB(Configuration& db, int& align)
{
  debug("getAlignFromDB");
  if (db.checkForKey ("ALIGN_KEY"))
    {
      if (db.checkForEnumValue("ALIGN_KEY","LEFT_KEY")) align=0;
      else
	if (db.checkForEnumValue("ALIGN_KEY","CENTER_KEY")) align=1;
	else
	  if (db.checkForEnumValue("ALIGN_KEY","RIGH_KEY")) align=2;
    }
  else align = 0;
}

void getFontFromDB(Configuration& db, TText* text, float& relativeFontSize)
{
  debug("getFontFromDB");
  if (db.checkForKey("FONT_KEY"))
    {
      Configuration font = db.getSubConfiguration("FONT_KEY");
      getColorFromDB(font,"COLOR_KEY",text->color);
      getFloatFromDB(font,"SIZE_KEY", relativeFontSize);
      getFloatFromDB(font,"BOLDNESS_KEY",text->bold);
      getBoolFromDB(font,"SMOOTH_KEY",text->smoothing);
    }
}


void getVariablesFromDB(Configuration& db, int* vars, int dims, TArrayBuffer* buffer)
{
  //TODO:check the vars here
  debug("getVariablesFromDB");
  assert(dims>=1);
  assert(dims<=3);
  if (db.checkForKey("VARIABLES_KEY"))
    {
      Array<long> avars (dims);
      db.getArray("VARIABLES_KEY", avars);
      for(int i=0;i<dims;i++) vars[i] = avars[i];
    }
}

void getPositioner(Configuration& db1, TPositioner* posi, int dims, TArrayBuffer* buffer)
{
  debug("getPositioner");
  if (!db1.checkForKey("POSITION_KEY")) return;

  Configuration db = db1.getSubConfiguration("POSITION_KEY");

  if ((dims<1) || (dims>3)) cout << " dims=" << dims << endl;
  assert(dims>=1);
  assert(dims<=3);

  int vars[3];

  if (db.checkForKey("VARIABLES_KEY"))
    {
      getVariablesFromDB(db, vars, dims, buffer);
      posi->setVariables(buffer, vars,dims);
    }
  getFloatsFromDB (db, "OFFSET_KEY", &(posi->position[0]), dims);
  getIntFromDB (db, "TIME_SHIFT_KEY", posi->timeShift);
}


void loadSolidObjectFromConfiguration(Configuration& db, TGlutSolid* obj,
				      TWindow* win, int dims, TArrayBuffer* buffer)

{
  getColorFromDB(db,"COLOR_KEY", obj->color);
  getPositioner(db,obj->positioner, dims, buffer);
  getVector3FromDB(db,"SCALE_KEY", obj->scale);
  getBoolFromDB(db,"LIGHTING_KEY", obj->lighting);
}


TVisualObject* createSphere (Configuration& db,  TWindow* win, int dims, TArrayBuffer* buffer)
{
  TSolidSphere* sphere  = new TSolidSphere(1.0);
  loadSolidObjectFromConfiguration(db, sphere, win, dims, buffer);
  getFloatFromDB(db,"RADIUS_KEY", sphere->radius);
  return sphere;
}



TVisualObject* createSphereMarker (Configuration& db,  TWindow* win, int dims, TArrayBuffer* buffer)
{
  TSphereMarker* sphere  = new TSphereMarker(1.0, win);
  loadSolidObjectFromConfiguration(db, sphere, win, dims, buffer);
  getFloatFromDB(db,"RADIUS_KEY", sphere->relativeRadius);
  return sphere;
}


TVisualObject* createCurve(Configuration& curveDescription,  TWindow* win, int windowType, TArrayBuffer* buffer)
{
  //cout << "---------------------------------- curve ---------------------------" << endl;
  Array<long> variables;
  int vars;
  switch (windowType)
    {
    case ANTVIS_3D_WINDOW : vars = 3; break;
    case ANTVIS_2D_WINDOW : vars = 2; break;
    case ANTVIS_TL_WINDOW : vars = 1; break;
    }
  variables.alloc(vars);
 
  curveDescription.getArray ("VARIABLES_KEY", variables);


  // zero column is always time
  TCurve* curve;
  

  switch (windowType)
    {
    case ANTVIS_3D_WINDOW :
      {
	curve = new TCurve3D(buffer,win,variables[0]+1,variables[1]+1,variables[2]+1);
      }
      break;
    case ANTVIS_2D_WINDOW :
      {
	curve = new TCurve2D(buffer,win,variables[0]+1,variables[1]+1);
      }
      break;
    case ANTVIS_TL_WINDOW :
      {
	curve = new TCurve2D(buffer,win, 0,variables[0]+1);
      }
      break;
    }

  //kramerar  25.02.2005
  getVector3FromDB(curveDescription,"SCALE_KEY", curve->scale);
  
  
  if (curveDescription.checkForEnumValue ("COLORING_METHOD_KEY",
					 "COLORING_METHOD_FIXED_KEY"))
    {
      curve->setColoringMethod(1);
    }
  else
    if (curveDescription.checkForEnumValue ("COLORING_METHOD_KEY",
					   "COLORING_METHOD_VELOCITY_KEY"))
      {
	curve->setColoringMethod(0);
	((TColoringMethod3D*)curve->coloringMethods->get(0))->setType(0);
      }
    else
      if (curveDescription.checkForEnumValue ("COLORING_METHOD_KEY",
					     "COLORING_METHOD_ACCELERATION_KEY"))
	{
	  curve->setColoringMethod(0);
	  ((TColoringMethod3D*)curve->coloringMethods->get(0))->setType(1);
	}
      else
	if (curveDescription.checkForEnumValue("COLORING_METHOD_KEY","COLORING_METHOD_POSITION_KEY"))
	  {
	    curve->setColoringMethod(0);
	    ((TColoringMethod3D*)curve->coloringMethods->get(0))->setType(2);
	  }



  int plottingMethod = 0;
  getNumberOfPlottingMethod(curveDescription, plottingMethod);
  curve->setDrawMethod(plottingMethod);
  
  //method 0 = autocolor, method 1 = fixed color
  TColoringMethodFixed* methodfixed = (TColoringMethodFixed*)curve->coloringMethods->get(1);
  getColorFromDB(curveDescription,"COLOR_KEY",methodfixed->color);

  return curve;

}

void getObjectsName(Configuration& db, TVisualObject* obj)
{
  string name = getStringFromDB(db, "NAME_KEY");
  if (! name.empty ()) obj->setName(name);
}

void getObjectsVisibility(Configuration& db, TVisualObject* obj)
{
  getBoolFromDB(db, "VISIBLE_KEY", obj->visible);
}



void createAxis3D (Configuration& windowDescription,TWindow* win)
{
  debug("createAxis3D");
  if (windowDescription.checkForKey ("AXES_KEY"))
    {
      Configuration axesdb = windowDescription.getSubConfiguration("AXES_KEY");
      TAxis3D* axis = new TAxis3D((TWindow3D*)win);
      if (axesdb.checkForKey("STEPPING_KEY")) axis->autostep = false;
      getIntVector3FromDB(axesdb,"STEPPING_KEY", axis->steps);
      getColorFromDB(     axesdb,"COLOR_KEY", axis->color);
      getFontFromDB(      axesdb,axis->text,axis->relativeFontSize);
      string cap = getStringFromDB(axesdb, "X_LABEL_KEY");
      if (! cap.empty ()) axis->setCaptionX(cap);
      cap = getStringFromDB(axesdb, "Y_LABEL_KEY");
      if (! cap.empty ()) axis->setCaptionY(cap);
      cap = getStringFromDB(axesdb, "Z_LABEL_KEY");
      if (! cap.empty ()) axis->setCaptionZ(cap);

      win->add(axis);
    }

}


void createMesh3D (Configuration& windowDescription, TWindow* win)
{
  debug("createMesh3D");
  if (windowDescription.checkForKey ("MESH_KEY"))
    {
      assert (dynamic_cast<TWindow3D*> (win));
      TMesh3D *mesh = new TMesh3D((TWindow3D*)win);
      Configuration db = windowDescription.getSubConfiguration("MESH_KEY");
      if (db.checkForKey("STEPPING_KEY")) mesh->autostep = false;
      getIntVector3FromDB(db, "STEPPING_KEY", mesh->steps);
      getColorFromDB(     db, "COLOR_KEY",    mesh->color);
      getFontFromDB(db,mesh->text,mesh->relativeFontSize);
      string cap = getStringFromDB(db, "X_LABEL_KEY");
      if (! cap.empty ()) mesh->setCaptionX(cap);
      cap = getStringFromDB(db, "Y_LABEL_KEY");
      if (! cap.empty ()) mesh->setCaptionY(cap);
      cap = getStringFromDB(db, "Z_LABEL_KEY");
      if (! cap.empty ()) mesh->setCaptionZ(cap);
      win->add(mesh);
    }
}

void createMesh2D (Configuration& windowDescription, TWindow* win)
{
  debug("createMesh2D");
  if (windowDescription.checkForKey ("MESH_KEY"))
    {
      TMesh2D *mesh = new TMesh2D((TWindow2D*)win);
      Configuration db = windowDescription.getSubConfiguration("MESH_KEY");
      if (db.checkForKey("STEPPING_KEY")) mesh->autostep = false;
      getIntVector2FromDB(db, "STEPPING_KEY", mesh->steps);
      getColorFromDB     (db, "COLOR_KEY",    mesh->color);
      getFontFromDB(db,mesh->text,mesh->relativeFontSize);

      string cap = getStringFromDB(db, "X_LABEL_KEY");
      if (! cap.empty ()) mesh->setCaptionX(cap);
      cap = getStringFromDB(db, "Y_LABEL_KEY");
      if (! cap.empty ()) mesh->setCaptionY(cap);

      win->add(mesh);
    }
}

void createAxis2D (Configuration& windowDescription, TWindow* win)
{
  debug("createAxis2D");
  if (windowDescription.checkForKey ("AXES_KEY"))
    {
      TAxis2D *axis = new TAxis2D((TWindow2D*)win);
      Configuration db = windowDescription.getSubConfiguration("AXES_KEY");
      if (db.checkForKey("STEPPING_KEY")) axis->autostep = false;
      getIntVector2FromDB(db, "STEPPING_KEY", axis->steps);
      getColorFromDB     (db, "COLOR_KEY",    axis->color);
      getFontFromDB(db,axis->text,axis->relativeFontSize);

      win->add(axis);
    }
}


TVisualObject* createText(Configuration& objectDescription,  TWindow* win, int windowType, TArrayBuffer* buffer)
{
  debug("createText");
  TVariableText* text = new TVariableText();
  float relativeFontSize = 1.5;
  getFontFromDB(objectDescription, text,relativeFontSize);
  getPositioner(objectDescription,text->positioner,windowType,buffer);
  TVec3 pos;
  TCamera3D* camera = 0;
  if (objectDescription.getBool("FACING_KEY")) {
    if (windowType==ANTVIS_3D_WINDOW) camera=((TWindow3D*)win)->camera;
  }
  text->textRef = "";
  text->set(pos,camera,relativeFontSize);
  string caption = getStringFromDB(objectDescription,"TEXT_KEY");
  if (! caption.empty ()) text->setText(caption);
  int align = 1;
  getAlignFromDB(objectDescription,align);
  text->buffer = buffer;
  int var = 0;
  getIntFromDB(objectDescription,"VARIABLE_KEY",var);
  text->setVariable(var);
  text->align=align;
  return text;
}

TVisualObject* createLine(Configuration& objectDescription,  TWindow* win, int windowType, TArrayBuffer* buffer)
{
  TLine* line = new TLine();
  getPositioner(objectDescription,line->positioner,windowType,buffer);
  getColorFromDB(objectDescription,"COLOR_KEY",line->color);
  getVector3FromDB(objectDescription,"VECTOR_KEY",line->vector);
  return line;
}

TVisualObject* createArrow(Configuration& objectDescription,  TWindow* win, int windowType, TArrayBuffer* buffer)
{
  TArrow* arrow = new TArrow();
  arrow->dims = win->dimensions;
  getPositioner(objectDescription,arrow->positioner,windowType,buffer);
  getColorFromDB(objectDescription,"COLOR_KEY",arrow->color);
  getVector3FromDB(objectDescription,"VECTOR_KEY",arrow->vector);
  return arrow;
}


TVisualObject* createPlane(Configuration& objectDescription,  TWindow* win, int windowType, TArrayBuffer* buffer)
{
  TPlane* plane = new TPlane();
  getPositioner(objectDescription,plane->positioner,windowType,buffer);
  getFloatFromDB(objectDescription,"ALPHA_KEY",plane->alpha);
  getColorFromDB(objectDescription,"COLOR_KEY",plane->color);
  getVector3FromDB(objectDescription,"VECTOR_KEY",plane->vector);
  getVector3FromDB(objectDescription,"VECTOR2_KEY",plane->vector2);
  return plane;
}

TVisualObject* createScene(Configuration& objectDescription,  TWindow* win, TScene* scene, int windowType, TArrayBuffer* buffer)
{
  //   cout << "-------------------- create scene ------------------------" << endl;
  TPositionableScene* myscene = new TPositionableScene();
  getPositioner(objectDescription, myscene->positioner,windowType,buffer);
  //ak 26.02.2005
  getVector3FromDB(objectDescription,"SCALE_KEY", myscene->scale);

  addObjectsToScene(objectDescription,win, myscene,windowType, buffer);
  return myscene;
}

void addObjectsToScene(Configuration& objectDescriptionParent, TWindow* win, TScene* scene, int windowTypeID, TArrayBuffer* buffer)
{
  debug("addObjectsToScene");
  int numberOfObjects = 0;
  getIntFromDB(objectDescriptionParent,"NUMBER_OF_OBJECTS_KEY",numberOfObjects);
  TVisualObject *obj;
  for (int objectIndex = 0; objectIndex < numberOfObjects; objectIndex++)
    {
      sprintf(STRBUFF,"OBJECT[%d]",objectIndex);
      Configuration objConf =  objectDescriptionParent.getSubConfiguration(STRBUFF);

      debug(STRBUFF);

      bool objectOK = objConf.checkForKey("OBJECT_TYPE_KEY");

      if (objConf.checkForEnumValue("OBJECT_TYPE_KEY","CURVE_KEY"))
	{
	  obj = createCurve(objConf, win, windowTypeID, buffer);
	}
      else  if (objConf.checkForEnumValue("OBJECT_TYPE_KEY","SPHERE_KEY"))
	{
	  obj = createSphere(objConf, win, windowTypeID, buffer);
	}
      else  if (objConf.checkForEnumValue("OBJECT_TYPE_KEY","TEXT_KEY"))
	{
	  obj = createText(objConf, win, windowTypeID, buffer);
	}
      else  if (objConf.checkForEnumValue("OBJECT_TYPE_KEY","LINE_KEY"))
	{
	  obj = createLine(objConf, win, windowTypeID, buffer);
	}
      else  if (objConf.checkForEnumValue("OBJECT_TYPE_KEY","ARROW_KEY"))
	{
	  obj = createArrow(objConf, win, windowTypeID, buffer);
	}
      else  if (objConf.checkForEnumValue("OBJECT_TYPE_KEY","PLANE_KEY"))
	{
	  obj = createPlane(objConf, win, windowTypeID, buffer);
	}
      else  if (objConf.checkForEnumValue("OBJECT_TYPE_KEY","SCENE_KEY"))
	{
	  obj = createScene(objConf, win, scene, windowTypeID, buffer);
	}
      else  if (objConf.checkForEnumValue("OBJECT_TYPE_KEY","CUBE_KEY"))
	{
	  obj = new TSolidCube();
	  loadSolidObjectFromConfiguration(objConf,(TGlutSolid*)obj,win, windowTypeID, buffer);
	}
      else  if (objConf.checkForEnumValue("OBJECT_TYPE_KEY","TORUS_KEY"))
	{
	  TSolidTorus* torus = new TSolidTorus();
	  loadSolidObjectFromConfiguration(objConf,torus,win, windowTypeID, buffer);
	  getFloatFromDB(objConf,"RADIUS_KEY",torus->outerRadius);
	  getFloatFromDB(objConf,"INNER_RADIUS_KEY", torus->innerRadius);
	  obj = torus;
	}

      else  if (objConf.checkForEnumValue("OBJECT_TYPE_KEY","MARKER_KEY"))
	{
	  obj = createSphereMarker(objConf, win, windowTypeID, buffer);
	}
      else  if (objConf.checkForEnumValue("OBJECT_TYPE_KEY","SURFACE_KEY"))
	{
	  TAntSurface* surf =   new TAntSurface(buffer, win);
	  getVector3FromDB(objConf,"SCALE_KEY", surf->scale);
	  win->infoscene->add(surf->gradient);
	  obj = (TVisualObject*) surf;
	}
      else
	objectOK = false;


      if (objectOK)
	{
	  getObjectsName(objConf, obj);
	  getObjectsVisibility(objConf, obj);
	  scene->add(obj);
	}
      else
	{
	  cerr << "addObjectsToScene(): unrecognized value:"
	       << objConf.getEnum("OBJECT_TYPE_KEY")
	       << " = " 
	       << objConf.getOriginalKey("OBJECT_KEY") 
	       << ".  Object will be skiped" << endl;
	}

    }
}


void addObjectsToWindow(Configuration& windowDescription, int windowTypeID, TWindow* win,
			TArrayBuffer* buffer)
{
  debug("addObjectsToWindow");
  switch (windowTypeID)
    {
    case ANTVIS_3D_WINDOW :
      createMesh3D(windowDescription, win);
      createAxis3D(windowDescription, win);
      break;
    case ANTVIS_2D_WINDOW: case ANTVIS_TL_WINDOW:
      createMesh2D(windowDescription, win);
      createAxis2D(windowDescription, win);
      break;

    }

  addObjectsToScene(windowDescription, win, win->scene,windowTypeID, buffer);

  if (windowDescription.checkForKey("FIXED_SCENE_KEY")) 
    {
      Configuration infoscene_conf = windowDescription.getSubConfiguration("FIXED_SCENE_KEY");
      addObjectsToScene(infoscene_conf, win, win->infoscene, windowTypeID,buffer);
    }
}


void     addTimeTextToScene(TScene* scene, TArrayBuffer* buffer)
{
  debug("addTimeTextToScene");
  const float size = 0.06;
  TVariableText* text1 = new TVariableText();
  text1->size = size;
  text1->setText("Time start %g");
  text1->align = ALIGN_RIGHT;
  text1->timeShift = 1;
  text1->positioner->position.set(0.93,-1.0+1.5*size,0.0);
  text1->buffer = buffer;
  text1->setVariable(-1);
  scene->add(text1);

  TVariableText* text2 = new TVariableText();
  text2->size = size;
  text2->setText("Time end %g");
  text2->align = ALIGN_RIGHT;
  text2->timeShift = 0;
  text2->positioner->position.set(0.93,-1.0+0.2*size,0.0);
  text2->buffer = buffer;
  text2->setVariable(-1);
  scene->add(text2);

}


TWindow* createWindow (Configuration& windowDescription,
		       int windowTypeID, TArrayBuffer* buffer)
{
  // window width:
  int width;
  int height;
  // window height


  debug("enter createWindow");


  width = windowDescription.getInteger ("WIDTH_KEY");
  height = windowDescription.getInteger ("HEIGHT_KEY");

  debug("getting achor");
  int anchor[2];
  anchor[0] = 200;
  anchor[1] = 0;
  getIntVector2FromDB(windowDescription,"SCREEN_ANCHOR_KEY",anchor);

  debug("getting title");
  string title = getStringFromDB(windowDescription,"TITLE_KEY");
  if (title.empty ()) title = "Window";

  TWindow* win;
  switch (windowTypeID)
    {
    case ANTVIS_3D_WINDOW :
      win = new TWindow3D(title,anchor[0], anchor[1], width, height);
      break;
    case ANTVIS_2D_WINDOW:
    case ANTVIS_TL_WINDOW :
      win = new TWindow2D(title,anchor[0], anchor[1], width, height);
      break;
    }
   
  getColorFromDB(windowDescription,"BACKGROUND_COLOR_KEY",win->bgColor);
  addTimeTextToScene(win->infoscene, buffer);
  addObjectsToWindow(windowDescription, windowTypeID, win, buffer);
   
  return win;
   
}

/***********************************************************************************************
 *
 */

TArrayBuffer* parseVisualizationSection(TVisualization* vis, Configuration& ini, int bufferColumns)
{

  int buffer_size = 1000;
  getIntFromDB(ini, "BUFFER_SIZE_KEY",buffer_size);

  TArrayBuffer* buffer = new TArrayBuffer(buffer_size, bufferColumns);

  buffer->setName("Main Antvis Buffer");
  cout << "buffer_size=" << buffer_size << endl;
  int windowTypeID;

  int windowsNumber = ini.getInteger ("WINDOWS_NUMBER_KEY");
  for (int windowIndex = 0; windowIndex < windowsNumber; ++windowIndex)
    {
      sprintf(STRBUFF,"WINDOW[%d]",windowIndex);
      cout << "creating " << STRBUFF << endl;
      Configuration windowDescription = ini.getSubConfiguration(STRBUFF);


      windowTypeID = -1;
      if (windowDescription.checkForEnumValue("WINDOW_TYPE_KEY","WIN1D_KEY")) windowTypeID=ANTVIS_TL_WINDOW;
      else if (windowDescription.checkForEnumValue("WINDOW_TYPE_KEY","WIN2D_KEY")) windowTypeID=ANTVIS_2D_WINDOW;
      else if (windowDescription.checkForEnumValue("WINDOW_TYPE_KEY","WIN3D_KEY")) windowTypeID=ANTVIS_3D_WINDOW;
      else cerr << "Window type "
		<< windowDescription.getOriginalKey("WINDOW_TYPE_KEY")
		<< " not recognized" << endl;

      if (windowTypeID != -1) createWindow (windowDescription, windowTypeID, buffer);

    } // for (int windowIndex...)
  return buffer;

}

#endif
