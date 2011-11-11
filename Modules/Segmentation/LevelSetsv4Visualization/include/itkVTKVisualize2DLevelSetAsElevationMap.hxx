/*=========================================================================
 *
 *  Copyright Insight Software Consortium
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
#ifndef __itkVTKVisualize2DLevelSetAsElevationMap_hxx
#define __itkVTKVisualize2DLevelSetAsElevationMap_hxx

#include "itkVTKVisualize2DLevelSetAsElevationMap.h"

namespace itk
{

template< class TInputImage, class TLevelSet >
VTKVisualize2DLevelSetAsElevationMap< TInputImage, TLevelSet >
::VTKVisualize2DLevelSetAsElevationMap()
{
  this->m_ColorValue = true;
  this->m_MinValue = itk::NumericTraits< double >::max( );
  this->m_MaxValue = itk::NumericTraits< double >::min( );
  this->m_Constant = 0.1;

  this->m_Mesh = vtkSmartPointer< vtkPolyData >::New();

  this->m_Annotation = vtkSmartPointer< vtkCornerAnnotation >::New();
  this->m_Renderer->AddActor2D( m_Annotation );
}

template< class TInputImage, class TLevelSet >
VTKVisualize2DLevelSetAsElevationMap< TInputImage, TLevelSet >
::~VTKVisualize2DLevelSetAsElevationMap()
{
}

template< class TInputImage, class TLevelSet >
void
VTKVisualize2DLevelSetAsElevationMap< TInputImage, TLevelSet >
::SetLevelSet( LevelSetType * levelSet )
{
  this->m_LevelSet = levelSet;
}

template< class TInputImage, class TLevelSet >
void
VTKVisualize2DLevelSetAsElevationMap< TInputImage, TLevelSet >
::PrepareVTKPipeline()
{
  vtkSmartPointer< vtkImageShiftScale > shift =
      vtkSmartPointer< vtkImageShiftScale >::New();
  shift->SetInput( this->m_InputImageConverter->GetOutput() );
  shift->SetOutputScalarTypeToUnsignedChar();
  shift->Update();

  vtkSmartPointer< vtkImageActor > input_Actor =
      vtkSmartPointer< vtkImageActor >::New();
  input_Actor->SetInput( shift->GetOutput() );

  this->GenerateElevationMap();

  vtkPolyDataMapper* meshmapper = vtkPolyDataMapper::New( );
  meshmapper->SetInput( this->m_Mesh );

  if( !this->m_ColorValue )
    {
    meshmapper->ScalarVisibilityOff( );
    }
  else
    {
    meshmapper->SetScalarRange( this->m_MinValue, this->m_MaxValue );
    }

  vtkActor *SurfaceActor = vtkActor::New( );
  SurfaceActor->SetMapper( meshmapper );
  SurfaceActor->GetProperty( )->SetColor( 0.7, 0.7, 0.7 );

  if( this->m_ColorValue )
    {
    vtkSmartPointer< vtkScalarBarActor > scalarBar =
        vtkSmartPointer< vtkScalarBarActor >::New( );
    scalarBar->SetLookupTable( meshmapper->GetLookupTable( )  );
    scalarBar->SetTitle( "Level Set" );
    scalarBar->GetPositionCoordinate( )->SetCoordinateSystemToNormalizedViewport( );
    scalarBar->GetPositionCoordinate( )->SetValue( 0.1, 0.01 );
    scalarBar->GetTitleTextProperty( )->SetColor( 0., 0., 0. );
    scalarBar->GetLabelTextProperty( )->SetColor( 0., 0., 0. );
    scalarBar->SetOrientationToHorizontal( );
    scalarBar->SetWidth( 0.8 );
    scalarBar->SetHeight( 0.17 );

    this->m_Renderer->AddActor2D( scalarBar );
    }

  this->m_Renderer->AddActor ( input_Actor );
  this->m_Renderer->AddActor ( SurfaceActor );

  std::stringstream counter;
  counter << this->GetCurrentIteration();

  m_Annotation->SetText( 0, counter.str().c_str() );

  this->m_Renderer->AddActor2D( input_Actor );
  //      m_Ren->AddActor2D( scalarbar );
}

template< class TInputImage, class TLevelSet >
void
VTKVisualize2DLevelSetAsElevationMap< TInputImage, TLevelSet >
::GenerateElevationMap()
{
  typename InputImageType::ConstPointer inputImage = this->m_InputImageConverter->GetInput();
  typename InputImageType::RegionType       region = inputImage->GetLargestPossibleRegion();

  typedef typename InputImageType::IndexType      IndexType;
  typedef typename InputImageType::PointType      PointType;

  IndexType start = region.GetIndex();
  PointType itkPoint;
  PointType itkPoint2;

  InputImageSizeType   size =  region.GetSize();

  this->m_NumberOfSamples[0] = size[0] / 2;
  this->m_NumberOfSamples[1] = size[1] / 2;

  IndexType dx;
  dx[0] = static_cast< IndexValueType >( size[0] / this->m_NumberOfSamples[0] );
  dx[1] = static_cast< IndexValueType >( size[1] / this->m_NumberOfSamples[1] );

  vtkSmartPointer< vtkPoints >         vtkpoints = vtkSmartPointer< vtkPoints >::New( );
  vtkSmartPointer< vtkDoubleArray > vtkpointdata = vtkSmartPointer< vtkDoubleArray >::New( );
  vtkSmartPointer< vtkCellArray >          cells = vtkSmartPointer< vtkCellArray >::New( );

  this->m_Mesh->SetPoints( vtkpoints );
  this->m_Mesh->GetPointData( )->SetScalars( vtkpointdata );
  this->m_Mesh->SetPolys( cells );

  InputImageSizeValueType k = 0;

  IndexType index;
  double p[3];

  for( InputImageSizeValueType i = 0; i < this->m_NumberOfSamples[0]; i++ )
    {
    index[0] = start[0] + i * dx[0];

    for( InputImageSizeValueType j = 0; j < this->m_NumberOfSamples[1]; j++ )
      {
      index[1] = start[1] + j * dx[1];

      inputImage->TransformIndexToPhysicalPoint( index, itkPoint );

      p[0] = itkPoint[0];
      p[1] = itkPoint[1];
      p[2] = static_cast< double >( this->m_LevelSet->Evaluate( index ) );

      vtkpointdata->InsertNextTuple1( p[2] );

      if( p[2] < m_MinValue )
        {
        m_MinValue = p[2];
        }
      if( p[2] > m_MaxValue )
        {
        m_MaxValue = p[2];
        }

      vtkpoints->InsertPoint( k++, p );
      }
    }

  double den = vnl_math_max( vnl_math_abs( m_MinValue ),
                             vnl_math_abs( m_MaxValue ) );

  inputImage->TransformIndexToPhysicalPoint( start, itkPoint );

  index = start;

  index[0] += size[0] - 1;
  index[1] += size[1] - 1;

  inputImage->TransformIndexToPhysicalPoint( index, itkPoint2 );

  double ratio = m_Constant *
      static_cast< double >( itkPoint.EuclideanDistanceTo( itkPoint2 ) );

  if( den != 0. )
    {
    ratio /= den;
    }

  for( vtkIdType i = 0; i < vtkpoints->GetNumberOfPoints(); i++ )
    {
    vtkpoints->GetPoint( i, p );
    p[2] *= ratio;
    vtkpoints->SetPoint( i, p );
    }

  vtkIdType vtkId[3];

  for( InputImageSizeValueType j = 0; j < ( this->m_NumberOfSamples[1] - 1 ); j++ )
    {
    for( InputImageSizeValueType i = 0; i < ( this->m_NumberOfSamples[0] - 1 ); i++ )
      {
      vtkId[0] = j * m_NumberOfSamples[0] + i;
      vtkId[1] = vtkId[0] + 1;
      vtkId[2] = vtkId[1] + m_NumberOfSamples[0];

      this->m_Mesh->InsertNextCell( VTK_TRIANGLE, 3, static_cast< vtkIdType* >( vtkId ) );

      vtkId[1] = vtkId[2];
      vtkId[2] = vtkId[1] - 1;
      this->m_Mesh->InsertNextCell( VTK_TRIANGLE, 3, static_cast< vtkIdType* >( vtkId ) );
      }
    }
}


} // end namespace itk

#endif