/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Example of Simulating a UDP Star Topology with Flow Monitor
 * Copyright (c) 2022 Ryan Hu
 * This code is derived from the ns-3 example tcp-star-server.cc
 * The original copyright statement is included in the following.
 *
 * This program simulate a star topology with 5 nodes by default
 *  If tracing is enabled,  the queues and packet receptions are saved to
 *  "udp-star-server.tr" and pcap traces generated in the following files
 *  "udp-star-server-$n-$i.pcap" where n and i represent node and interface numbers respectively.
 *
 *  Usage of this example file:
 *        NS_LOG="UdpStar" ./waf --run="scratch/04_02"
 *        NS_LOG="UdpStar" ./waf --run="scratch/04_02" --cwd=<your directory to redirect output traces>
 *        NS_LOG="UdpStar" ./waf --run="scratch/04_02 --nNodes=5"
 *        NS_LOG="UdpStar" ./waf --run="scratch/04_02 --nNodes=5 --logging=true"
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
 *
 */

#include <iostream>
#include <fstream>
#include <string>
#include <cassert>

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/flow-monitor-helper.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("UdpStar");

int main(int argc, char *argv[])
{

  // Set up some default values for the simulation.
  uint32_t N = 5; // Number of nodes in the star network
  bool logging = false;

  // Allow the user to override any of the defaults and the above
  CommandLine cmd(__FILE__);
  cmd.AddValue("nNodes", "Number of nodes to place in the star", N);
  cmd.AddValue("logging", "Enable logging if true", logging);
  cmd.Parse(argc, argv);

  // Explicit debugging for selected modules is possible.
  if (logging == true)
  {
    LogComponentEnable("UdpClient", LOG_LEVEL_INFO);
    LogComponentEnable("UdpServer", LOG_LEVEL_INFO);
  }

  // Create nodes on the network.
  NS_LOG_INFO("Create Nodes.");
  NodeContainer serverNode;
  NodeContainer clientNodes;
  serverNode.Create(1);
  clientNodes.Create(N - 1);
  NodeContainer allNodes = NodeContainer(serverNode, clientNodes);

  // Collect an adjacency list of nodes for the p2p topology
  std::vector<NodeContainer> nodeAdjacencyList(N - 1);
  for (uint32_t i = 0; i < nodeAdjacencyList.size(); ++i)
  {
    nodeAdjacencyList[i] = NodeContainer(serverNode, clientNodes.Get(i));
  }

  // Create P2P channels
  NS_LOG_INFO("Build a Star Topology.");
  PointToPointHelper p2p;
  p2p.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
  p2p.SetChannelAttribute("Delay", StringValue("2ms"));
  std::vector<NetDeviceContainer> deviceAdjacencyList(N - 1);
  // Each node in the NodeContainer will be installed with a p2p NetDevice, the returned
  // NetDeviceContainer will be stored in the deviceAdjacencyList.
  for (uint32_t i = 0; i < deviceAdjacencyList.size(); ++i)
  {
    deviceAdjacencyList[i] = p2p.Install(nodeAdjacencyList[i]);
  }

  // Install network stacks on the nodes
  InternetStackHelper internet;
  internet.Install(allNodes);

  // Configure IP addresses
  NS_LOG_INFO("Assign IP Addresses.");
  Ipv4AddressHelper ipv4;
  std::vector<Ipv4InterfaceContainer> interfaceAdjacencyList(N - 1);
  for (uint32_t i = 0; i < interfaceAdjacencyList.size(); ++i)
  {
    std::ostringstream subnet;
    subnet << "10.1." << i + 1 << ".0";
    ipv4.SetBase(subnet.str().c_str(), "255.255.255.0");
    interfaceAdjacencyList[i] = ipv4.Assign(deviceAdjacencyList[i]);
  }

  Address serverAddress = Address(interfaceAdjacencyList[0].GetAddress(0));

  // Build a routing table with all nodes as routers in the simulation
  Ipv4GlobalRoutingHelper::PopulateRoutingTables();

  // Create the UDP server application on Node 0
  NS_LOG_INFO("Create UdpServer application on Node 0.");
  uint16_t port = 50000; // Use a private port beyond the registered ports by IANA
  UdpServerHelper server(port);
  ApplicationContainer severApps = server.Install(serverNode.Get(0));
  severApps.Start(Seconds(1.0));
  severApps.Stop(Seconds(10.0));

  // Create UDP client applications on Nodes 1-4
  NS_LOG_INFO("Create UDP client applications on Nodes 1-4.");
  uint32_t maxPacketSize = 1024;
  Time interPacketInterval = Seconds(0.5);
  uint32_t maxPacketCount = 10;
  UdpClientHelper clientHelper(serverAddress, port);
  clientHelper.SetAttribute("MaxPackets", UintegerValue(maxPacketCount));
  clientHelper.SetAttribute("Interval", TimeValue(interPacketInterval));
  clientHelper.SetAttribute("PacketSize", UintegerValue(maxPacketSize));
  ApplicationContainer clientApps;
  for (uint32_t i = 0; i < clientNodes.GetN(); ++i)
  {
    clientApps.Add(clientHelper.Install(clientNodes.Get(i)));
  }

  clientApps.Start(Seconds(2.0));
  clientApps.Stop(Seconds(10.0));

  // Configure tracing (Disabled/Commented)
  // AsciiTraceHelper ascii;
  // p2p.EnableAsciiAll (ascii.CreateFileStream ("udp-star-server.tr"));
  // p2p.EnablePcapAll ("udp-star-server");

  // Flow Monitor
  FlowMonitorHelper flowmonHelper;
  flowmonHelper.InstallAll();

  NS_LOG_INFO("Run Simulation.");
  Simulator::Stop(Seconds(10.0 + 2.0)); // To ensure the flow monitor tracks all packets, stop the simulation
                                        // at least a few seconds after the application stop time.
  Simulator::Run();
  flowmonHelper.SerializeToXmlFile("04_02_flow_monitor.xml", false, false);
  Simulator::Destroy();
  NS_LOG_INFO("Done.");

  return 0;
}
