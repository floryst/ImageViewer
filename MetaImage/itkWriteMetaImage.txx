/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkWriteMetaImage.txx
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
#ifndef _itkWriteMetaImage_txx
#define _itkWriteMetaImage_txx

#include <string>

#include "itkWriteMetaImage.h"
#include "itkObjectFactory.h"
#include <MetaImageLib.h>
#include <itkSimpleImageRegionIterator.h>

namespace itk
{

/**
 *
 */
template <class TInputImage>
WriteMetaImage<TInputImage>
::WriteMetaImage()
{
  m_FileName = "";
}





/**
 *
 */
template <class TInputImage>
void 
WriteMetaImage<TInputImage>
::GenerateData(void)
{
  this->WriteData();  
}





/**
 *
 */
template <class TInputImage>
void 
WriteMetaImage<TInputImage>
::WriteData(void)
{
  
  const unsigned int BitsPerPixel = 
                          8*sizeof( PixelType );

  const unsigned int dimension = TInputImage::ImageDimension;

  typename TInputImage::Pointer m_InputImage( GetInput() );

  int dimSize[ dimension ];

  const typename TInputImage::SizeType & size = 
        m_InputImage->GetBufferedRegion().GetSize();
        
  const double         *spacing = m_InputImage->GetSpacing();                
  float fspacing[dimension];

  for(unsigned int i=0; i<dimension; i++) 
  {
    dimSize[i]  = size[i];
    fspacing[i] = spacing[i];
  }

  typedef typename TInputImage::PixelType PixelType;
  
  PixelType *yetAnotherBuffer = new PixelType[ 
            m_InputImage->GetOffsetTable()[dimension] ];

  typedef itk::SimpleImageRegionIterator< TInputImage > IteratorType;
  
  IteratorType it(	m_InputImage, 
                    m_InputImage->GetBufferedRegion() );

  
  PixelType * destination = yetAnotherBuffer;
  it.Begin();
  while( !it.IsAtEnd() ) 
  {
    *destination++ = it.Get(); 
    ++it;
  }

  MetaImage saver(  dimension,
                    dimSize,
                    GetTypeCode(),
                    fspacing,
                    BitsPerPixel,
                    MET_SYSTEM_BYTE_ORDER_MSB,
                    yetAnotherBuffer  );


  std::string fileExtension ;
  fileExtension= m_FileName.substr(m_FileName.rfind('.') + 1, 
                                   m_FileName.size() - m_FileName.rfind('.')) ;

  // if file extension is mhd, then save the file with header file (.mhd)
  // and data file (.raw)
  if (fileExtension == "mhd")
    {
      std::string baseFileName = 
        m_FileName.substr(0, m_FileName.rfind('.')) ;
      std::string rawFileName = baseFileName + ".raw" ;
      saver.Save( this->m_FileName.c_str(), rawFileName.c_str(), 1);
    }
  else
      saver.Save( this->m_FileName.c_str(), 0, 1);
    

  delete [] yetAnotherBuffer;

  if(saver.Error())  {
    std::cerr << "Unable to open file:" << m_FileName << std::endl;
    return;
  }

}






/**
 *
 */
template <class TInputImage>
void 
WriteMetaImage<TInputImage>
::PrintSelf(std::ofstream& os, Indent indent)
{
  Superclass::PrintSelf(os,indent);
  os << indent << "Meta Image file format writer";
}







} // end namespace itk

#endif
