/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    fltkGlWindow.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

Copyright (c) 2001 Insight Consortium
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

 * Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

 * The name of the Insight Consortium, nor the names of any consortium members,
   nor of any contributors, may be used to endorse or promote products derived
   from this software without specific prior written permission.

  * Modified source versions must be plainly marked as such, and must not be
    misrepresented as being the original software.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER AND CONTRIBUTORS ``AS IS''
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

=========================================================================*/

#include <fltkGlWindow.h>
#include <FL/fl_ask.H>
#include <FL/fl_file_chooser.H>
#include <fstream>
#include <iostream>


namespace fltk {


//--------------------------------------------------
//
//    Creator
//
//--------------------------------------------------
GlWindow::GlWindow(int x,int y,int w,int h, const char * label)
  :Fl_Gl_Window(x,y,w,h,label) 
{
  m_RedrawCommand = RedrawCommandType::New();
  m_RedrawCommand->SetWidget( this );
}


//--------------------------------------------------
//
//    Destructor
//
//--------------------------------------------------
GlWindow::~GlWindow() 
{

}



//--------------------------------------------------
//
//    Grab Image and Store in a PPM file 
//    This is the user interface part.
//
//--------------------------------------------------
void GlWindow::SaveImage(void) 
{

  const char * filename = 
        fl_file_chooser("Please select a file name","*.ppm","");

  if( !filename ) return;

  SaveImage( filename );

}



//------------------------------------------
//
//    Return Redraw Command
//
//------------------------------------------
const GlWindow::RedrawCommandType::Pointer & 
GlWindow
::GetRedrawCommand(  void )
{
  
  return m_RedrawCommand;

}

 

//------------------------------------------
//
//    Add Drawer Object
//
//------------------------------------------
void GlWindow::AddDrawer(  GlDrawerPtr dr) 
{
  
  if( !dr ) return;
  drawer.push_back(dr);

}

  

//------------------------------------------
//
//    Call Model Drawers
//
//------------------------------------------
void GlWindow::CallDrawers(void) const 
{

  if( !drawer.empty() ) 
  {
    std::vector<GlDrawerPtr>::const_iterator dr = drawer.begin();
    while( dr != drawer.end() ) 
    {
      (*dr)->glDraw();
      dr++;
    }
  }

}






//--------------------------------------------------
//
//    Grab Image and Store in a PPM file
//
//--------------------------------------------------
void GlWindow::SaveImage(const char * filename) 
{

  if( !filename ) 
  {
    return;
  }

  if( strlen(filename) == 0 ) 
  {
    return;
  }

  const int wnx = w();
  const int wny = h();

  std::ofstream  of;
  of.open(filename);
  if( of.fail() ) 
  {
    fl_alert("Error opening file %s",filename);
    return;
  }

  unsigned char *image = new unsigned char[ wnx * wny * 8 ];

  if( !image ) 
  {
    fl_alert("Problem","Allocating memory for image grabbing buffer");
    of.close();
    return;
  }


  of << "P6" << std::endl;      // Magic number for PPM files
  of << wnx << " " << wny << std::endl;

  of << 255 << std::endl;

  make_current();
  draw();

  glReadBuffer(GL_FRONT);
  glReadPixels(0,0,wnx,wny,GL_RGBA,GL_UNSIGNED_BYTE,(GLvoid *)image);

  bool firstPixel = true;

  for(int y=wny-1; y>=0; y--)  
  {
    unsigned char *p = image + 4*y*wnx;
    for(int x=0; x<wnx; x++) 
    {
      const unsigned char red   = *p++;
      const unsigned char green = *p++;
      const unsigned char blue  = *p++;
                                   p++; // alpha channel
      if( firstPixel )       
      {
        of << green << blue;
        firstPixel = false;
      }
      else 
      {
        of << red << green << blue;
      }
    }
  }


  of.close();

  delete [] image;

}



} // end namespace fltk


