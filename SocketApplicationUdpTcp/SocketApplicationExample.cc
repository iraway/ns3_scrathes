#include "SocketApplication.h"
#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"

using namespace ns3;

int main(int argc, char* argv[])
{
    bool udp = false;
    CommandLine cmd(__FILE__);
    cmd.AddValue ("udp",  "udp model or not",       udp);
    cmd.Parse(argc, argv);

    NodeContainer nodes0;
    nodes0.Create(3);

    PointToPointHelper pointToPoint0;
    pointToPoint0.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
    pointToPoint0.SetChannelAttribute("Delay", StringValue("2ms"));

    PointToPointHelper pointToPoint1;
    pointToPoint1.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
    pointToPoint1.SetChannelAttribute("Delay", StringValue("2ms"));

    NetDeviceContainer devices0;
    devices0 = pointToPoint0.Install(nodes0.Get(0), nodes0.Get(1));
    NetDeviceContainer devices1;
    devices1 = pointToPoint1.Install(nodes0.Get(1), nodes0.Get(2));

    InternetStackHelper stack;
    stack.Install(nodes0);

    Ipv4AddressHelper address0;
    address0.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4AddressHelper address1;
    address1.SetBase("10.20.1.0", "255.255.255.0");

    Ipv4InterfaceContainer interfaces0 = address0.Assign(devices0);
    Ipv4InterfaceContainer interfaces1 = address1.Assign(devices1);

    Ipv4GlobalRoutingHelper::PopulateRoutingTables();
    Packet::EnablePrinting();

    Ptr<SocketApplication> udp0 = CreateObject<SocketApplication>();
    Ptr<SocketApplication> udp1 = CreateObject<SocketApplication>();
    Ptr<SocketApplication> udp2 = CreateObject<SocketApplication>();

    udp0->SetUdp(udp);
    udp1->SetUdp(udp);
    udp2->SetUdp(udp);

    uint8_t startTime = 0, stopTime = 10;
    uint16_t port = 8080;

    udp0->SetStartTime(Seconds(startTime));
    udp0->SetStopTime(Seconds(stopTime));
    udp1->SetStartTime(Seconds(startTime));
    udp1->SetStopTime(Seconds(stopTime));
    udp2->SetStartTime(Seconds(startTime));
    udp2->SetStopTime(Seconds(stopTime));

    nodes0.Get(0)->AddApplication(udp0);
    nodes0.Get(1)->AddApplication(udp1);
    if(udp){
        nodes0.Get(2)->AddApplication(udp2);
    }else{
        PacketSinkHelper sink("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), port));
        ApplicationContainer sinkApp0 = sink.Install(nodes0.Get(2));
        sinkApp0.Start(Seconds(startTime));
        sinkApp0.Stop(Seconds(stopTime));

    }

    Ipv4Address dest_ip2 = interfaces1.GetAddress(1);

    Ptr<Packet> packet0 = Create<Packet>(1000);

    Simulator::Schedule(Seconds(2), &SocketApplication::SendPacket, udp0, packet0, dest_ip2, port);

    AsciiTraceHelper ascii;
    pointToPoint0.EnableAsciiAll(ascii.CreateFileStream("pcap/SocketApplicationUdpTcp/full.tr"));
    pointToPoint1.EnableAsciiAll(ascii.CreateFileStream("pcap/SocketApplicationUdpTcp/full.tr"));

    pointToPoint0.EnablePcapAll("pcap/SocketApplicationUdpTcp/full");
    pointToPoint1.EnablePcapAll("pcap/SocketApplicationUdpTcp/full");

    LogComponentEnable("SocketApplicationUdpTcp", LOG_LEVEL_ALL);
    Simulator::Stop(Seconds(stopTime));

    Simulator::Run();
    Simulator::Destroy();
}