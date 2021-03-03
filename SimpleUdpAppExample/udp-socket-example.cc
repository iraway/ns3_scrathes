#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/internet-module.h"
#include "ns3/csma-module.h"
#include "simple-udp-application.h"

using namespace ns3;

/**
This example illustrates the use of SimpleUdpApplication. It should work regardless of the device you have. 
*/

int main (int argc, char *argv[])
{
  CommandLine cmd;
  cmd.Parse (argc, argv);
  
  NodeContainer nodes;
  nodes.Create (3);

  CsmaHelper csma;  
  csma.SetChannelAttribute ("DataRate", StringValue ("20Mbps"));
  csma.SetChannelAttribute ("Delay", TimeValue (MicroSeconds(5000)));

  NetDeviceContainer csmaDevs;
  csmaDevs = csma.Install (nodes);
  csma.EnableAsciiAll("simple-udp");

  InternetStackHelper stack;
  stack.Install (nodes);

  Ipv4AddressHelper address;
  address.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer ifaces = address.Assign (csmaDevs);

  
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
  Packet::EnablePrinting (); 

  //Create our Two UDP applications
  Ptr <SimpleUdpApplication> udp0 = CreateObject <SimpleUdpApplication> ();
  Ptr <SimpleUdpApplication> udp1 = CreateObject <SimpleUdpApplication> ();
  Ptr <SimpleUdpApplication> udp2 = CreateObject <SimpleUdpApplication> ();
  
  //Set the start & stop times
  udp0->SetStartTime (Seconds(0));
  udp0->SetStopTime (Seconds (10));
  udp1->SetStartTime (Seconds(0));
  udp1->SetStopTime (Seconds (10));
  udp2->SetStartTime (Seconds(0));
  udp2->SetStopTime (Seconds (10));
  
  //install one application at node 0, and the other at node 1
  nodes.Get(0)->AddApplication (udp0);
  nodes.Get(1)->AddApplication (udp1);
  nodes.Get(2)->AddApplication (udp2);
  
  //This is the IP address of node 1
  Ipv4Address dest_ip ("10.1.1.1");
  Ipv4Address dest_ip2 ("10.1.1.2");
  Ipv4Address dest_ip3 ("10.1.1.3");

  uint32_t port = 7777;

  //Schedule an event to send a packet of size 400 using udp0 targeting IP of node 0, and port 7777
  Ptr <Packet> packet1 = Create <Packet> (400);
  Simulator::Schedule (Seconds (1), &SimpleUdpApplication::SendPacket, udp2, packet1, dest_ip, port);
  //Another packet of size 800, targeting the same IP address, but a different port.
  Ptr <Packet> packet2 = Create <Packet> (800);
  Simulator::Schedule (Seconds (2), &SimpleUdpApplication::SendPacket, udp0, packet2, dest_ip2, port);

  Ptr <Packet> packet3 = Create <Packet> (199);
  Simulator::Schedule (Seconds (3), &SimpleUdpApplication::SendPacket, udp1, packet2, dest_ip3, port);
    
  LogComponentEnable ("SimpleUdpApplication", LOG_LEVEL_INFO);

  Simulator::Stop (Seconds (10));
  Simulator::Run ();
  Simulator::Destroy ();

}