/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Example of Simulating a 802.11ac Wi-Fi Network
 * Copyright (c) 2022 Ryan Hu
 * This code is derived from the work by Sebastien Deronne.
 * The original copyright statement is included in the following.
 *
 * This script outputs the throughput for UDP packets for a VHT MCS value, where the PHY bitrate is constant over the simulation run.
 * Users can specify the distance between the access point and the station: the larger the distance, the smaller the throughput.
 * The network has two nodes: one Wi-Fi station (STN) and one access point (AP) as shown below:
 *
 *  STA (n0)  <-- 802.11ac channel -->   AP (n1)
 *
 *  Usage of this example file:
 *        ./waf --run="scratch/03_07"
 *        NS_LOG="WifiVht" ./waf --run="scratch/03_07"
 */

/*
 * Copyright (c) 2015 SEBASTIEN DERONNE
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
 * Author: Sebastien Deronne <sebastien.deronne@gmail.com>
 */

#include "ns3/command-line.h"
#include "ns3/config.h"
#include "ns3/uinteger.h"
#include "ns3/boolean.h"
#include "ns3/double.h"
#include "ns3/string.h"
#include "ns3/log.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/ssid.h"
#include "ns3/mobility-helper.h"
#include "ns3/internet-stack-helper.h"
#include "ns3/ipv4-address-helper.h"
#include "ns3/udp-client-server-helper.h"
#include "ns3/packet-sink-helper.h"
#include "ns3/on-off-helper.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/packet-sink.h"
#include "ns3/yans-wifi-channel.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("WifiVht");

int main(int argc, char *argv[])
{

  // Some parameters for simulation
  bool useRts = false;          // A RTS/CTS handshake is not used
  double distance = 10.0;       // Distance between AP and STN nodes in meters
  double simulationTime = 10.0; // Simulation time in seconds

  int mcs = 8;           //  MCS value: 0 - 9
  int channelWidth = 40; // MHz
  bool sgi = true;       // Short guard interval support set to TRUE

  if (useRts)
  {
    Config::SetDefault("ns3::WifiRemoteStationManager::RtsCtsThreshold", StringValue("0"));
  }

  NodeContainer wifiStaNode;
  wifiStaNode.Create(1);
  NodeContainer wifiApNode;
  wifiApNode.Create(1);

  YansWifiChannelHelper channel = YansWifiChannelHelper::Default();
  YansWifiPhyHelper phy;
  phy.SetChannel(channel.Create());

  WifiHelper wifi;
  wifi.SetStandard(WIFI_STANDARD_80211ac);
  WifiMacHelper mac;

  std::ostringstream oss;
  oss << "VhtMcs" << mcs;
  wifi.SetRemoteStationManager("ns3::ConstantRateWifiManager", "DataMode", StringValue(oss.str()),
                               "ControlMode", StringValue(oss.str()));

  Ssid ssid = Ssid("ns3-80211ac");

  mac.SetType("ns3::StaWifiMac", "Ssid", SsidValue(ssid));
  phy.Set("ChannelWidth", UintegerValue(channelWidth));

  NetDeviceContainer staDevice;
  staDevice = wifi.Install(phy, mac, wifiStaNode);

  mac.SetType("ns3::ApWifiMac", "EnableBeaconJitter", BooleanValue(false),
              "Ssid", SsidValue(ssid));
  phy.Set("ChannelWidth", UintegerValue(channelWidth));

  NetDeviceContainer apDevice;
  apDevice = wifi.Install(phy, mac, wifiApNode);

  // Set guard interval
  Config::Set("/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/HtConfiguration/ShortGuardIntervalSupported", BooleanValue(sgi));

  // Mobility setup for nodes
  MobilityHelper mobility;
  Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator>();

  positionAlloc->Add(Vector(0.0, 0.0, 0.0));
  positionAlloc->Add(Vector(distance, 0.0, 0.0));
  mobility.SetPositionAllocator(positionAlloc);

  mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");

  mobility.Install(wifiApNode);
  mobility.Install(wifiStaNode);

  // Internet stack
  InternetStackHelper stack;
  stack.Install(wifiApNode);
  stack.Install(wifiStaNode);

  Ipv4AddressHelper address;
  address.SetBase("192.168.1.0", "255.255.255.0");
  Ipv4InterfaceContainer staNodeInterface;
  Ipv4InterfaceContainer apNodeInterface;

  staNodeInterface = address.Assign(staDevice);
  apNodeInterface = address.Assign(apDevice);

  // Create a UDP client-server  application
  NS_LOG_INFO("Create UDP  applications");
  uint32_t maxPacketSize = 1400;  // Byte
  Time interPacketInterval = Seconds(0.00002); // The smaller the value, the more packets are to be sent.
  uint32_t maxPacketCount = 10000000;

  ApplicationContainer serverApp;

  uint16_t port = 50000; // Use a private port beyond the registered ports by IANA
  UdpServerHelper server(port);
  serverApp = server.Install(wifiStaNode.Get(0)); // Install the UDP server app on the Wi-Fi STN node
  serverApp.Start(Seconds(0.0));
  serverApp.Stop(Seconds(simulationTime));

  UdpClientHelper client(staNodeInterface.GetAddress(0), port);
  client.SetAttribute("MaxPackets", UintegerValue(maxPacketCount));
  client.SetAttribute("Interval", TimeValue(interPacketInterval));
  client.SetAttribute("PacketSize", UintegerValue(maxPacketSize));

  ApplicationContainer clientApp = client.Install(wifiApNode.Get(0)); // Install the UDP client app on the Wi-Fi AP node
  clientApp.Start(Seconds(0.0));
  clientApp.Stop(Seconds(simulationTime));

  // Ipv4GlobalRoutingHelper::PopulateRoutingTables (); // Not needed for this example
  // ns3::LogComponentEnable ("UdpClient", LOG_LEVEL_INFO);  // Enable to see the results
  // ns3::LogComponentEnable ("UdpServer", LOG_LEVEL_INFO); // Enable to see the results

  NS_LOG_INFO("Run Simulation.");
  Simulator::Stop(Seconds(simulationTime + 1));
  Simulator::Run();

  // Calculate the actual throughput
  uint64_t rxBytes = 0;
  uint32_t payloadSize = 1472;   // UDP packet length in bytes
  rxBytes = payloadSize * DynamicCast<UdpServer>(serverApp.Get(0))->GetReceived(); // Calculate received data at the UDP server in bytes
  double throughput = (rxBytes * 8) / (simulationTime * 1000000.0);   // Mbps
  Simulator::Destroy();

  std::cout << mcs << "\t\t\t" << channelWidth << " MHz\t\t\t" << sgi << "\t\t\t" << throughput << " Mbps" << std::endl;
}
