/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2020 SIGNET Lab, Department of Information Engineering,
 * University of Padova
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include "simple-matrix-based-channel-model.h"
#include "ns3/log.h"
#include "ns3/three-gpp-antenna-array-model.h"
#include "ns3/double.h"
#include "ns3/pointer.h"
#include "ns3/node.h"
#include "ns3/mobility-model.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("SimpleMatrixBasedChannelModel");

NS_OBJECT_ENSURE_REGISTERED (SimpleMatrixBasedChannelModel);

TypeId
SimpleMatrixBasedChannelModel::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::SimpleMatrixBasedChannelModel")
    .SetParent<Object> ()
    .SetGroupName ("Spectrum")
    .SetParent<MatrixBasedChannelModel> ()
    .AddConstructor<SimpleMatrixBasedChannelModel> ()
    .AddAttribute ("Frequency",
                   "The operating Frequency in Hz",
                   DoubleValue (500.0e6),
                   MakeDoubleAccessor (&SimpleMatrixBasedChannelModel::SetFrequency,
                                       &SimpleMatrixBasedChannelModel::GetFrequency),
                   MakeDoubleChecker<double> ())
  ;
  return tid;
}

SimpleMatrixBasedChannelModel::SimpleMatrixBasedChannelModel ()
{
  NS_LOG_FUNCTION (this);
}

SimpleMatrixBasedChannelModel::~SimpleMatrixBasedChannelModel ()
{
  NS_LOG_FUNCTION (this);
}

void
SimpleMatrixBasedChannelModel::DoDispose ()
{
}

void
SimpleMatrixBasedChannelModel::SetFrequency (double f)
{
  NS_LOG_FUNCTION (this << f);
  NS_ABORT_MSG_UNLESS (f >= 0, "Frequency must be non-negative");

  m_frequency = f;
}

double
SimpleMatrixBasedChannelModel::GetFrequency (void) const
{
  NS_LOG_FUNCTION (this);
  return m_frequency;
}


void
SimpleMatrixBasedChannelModel::SetAodAzimuth (MatrixBasedChannelModel::MatrixBasedChannelModel::DoubleVector x)
{
  NS_LOG_FUNCTION (this);
  m_aodAz = x;
}

MatrixBasedChannelModel::DoubleVector
SimpleMatrixBasedChannelModel::GetAodAzimuth (void) const
{
  NS_LOG_FUNCTION (this);
  return m_aodAz;
}


void
SimpleMatrixBasedChannelModel::SetAodElevation (MatrixBasedChannelModel::DoubleVector x)
{
  NS_LOG_FUNCTION (this);
  for (double val : x)
    {
      NS_ABORT_MSG_UNLESS (val >= 0.0 && val <= 180.0, "Elevation should be in [0, 180] deg");
    }
  m_aodEl = x;
}

MatrixBasedChannelModel::DoubleVector
SimpleMatrixBasedChannelModel::GetAodElevation (void) const
{
  NS_LOG_FUNCTION (this);
  return m_aodEl;
}


void
SimpleMatrixBasedChannelModel::SetAoaAzimuth (MatrixBasedChannelModel::DoubleVector x)
{
  NS_LOG_FUNCTION (this);
  m_aoaAz = x;
}

MatrixBasedChannelModel::DoubleVector
SimpleMatrixBasedChannelModel::GetAoaAzimuth (void) const
{
  NS_LOG_FUNCTION (this);
  return m_aoaAz;
}


void
SimpleMatrixBasedChannelModel::SetAoaElevation (MatrixBasedChannelModel::DoubleVector x)
{
  NS_LOG_FUNCTION (this);
  for (double val : x)
    {
      NS_ABORT_MSG_UNLESS (val >= 0.0 && val <= 180.0, "Elevation should be in [0, 180] deg");
    }
  m_aoaEl = x;
}

MatrixBasedChannelModel::DoubleVector
SimpleMatrixBasedChannelModel::GetAoaElevation (void) const
{
  NS_LOG_FUNCTION (this);
  return m_aoaEl;
}


void
SimpleMatrixBasedChannelModel::SetPhaseShift (MatrixBasedChannelModel::DoubleVector x)
{
  NS_LOG_FUNCTION (this);
  m_phaseShift = x;
}

MatrixBasedChannelModel::DoubleVector
SimpleMatrixBasedChannelModel::GetPhaseShift (void) const
{
  NS_LOG_FUNCTION (this);
  return m_phaseShift;
}


void
SimpleMatrixBasedChannelModel::SetPathLoss (MatrixBasedChannelModel::DoubleVector x)
{
  NS_LOG_FUNCTION (this);
  m_pathLoss = x;
}

MatrixBasedChannelModel::DoubleVector
SimpleMatrixBasedChannelModel::GetPathLoss (void) const
{
  NS_LOG_FUNCTION (this);
  return m_pathLoss;
}


void
SimpleMatrixBasedChannelModel::SetDelay (MatrixBasedChannelModel::DoubleVector x)
{
  NS_LOG_FUNCTION (this);
  m_delay = x;
}

MatrixBasedChannelModel::DoubleVector
SimpleMatrixBasedChannelModel::GetDelay (void) const
{
  NS_LOG_FUNCTION (this);
  return m_delay;
}



Ptr<const MatrixBasedChannelModel::ChannelMatrix>
SimpleMatrixBasedChannelModel::GetChannel (Ptr<const MobilityModel> aMob,
                                           Ptr<const MobilityModel> bMob,
                                           Ptr<const ThreeGppAntennaArrayModel> aAntenna,
                                           Ptr<const ThreeGppAntennaArrayModel> bAntenna)
{
  NS_LOG_FUNCTION (this << aMob << bMob << aAntenna << bAntenna);

  // Check for consistency
  NS_ABORT_MSG_UNLESS (m_delay.size () == m_aodAz.size (), "All ray-related vectors should have the same size");
  NS_ABORT_MSG_UNLESS (m_delay.size () == m_aodEl.size (), "All ray-related vectors should have the same size");
  NS_ABORT_MSG_UNLESS (m_delay.size () == m_aoaAz.size (), "All ray-related vectors should have the same size");
  NS_ABORT_MSG_UNLESS (m_delay.size () == m_aoaEl.size (), "All ray-related vectors should have the same size");
  NS_ABORT_MSG_UNLESS (m_delay.size () == m_phaseShift.size (), "All ray-related vectors should have the same size");
  NS_ABORT_MSG_UNLESS (m_delay.size () == m_pathLoss.size (), "All ray-related vectors should have the same size");

  uint64_t aSize = aAntenna->GetNumberOfElements ();
  uint64_t bSize = bAntenna->GetNumberOfElements ();
  uint64_t numClusters = m_delay.size ();

  // Initialize the channel matrix: consider a the tx, b the rx
  // The size of the channel matrix will be (bSize) x (aSize) x (numClusters)
  Complex3DVector H;  //channel coffecient H[b][a][n];

  H.resize (bSize);
  for (uint64_t bIndex = 0; bIndex < bSize; bIndex++)
    {
      H[bIndex].resize (aSize);
      for (uint64_t aIndex = 0; aIndex < aSize; aIndex++)
        {
          H[bIndex][aIndex].resize (numClusters);
        }
    }

  // Create the channel matrix
  for (uint64_t n = 0; n < numClusters; n++)
    {
      Angles aod (DegreesToRadians (m_aodAz[n]), DegreesToRadians (m_aodEl[n]));
      Angles aoa (DegreesToRadians (m_aoaAz[n]), DegreesToRadians (m_aoaEl[n]));

      for (uint64_t aIndex = 0; aIndex < aSize; aIndex++)
        {
          // Element location phase shift
          Vector aLoc = aAntenna->GetElementLocation (aIndex);
          double aPhase = 2 * M_PI * (sin (aod.theta) * cos (aod.phi) * aLoc.x
                                      + sin (aod.theta) * sin (aod.phi) * aLoc.y
                                      + cos (aod.theta) * aLoc.z);

          for (uint64_t bIndex = 0; bIndex < bSize; bIndex++)
            {
              // Element location phase shift
              Vector bLoc = bAntenna->GetElementLocation (bIndex);
              double bPhase = 2 * M_PI * (sin (aoa.theta) * cos (aoa.phi) * bLoc.x
                                          + sin (aoa.theta) * sin (aoa.phi) * bLoc.y
                                          + cos (aoa.theta) * bLoc.z);

              // Pathloss and phase shift
              double p = std::pow (10, m_pathLoss[n] / 20);
              double delayPhase = -2 * M_PI * m_delay[n] * m_frequency;
              double totalPhase = delayPhase + m_phaseShift[n] + aPhase + bPhase;
              std::complex<double> totalShift = std::polar (1.0, totalPhase);

              // Consider only the vertical component
              double aGain = std::get<1> (aAntenna->GetElementFieldPattern (aod));
              double bGain = std::get<1> (bAntenna->GetElementFieldPattern (aoa));

              H[bIndex][aIndex][n] = (p * aGain * bGain) * totalShift;
            }
        }
    }

  // Channel matrix params
  Double2DVector angles;
  angles.resize (4);
  angles[0] = m_aoaAz;
  angles[1] = m_aoaEl;
  angles[2] = m_aodAz;
  angles[3] = m_aodEl;

  MatrixBasedChannelModel::DoubleVector delays;
  delays.resize (m_delay.size ());
  for (uint32_t i = 0; i < m_delay.size (); ++i)
    {
      delays[i] = m_delay[i] * 1e9; // [s] -> [ns]
    }

  // fill channel matrix
  Ptr<MatrixBasedChannelModel::ChannelMatrix> channelMatrix = Create<MatrixBasedChannelModel::ChannelMatrix> ();
  channelMatrix->m_channel = H;
  channelMatrix->m_delay = delays;
  channelMatrix->m_angle = angles;
  channelMatrix->m_generatedTime = Seconds (0);
  channelMatrix->m_nodeIds = std::make_pair (aMob->GetObject<Node> ()->GetId (),
                                             bMob->GetObject<Node> ()->GetId ());

  return channelMatrix;
}

}  // namespace ns3
