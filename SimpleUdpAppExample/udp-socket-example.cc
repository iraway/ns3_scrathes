
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/internet-module.h"
#include "ns3/csma-module.h"
#include "simple-udp-application.h"
#include "ns3/point-to-point-module.h"

using namespace ns3;

/**
This example illustrates the use of SimpleUdpApplication. It should work regardless of the device you have. 
*/

int main (int argc, char *argv[])
{
  CommandLine cmd;
  cmd.Parse (argc, argv);
  
  NodeContainer nodes0;
  nodes0.Create (4);



  PointToPointHelper pointToPoint;
   pointToPoint.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
   pointToPoint.SetChannelAttribute("Delay", StringValue("2ms"));

   NetDeviceContainer devices0;
   devices0 = pointToPoint.Install(nodes0.Get(0), nodes0.Get(1));
   NetDeviceContainer devices1;
   devices1 = pointToPoint.Install(nodes0.Get(1), nodes0.Get(2));

/*
  //NetDeviceContainer csmaDevs1 = csma.Install (nodes0);

  InternetStackHelper stack;
  stack.Install (nodes0);
  stack.Install (nodes1);

  Ipv4AddressHelper address;
  address.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer ifaces = address.Assign (csmaDevs0);
  address.SetBase ("10.20.1.0", "255.255.255.0");
  Ipv4InterfaceContainer ifaces1 = address.Assign (csmaDevs0);
  */

   InternetStackHelper stack;
   stack.Install(nodes0);

   Ipv4AddressHelper address0;
   address0.SetBase("10.1.1.0", "255.255.255.0");
   Ipv4AddressHelper address1;
   address1.SetBase("10.20.1.0", "255.255.255.0");

   Ipv4InterfaceContainer interfaces0 = address0.Assign(devices0);
   Ipv4InterfaceContainer interfaces1 = address1.Assign(devices1);

  
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
  Packet::EnablePrinting (); 

  //Create our Two UDP applications
  Ptr <SimpleUdpApplication> udp0 = CreateObject <SimpleUdpApplication> ();
  Ptr <SimpleUdpApplication> udp1 = CreateObject <SimpleUdpApplication> ();

  Ptr <SimpleUdpApplication> udp2 = CreateObject <SimpleUdpApplication> ();
 // Ptr <SimpleUdpApplication> udp3 = CreateObject <SimpleUdpApplication> ();
  
  //Set the start & stop times
  udp0->SetStartTime (Seconds(0));
  udp0->SetStopTime (Seconds (10));
  udp1->SetStartTime (Seconds(0));
  udp1->SetStopTime (Seconds (10));
  udp2->SetStartTime (Seconds(0));
  udp2->SetStopTime (Seconds (10));
 // udp3->SetStartTime (Seconds(0));
 // udp3->SetStopTime (Seconds (10));
  
  //install one application at node 0, and the other at node 1
  nodes0.Get(0)->AddApplication (udp0);
  nodes0.Get(1)->AddApplication (udp1);
  nodes0.Get(2)->AddApplication (udp2);

  //nodes1.Get(0)->AddApplication (udp2);
  //nodes1.Get(1)->AddApplication (udp3);
  
  //This is the IP address of node 1
   Ipv4Address dest_ip0 = interfaces0.GetAddress(0);
   Ipv4Address dest_ip1 = interfaces0.GetAddress(1);

   Ipv4Address dest_ip2 = interfaces1.GetAddress(1);
  //Ipv4Address dest_ip0 ("10.1.1.1");
  //Ipv4Address dest_ip1 ("10.1.1.2");

  //Ipv4Address dest_ip2 ("10.20.1.1");
  //Ipv4Address dest_ip3 ("10.20.1.2");

  uint32_t port = 7777;
  port++; port--;

  //Schedule an event to send a packet of size 400 using udp0 targeting IP of node 0, and port 7777

  Ptr <Packet> packet0 = Create <Packet> (64);
  Ptr <Packet> packet1 = Create <Packet> (128);
  Ptr <Packet> packet2 = Create <Packet> (256);
  Ptr <Packet> packet3 = Create <Packet> (512);

  //NS_LOG_INFO("from node one to node two");
  //std::cout << "from node one to node two\n";
  Simulator::Schedule (Seconds (1), &SimpleUdpApplication::SendPacket, udp0, packet0, dest_ip1, port);

  //std::cout << "from node two to node three\n";
  Simulator::Schedule (Seconds (2), &SimpleUdpApplication::SendPacket, udp1, packet1, dest_ip2, port);


  //NS_LOG_INFO("from node one to node three ");
  Simulator::Schedule (Seconds (4), &SimpleUdpApplication::SendPacket, udp0, packet2, dest_ip2, port);

  Simulator::Schedule (Seconds (4), &SimpleUdpApplication::SendPacket, udp2, packet2, dest_ip0, port);


  //Simulator::Schedule (Seconds (7), &SimpleUdpApplication::SendPacket, udp2, packet3, dest_ip3, port);
/*
  Ptr <Packet> packet1 = Create <Packet> (400);
  Simulator::Schedule (Seconds (1), &SimpleUdpApplication::SendPacket, udp2, packet1, dest_ip, port);
  //Another packet of size 800, targeting the same IP address, but a different port.
  Ptr <Packet> packet2 = Create <Packet> (800);
  Simulator::Schedule (Seconds (2), &SimpleUdpApplication::SendPacket, udp0, packet2, dest_ip2, port);

  Ptr <Packet> packet3 = Create <Packet> (199);
  Simulator::Schedule (Seconds (3), &SimpleUdpApplication::SendPacket, udp1, packet2, dest_ip3, port);
    
    */
  LogComponentEnable ("SimpleUdpApplication", LOG_LEVEL_INFO);

  Simulator::Stop (Seconds (10));
  Simulator::Run ();
  Simulator::Destroy ();

}