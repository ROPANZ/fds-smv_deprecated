#include "options.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include GLUT_H

#include "update.h"
#include "smokeviewvars.h"
#include "viewports.h"


void drawsphere(float diameter, unsigned char *rgbcolor);
void ShowScene2(int mode, int view_mode, int quad, GLint s_left, GLint s_down);

/* ------------------ ShowScene ------------------------ */

void ShowScene(int mode, int view_mode, int quad, GLint s_left, GLint s_down, screendata *screen){
  CheckMemory;

  LOCK_IBLANK
  show_mode = mode;

  UNCLIP;

  /* ++++++++++++++++++++++++ update variables as needed +++++++++++++++++++++++++ */

  UpdateShowScene();
  if(stereotype == STEREO_NONE || stereotype == STEREO_TIME)ClearBuffers(mode);

  /* ++++++++++++++++++++++++ setup viewports +++++++++++++++++++++++++ */

  if(mode == DRAWSCENE){
    GetViewportInfo();

    if(clip_rendered_scene == 1){
      ViewportClip(quad, s_left, s_down);
      SNIFF_ERRORS("after ViewportClip");
    }

    if(VP_info.doit == 1){
      ViewportInfo(quad, s_left, s_down);
      SNIFF_ERRORS("after ViewportInfo");
    }

    if(VP_timebar.doit == 1){
      ViewportTimebar(quad, s_left, s_down);
      SNIFF_ERRORS("after ViewportTimebar");
    }

    if(VP_colorbar.doit == 1){
      ViewportColorbar(quad, s_left, s_down);
      SNIFF_ERRORS("after ViewportColorbar");
    }

    if(VP_title.doit == 1){
      ViewportTitle(quad, s_left, s_down);
      SNIFF_ERRORS("after ViewportTitle");
    }

    ViewportScene(quad, view_mode, s_left, s_down, screen);
    SNIFF_ERRORS("after ViewportScene");
  }



  /* ++++++++++++++++++++++++ draw "fancy" colorbar +++++++++++++++++++++++++ */

  if(viscolorbarpath == 1){
    if(colorbar_hidescene == 1)UNCLIP;
    DrawColorbarPath();
    SNIFF_ERRORS("after setColorbarClipPlanes 1");
  }
  if(viscolorbarpath==0||colorbar_hidescene==0)ShowScene2(mode, view_mode, quad, s_left, s_down);

/* ++++++++++++++++++++++++ render scene +++++++++++++++++++++++++ */
// if rendering is not working remove following comment
// then determine where Render should have been called
//  Render(view_mode);

  SNIFF_ERRORS("end of ShowScene");
  UNLOCK_IBLANK
}

  /* ------------------ ShowScene2 ------------------------ */

void ShowScene2(int mode, int view_mode, int quad, GLint s_left, GLint s_down){
  if(rotation_type==EYE_CENTERED&&nskyboxinfo>0)draw_skybox();

  if(UpdateLIGHTS==1)UpdateLights(light_position0,light_position1);

  if(mode==DRAWSCENE){
    glPointSize((float)1.0);


    /* ++++++++++++++++++++++++ draw north  +++++++++++++++++++++++++ */

    if(vis_northangle==1){
      CLIP_GEOMETRY;
      drawnorth();
      SNIFF_ERRORS("after drawnorth");
    }

    /* ++++++++++++++++++++++++ draw trees +++++++++++++++++++++++++ */

    if(ntreeinfo>0){
      CLIP_GEOMETRY;
      drawtrees();
      SNIFF_ERRORS("after drawtrees");
    }

/* ++++++++++++++++++++++++ draw particles +++++++++++++++++++++++++ */

    if(showsmoke==1){
      CLIP_VALS;
      draw_partframe();
    }

/* ++++++++++++++++++++++++ draw evacuation +++++++++++++++++++++++++ */

    if(showevac==1){
      CLIP_VALS;
      draw_evacframe();
    }

/* ++++++++++++++++++++++++ draw test geometry +++++++++++++++++++++++++ */

#ifdef pp_GEOMTEST
    if(geomtest_option == TETRAHEDRON_TEST){
      CLIP_GEOMETRY;
      draw_test_clip();
      draw_test_outline();
    }
    if(show_cutcells==1)draw_geom_cutcells();
    if(geomtest_option == TRIANGLE_TEST){
      CLIP_GEOMETRY;
      draw_test_triangle();
    }
    if(geomtest_option == POLYGON_TEST){
      CLIP_GEOMETRY;
      draw_test_polygon();
    }
#endif

    /* ++++++++++++++++++++++++ draw screeninfo +++++++++++++++++++++++++ */

#ifdef pp_RENDER360_DEBUG
    if(screenview == 1)draw_screeninfo();
#endif

/* ++++++++++++++++++++++++ draw circular vents +++++++++++++++++++++++++ */

    if(ncvents>0&&visCircularVents!=VENT_HIDE){
      CLIP_GEOMETRY;
      DrawCircVents(visCircularVents);
    }

/* ++++++++++++++++++++++++ draw sensors/sprinklers/heat detectors +++++++++++++++++++++++++ */

    CLIP_GEOMETRY;
    draw_devices();
#ifdef pp_PILOT
    draw_pilot();
#endif
    SNIFF_ERRORS("after draw_devices");

    if(visaxislabels==1){
      UNCLIP;
      OutputAxisLabels();
      SNIFF_ERRORS("after outputAxisLables");
    }


 /* ++++++++++++++++++++++++ draw user ticks +++++++++++++++++++++++++ */

    if(visUSERticks==1){
      Antialias(ON);
      UNCLIP;
      DrawUserTicks();
      Antialias(OFF);
      SNIFF_ERRORS("after DrawTicks");
    }

 /* ++++++++++++++++++++++++ draw ticks +++++++++++++++++++++++++ */

    if(visFDSticks==1&&ntickinfo>0){
      UNCLIP;
      DrawTicks();
      SNIFF_ERRORS("after DrawTicks");
    }

    /* ++++++++++++++++++++++++ draw ticks +++++++++++++++++++++++++ */

    if(showgravity==1){
      UNCLIP;
      DrawGravityAxis();
      SNIFF_ERRORS("after drawaxis");
    }

    /* draw the box framing the simulation (corners at (0,0,0) (xbar,ybar,zbar) */


/* ++++++++++++++++++++++++ draw simulation frame (corners at (0,0,0) and (xbar,ybar,zbar) +++++++++++++++++++++++++ */

    if(isZoneFireModel==0&&visFrame==1&&highlight_flag==2){
      CLIP_GEOMETRY;
      DrawOutlines();
      SNIFF_ERRORS("after DrawOutlines");
    }

    if(show_rotation_center==1){
      unsigned char pcolor[4];

      CLIP_GEOMETRY;
      glPushMatrix();
      glTranslatef(camera_current->xcen,camera_current->ycen,camera_current->zcen);
      pcolor[0]=255*foregroundcolor[0];
      pcolor[1]=255*foregroundcolor[1];
      pcolor[2]=255*foregroundcolor[2];
      drawsphere(0.03,pcolor);
      glPopMatrix();
    }


/* ++++++++++++++++++++++++ draw mesh +++++++++++++++++++++++++ */

    if(setPDIM==1){
      if(visGrid!=noGridnoProbe){
        int igrid;
        meshdata *meshi;

        UNCLIP;
        for(igrid=0;igrid<nmeshes;igrid++){
          meshi=meshinfo+igrid;
          drawgrid(meshi);
          SNIFF_ERRORS("drawgrid");
        }
      }
    }
  } /* end of if(mode==DRAWSCENE) code segment */


/* ++++++++++++++++++++++++ draw selected devices +++++++++++++++++++++++++ */

  if(mode==SELECTOBJECT){
    if(select_device==1){
      CLIP_GEOMETRY;
      draw_devices();
      SNIFF_ERRORS("after drawselect_devices");
      return;
    }
  }

/* ++++++++++++++++++++++++ draw selected avatars +++++++++++++++++++++++++ */

  if(mode==SELECTOBJECT){
    if(select_avatar==1){
      CLIP_GEOMETRY;
      draw_select_avatars();
      SNIFF_ERRORS("after draw_select_avatars");
      return;
    }
  }

/* ++++++++++++++++++++++++ draw selected tours +++++++++++++++++++++++++ */

  if(mode==SELECTOBJECT){
    if(edittour==1&&ntours>0){
      CLIP_GEOMETRY;
      drawselect_tours();
      SNIFF_ERRORS("after drawselect_tours");
      return;
    }
  }


/* ++++++++++++++++++++++++ draw tours +++++++++++++++++++++++++ */

  if(showtours==1){
    CLIP_GEOMETRY;
    drawtours();
    SNIFF_ERRORS("after drawtours");
  }

  /* ++++++++++++++++++++++++ draw stereo parallax indicator +++++++++++++++++++++++++ */

  if(show_parallax==1){
    UNCLIP;
    Antialias(ON);
    glLineWidth(linewidth);
    glBegin(GL_LINES);
    glColor3fv(foregroundcolor);
    glVertex3f(0.75,0.0,0.25);
    glVertex3f(0.75,1.0,0.25);
    glEnd();
    Antialias(OFF);
  }

  /* ++++++++++++++++++++++++ draw blockages +++++++++++++++++++++++++ */

  CLIP_GEOMETRY;
  DrawBlockages(mode,DRAW_OPAQUE);
  SNIFF_ERRORS("DrawBlockages");

  /* ++++++++++++++++++++++++ draw triangles +++++++++++++++++++++++++ */

  if(ngeominfoptrs>0){
    CLIP_GEOMETRY;
    draw_geom(DRAW_OPAQUE,GEOM_STATIC);
    draw_geom(DRAW_OPAQUE,GEOM_DYNAMIC);
    SNIFF_ERRORS("draw_geom");
  }

  /* ++++++++++++++++++++++++ draw diagnostic geometry +++++++++++++++++++++++++ */

  if(show_geometry_diagnostics==1){
    CLIP_GEOMETRY;
    draw_geomdiag();
    SNIFF_ERRORS("draw_geomdiag");
  }

  /* ++++++++++++++++++++++++ draw shooter points +++++++++++++++++++++++++ */

  if(showshooter!=0&&shooter_active==1){
    CLIP_VALS;
    draw_shooter();
    SNIFF_ERRORS("draw_shooter");
  }

/* ++++++++++++++++++++++++ draw terrain +++++++++++++++++++++++++ */

  if(visTerrainType!=TERRAIN_HIDDEN&&nterraininfo>0){
    int i;

    //shaded 17 0
    //stepped 18 1
    //line    19 2
    //texture 20 3
    //hidden 20 4

    CLIP_GEOMETRY;
    for(i=0;i<nterraininfo;i++){
      terraindata *terri;
      int only_geom;

      terri = terraininfo + i;
      if(terri->loaded==1){
        only_geom=0;
      }
      else{
        only_geom=1;
      }
      switch(visTerrainType){
        case TERRAIN_3D:
          drawterrain(terri,only_geom);
          break;
        case TERRAIN_2D_STEPPED:
          if(cullfaces==1)glDisable(GL_CULL_FACE);
          glPushMatrix();
          glScalef(SCALE2SMV(1.0),SCALE2SMV(1.0),SCALE2SMV(1.0));
          glTranslatef(-xbar0,-ybar0,-zbar0);
          DrawContours(&meshinfo[i].terrain_contour);
          glPopMatrix();
          if(cullfaces==1)glEnable(GL_CULL_FACE);
          break;
        case TERRAIN_2D_LINE:
          glPushMatrix();
          glScalef(SCALE2SMV(1.0),SCALE2SMV(1.0),SCALE2SMV(1.0));
          glTranslatef(-xbar0,-ybar0,-zbar0);
          DrawLineContours(&meshinfo[i].terrain_contour,1.0);
          glPopMatrix();
          break;
        case TERRAIN_3D_MAP:
          if(terrain_texture!=NULL&&terrain_texture->loaded==1){
            drawterrain_texture(terri,only_geom);
          }
          else{
            drawterrain(terri,only_geom);
          }
          break;
        default:
          ASSERT(FFALSE);
          break;
      }
    }
  }

/* ++++++++++++++++++++++++ draw slice files +++++++++++++++++++++++++ */

  if(show_gslice_triangles==1||show_gslice_normal==1||show_gslice_normal_keyboard==1||show_gslice_triangulation==1){
    CLIP_VALS;
    drawgslice_outline();
  }
  if((show_slices_and_vectors==1&&showvslice==1)||(showslice==1&&use_transparency_data==0)){
    CLIP_VALS;
    draw_sliceframe();
  }

  /* ++++++++++++++++++++++++ draw boundary files +++++++++++++++++++++++++ */

  if(showpatch==1){
    CLIP_VALS;
    draw_patchframe(DRAW_OPAQUE);
  }

/* ++++++++++++++++++++++++ draw labels +++++++++++++++++++++++++ */

  if(visLabels==1){
    CLIP_GEOMETRY;
    DrawLabels();
  }

/* ++++++++++++++++++++++++ draw animated isosurfaces +++++++++++++++++++++++++ */

    //if(isoinfo!=NULL)drawspherepoints(sphereinfo);
  if(showiso==1){
    CLIP_VALS;
    drawiso(DRAW_OPAQUE);
  }

/* ++++++++++++++++++++++++ draw zone fire modeling info +++++++++++++++++++++++++ */

  if(nrooms>0){
    CLIP_GEOMETRY;
    drawroomgeom();
    SNIFF_ERRORS("after drawroomgeom");
  }
  if(nrooms>0){
    if(showzone==1){
      CLIP_VALS;
      drawfiredata();
      SNIFF_ERRORS("after drawroomdata");
      if(ReadZoneFile==1&&nzvents>0){
        drawventdata();
        SNIFF_ERRORS("after drawventdata");
      }
    }
  }


//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
//    nothing transparent should be drawn before this portion of the code
//    (ie draw all opaque objects first then draw transparent objects
//**********************************************************************************
//**********************************************************************************
//**********************************************************************************

  /* ++++++++++++++++++++++++ draw triangles +++++++++++++++++++++++++ */

  if(ngeominfoptrs>0){
    CLIP_GEOMETRY;
    draw_geom(DRAW_TRANSPARENT,GEOM_STATIC);
    draw_geom(DRAW_TRANSPARENT,GEOM_DYNAMIC);
  }

  if(showiso==1){
    CLIP_VALS;
    drawiso(DRAW_TRANSPARENT);
  }

/* ++++++++++++++++++++++++ draw transparent faces +++++++++++++++++++++++++ */

  CLIP_GEOMETRY;
  DrawTransparentFaces();

/* ++++++++++++++++++++++++ draw 3D smoke +++++++++++++++++++++++++ */

  if(show3dsmoke==1||showvolrender==1){
    CLIP_VALS;
    draw_smokeframe();
  }

  if(active_smokesensors==1&&show_smokesensors!=SMOKESENSORS_HIDDEN){
    CLIP_VALS;
    getsmokesensors();
    draw_devices_val();
  }

/* ++++++++++++++++++++++++ draw zone fire modeling info +++++++++++++++++++++++++ */

  if(nrooms>0&&showzone==1){
    CLIP_VALS;
    drawroomdata();
    SNIFF_ERRORS("after drawroomdata");
  }

  /* ++++++++++++++++++++++++ draw boundary files +++++++++++++++++++++++++ */

  if(showpatch == 1){
    CLIP_VALS;
    draw_patchframe(DRAW_TRANSPARENT);
  }

/* ++++++++++++++++++++++++ draw slice files +++++++++++++++++++++++++ */

  if((show_slices_and_vectors==1&&showvslice==1)||(showslice==1&&use_transparency_data==1)){
    CLIP_VALS;
    draw_sliceframe();
    SNIFF_ERRORS("after draw_sliceframe");
  }

/* ++++++++++++++++++++++++ draw transparent blockages +++++++++++++++++++++++++ */

//  DrawDemo(20,20);
//  DrawDemo2(1);
  CLIP_GEOMETRY;
  DrawBlockages(mode,DRAW_TRANSPARENT);
  SNIFF_ERRORS("after drawBlokcages");

/* ++++++++++++++++++++++++ draw vector slice files +++++++++++++++++++++++++ */

  if(showvslice==1){
    CLIP_VALS;
    draw_vsliceframe();
  }
  SNIFF_ERRORS("after drawvslice");

/* ++++++++++++++++++++++++ draw plot3d files +++++++++++++++++++++++++ */

  if(showplot3d==1){
    CLIP_VALS;
    draw_plot3dframe();
  }
  SNIFF_ERRORS("after drawplot3d");
}

