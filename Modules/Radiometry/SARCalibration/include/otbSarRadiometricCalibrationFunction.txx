/*=========================================================================

  Program:   ORFEO Toolbox
  Language:  C++
  Date:      $Date$
  Version:   $Revision$


  Copyright (c) Centre National d'Etudes Spatiales. All rights reserved.
  See OTBCopyright.txt for details.


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __otbSarRadiometricCalibrationFunction_txx
#define __otbSarRadiometricCalibrationFunction_txx

#include "otbSarRadiometricCalibrationFunction.h"
#include "itkNumericTraits.h"

namespace otb
{
/**
 * Constructor
 */
template <class TInputImage, class TCoordRep>
SarRadiometricCalibrationFunction<TInputImage, TCoordRep>
::SarRadiometricCalibrationFunction():
  m_Scale(1.0)
{
  m_Noise = ParametricFunctionType::New();
  m_AntennaPatternNewGain = ParametricFunctionType::New();
  m_AntennaPatternOldGain = ParametricFunctionType::New();
  m_IncidenceAngle = ParametricFunctionType::New();
  m_RangeSpreadLoss = ParametricFunctionType::New();

  m_Noise->SetConstantValue(0.0);
  m_EnableNoise = true;
  m_AntennaPatternNewGain->SetConstantValue(1.0);
  m_AntennaPatternOldGain->SetConstantValue(1.0);
  m_IncidenceAngle->SetConstantValue(CONST_PI_2);
  m_RangeSpreadLoss->SetConstantValue(1.0);
  m_Lut = 0; //new LookupTableBase();

  m_RescalingFactor = 1.0;
}

/**
 * Initialize by setting the input image
 */
template <class TInputImage, class TCoordRep>
void
SarRadiometricCalibrationFunction<TInputImage, TCoordRep>
::SetInputImage(
  const InputImageType * ptr )
{
  Superclass::SetInputImage(ptr);
  m_Noise->SetInputImage(ptr);
  m_IncidenceAngle->SetInputImage(ptr);
  m_AntennaPatternNewGain->SetInputImage(ptr);
  m_AntennaPatternOldGain->SetInputImage(ptr);
  m_RangeSpreadLoss->SetInputImage(ptr);

}

/**
 *
 */
template <class TInputImage, class TCoordRep>
void
SarRadiometricCalibrationFunction<TInputImage, TCoordRep>
::PrintSelf(std::ostream& os, itk::Indent indent) const
{
  this->Superclass::PrintSelf(os, indent);
}


template <class TInputImage, class TCoordRep>
typename SarRadiometricCalibrationFunction<TInputImage, TCoordRep>
::OutputType
SarRadiometricCalibrationFunction<TInputImage, TCoordRep>
::EvaluateAtIndex(const IndexType& index) const
{

  if (!this->GetInputImage())
    {
    itkDebugMacro( <<"ERROR with GetInputImage()");
    return (itk::NumericTraits<OutputType>::max());
    }

  if (!this->IsInsideBuffer(index))
    {
    itkDebugMacro( << "ERROR with IsInsideBuffer");
    return (itk::NumericTraits<OutputType>::max());
    }


  PointType point;
  if (m_ApplyAntennaPatternGain || m_ApplyIncidenceAngleCorrection || m_ApplyRangeSpreadingLossCorrection)
    this->GetInputImage()->TransformIndexToPhysicalPoint( index, point);


  FunctorType functor;
  if (m_EnableNoise)
    {
    functor.SetNoise(static_cast<FunctorRealType>(m_Noise->Evaluate(point)));
    }
  functor.SetScale(m_Scale);


  if (m_ApplyAntennaPatternGain)
    {
    functor.SetAntennaPatternNewGain(static_cast<FunctorRealType>(m_AntennaPatternNewGain->Evaluate(point)));
    functor.SetAntennaPatternOldGain(static_cast<FunctorRealType>(m_AntennaPatternOldGain->Evaluate(point)));
    }

    if (m_ApplyIncidenceAngleCorrection)
    {
    functor.SetIncidenceAngle(static_cast<FunctorRealType>(m_IncidenceAngle->Evaluate(point)));
    }

    if (m_ApplyRangeSpreadingLossCorrection)
    {
    functor.SetRangeSpreadLoss(static_cast<FunctorRealType>(m_RangeSpreadLoss->Evaluate(point)));
    }

    if (m_ApplyLookupDataCorrection)
    {
    FunctorRealType lutVal = m_Lut->GetValue(index[0], index[1]);
    functor.SetLutValue(lutVal * lutVal);
    }

    if (m_ApplyRescalingFactor)
    {
    functor.SetRescalingFactor(m_RescalingFactor);
    }
  const RealType value = static_cast<RealType>(vcl_abs(this->GetInputImage()->GetPixel(index)));

  RealType result = functor(value);

  return static_cast<OutputType>(result);
}

} // end namespace otb

#endif
