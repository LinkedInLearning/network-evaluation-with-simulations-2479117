/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Example of Simulating a Mesh Topology
 * Copyright (c) 2022 Ryan Hu
 *
 * This program creates a full mesh topology with 4 nodes, from Node 0 to Node 4. Six point-to-point 
 * (p2p) channels/links will be created where each node has 3 network interfaces (a.k.a. NetDevices)
 * Each network interface pair is on a p2p sub network with a network address, 10.1.1.0 - 10.1.6.0.
 * Node 0 and Node 3 are installed with a UDP Server app and a UDP Client app, respectively. The app
 * transmits 10 packets with 1024 B UDP payload every 0.5 seconds.
 *
 *  Usage of this example file:
 *        NS_LOG="MeshExample" ./waf --run="scratch/03_05"
 *        NS_LOG="MeshExample" ./waf --run="scratch/03_05 --logging=true"
 *  Note: NS_LOG="MeshExample" is to set the environment variable to enable basic logging 
 * outputs using NS_LOG_INFO() macro for this simulation.
 */

/*
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
 */

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv4-global-routing-helper.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("MeshExample");

int main(int argc, char *argv[])
{
  bool logging = false; // Set true to output some UDP server/client logging info

  CommandLine cmd(__FILE__);
  cmd.AddValue("logging", "Enable logging if true", logging);

  cmd.Parse(argc, argv);

  if (logging)
  {
    LogComponentEnable("UdpClient", LOG_LEVEL_INFO);
    LogComponentEnable("UdpServer", LOG_LEVEL_INFO);
  }

  // Create nodes and NodeContainer objects for 6 channels
  NS_LOG_INFO("Create nodes.");
  NodeContainer nodes;
  nodes.Create(4);
  NodeContainer n0n1 = NodeContainer(nodes.Get(0), nodes.Get(1));
  NodeContainer n0n2 = NodeContainer(nodes.Get(0), nodes.Get(2));
  NodeContainer n0n3 = NodeContainer(nodes.Get(0), nodes.Get(3));
  NodeContainer n1n2 = NodeContainer(nodes.Get(1), nodes.Get(2));
  NodeContainer n1n3 = NodeContainer(nodes.Get(1), nodes.Get(3));
  NodeContainer n2n3 = NodeContainer(nodes.Get(2), nodes.Get(3));

  NS_LOG_INFO("Create channels.");
  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute("DataRate", StringValue("1Gbps"));
  pointToPoint.SetChannelAttribute("Delay", StringValue("1ms"));
  NetDeviceContainer d0d1 = pointToPoint.Install(n0n1);
  NetDeviceContainer d0d2 = pointToPoint.Install(n0n2);
  NetDeviceContainer d0d3 = pointToPoint.Install(n0n3);
  NetDeviceContainer d1d2 = pointToPoint.Install(n1n2);
  NetDeviceContainer d1d3 = pointToPoint.Install(n1n3);
  NetDeviceContainer d2d3 = pointToPoint.Install(n2n3);

  NS_LOG_INFO("Install Internet Stack and Set Up IP Addresses.");
  InternetStackHelper stack;
  stack.Install(nodes);

  Ipv4AddressHelper ipv4;
  ipv4.SetBase("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer i0i1 = ipv4.Assign(d0d1);

  ipv4.SetBase("10.1.2.0", "255.255.255.0");
  Ipv4InterfaceContainer i0i2 = ipv4.Assign(d0d2);

  ipv4.SetBase("10.1.3.0", "255.255.255.0");
  Ipv4InterfaceContainer i0i3 = ipv4.Assign(d0d3);

  ipv4.SetBase("10.1.4.0", "255.255.255.0");
  Ipv4InterfaceContainer i1i2 = ipv4.Assign(d1d2);

  ipv4.SetBase("10.1.5.0", "255.255.255.0");
  Ipv4InterfaceContainer i1i3 = ipv4.Assign(d1d3);

  ipv4.SetBase("10.1.6.0", "255.255.255.0");
  Ipv4InterfaceContainer i2i3 = ipv4.Assign(d2d3);

  NS_LOG_INFO("Make router nodes and create routing tables in the nodes.");
  Ipv4GlobalRoutingHelper::PopulateRoutingTables();

  NS_LOG_INFO("Create UdpServer application on Node 0.");
  uint16_t port = 50000; // Use a private port beyond the registered ports by IANA
  UdpServerHelper server(port);
  ApplicationContainer serverApps = server.Install(nodes.Get(0));
  serverApps.Start(Seconds(1.0));
  serverApps.Stop(Seconds(10.0));

  NS_LOG_INFO("Create UdpClient application on Node 3.");
  UdpClientHelper clientHelper(i0i1.GetAddress(0), port);
  uint32_t maxPacketSize = 1024;
  Time interPacketInterval = Seconds(0.5);
  uint32_t maxPacketCount = 10;
  clientHelper.SetAttribute("MaxPackets", UintegerValue(maxPacketCount));
  clientHelper.SetAttribute("Interval", TimeValue(interPacketInterval));
  clientHelper.SetAttribute("PacketSize", UintegerValue(maxPacketSize));
  ApplicationContainer clientApps = clientHelper.Install(nodes.Get(3));
  clientApps.Start(Seconds(2.0));
  clientApps.Stop(Seconds(10.0));

  pointToPoint.EnablePcapAll("mesh");

  NS_LOG_INFO("Run Simulation.");
  Simulator::Run();
  Simulator::Destroy();
  NS_LOG_INFO("Done.");

  return 0;
}
