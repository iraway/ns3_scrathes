#include "SocketApplication.h"
#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/stats-module.h"
#include <ctime>
#include <sstream>

using namespace ns3;

const uint8_t NN = 3; // nodes number
uint16_t N = 10; // messages number

int32_t macTxA[NN];
int32_t macRxA[NN];

static uint32_t macTx = 0;
static uint32_t macRx = 0;

void MacTxTrace(std::string context, Ptr<const Packet> pkt)
{
    int n = context[10] - '0';
    macTxA[n]++;
    macTx++;
}

void MacRxTrace(std::string context, Ptr<const Packet> pkt)
{
    int n = context[10] - '0';
    macRxA[n]++;
    macRx++;
}

int main(int argc, char* argv[])
{
    std::memset(macTxA, 0, NN * sizeof(macTxA[0]));
    std::memset(macRxA, 0, NN * sizeof(macRxA[0]));

    bool udp = false;
    double errorRate = 0.3;
    bool allLogs = false;
    CommandLine cmd(__FILE__);
    cmd.AddValue("udp", "udp model or not", udp);
    cmd.AddValue("errorRate", "errorRate of channel", errorRate);
    cmd.AddValue("numMes", "Number of messages", N);
    cmd.AddValue("logs", "Show all logs", allLogs);
    cmd.Parse(argc, argv);

    Config::SetDefault("ns3::RateErrorModel::ErrorRate", DoubleValue(errorRate));
    Config::SetDefault("ns3::RateErrorModel::ErrorUnit", StringValue("ERROR_UNIT_PACKET"));

    NodeContainer nodes0;
    nodes0.Create(NN);

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

    uint8_t startTime = 0, stopTime = 100;
    uint16_t port = 8080;

    udp0->SetStartTime(Seconds(startTime));
    udp0->SetStopTime(Seconds(stopTime));
    udp1->SetStartTime(Seconds(startTime));
    udp1->SetStopTime(Seconds(stopTime));
    udp2->SetStartTime(Seconds(startTime));
    udp2->SetStopTime(Seconds(stopTime));

    nodes0.Get(0)->AddApplication(udp0);
    nodes0.Get(1)->AddApplication(udp1);
    //nodes0.Get(2)->AddApplication(udp2);

    if (udp) {
        nodes0.Get(2)->AddApplication(udp2);
    } else {
        PacketSinkHelper sink("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), port));
        ApplicationContainer sinkApp0 = sink.Install(nodes0.Get(2));
        sinkApp0.Start(Seconds(startTime));
        sinkApp0.Stop(Seconds(stopTime));
    }

    ObjectFactory factory;
    factory.SetTypeId("ns3::RateErrorModel");
    Ptr<ErrorModel> em = factory.Create<ErrorModel>();
    devices1.Get(1)->SetAttribute("ReceiveErrorModel", PointerValue(em));
    devices0.Get(1)->SetAttribute("ReceiveErrorModel", PointerValue(em));

    Ipv4Address dest_ip2 = interfaces1.GetAddress(1);
    Ptr<Packet> packet0 = Create<Packet>(1000);

    double time = 0, td = (stopTime - startTime - 0.5) / N;
    for (size_t i = 0; i < N; i++) {
        time += td;
        Simulator::Schedule(Seconds(time), &SocketApplication::SendPacket, udp0, packet0, dest_ip2, port);
    }

    AsciiTraceHelper ascii;
    pointToPoint0.EnableAsciiAll(ascii.CreateFileStream("pcap/SocketApplicationErrorModel/full.tr"));
    pointToPoint1.EnableAsciiAll(ascii.CreateFileStream("pcap/SocketApplicationErrorModel/full.tr"));

    pointToPoint0.EnablePcapAll("pcap/SocketApplicationErrorModel/full");
    pointToPoint1.EnablePcapAll("pcap/SocketApplicationErrorModel/full");

    if (allLogs) {
        LogComponentEnable("SocketApplicationErrorModel", LOG_LEVEL_ALL);
    }
    Simulator::Stop(Seconds(stopTime));

    //Config::SetDefault ("ns3::TcpSocket::DelAckCount", UintegerValue (100));
    // Config::SetDefault ("ns3::TcpSocket::DelAckTimeout", TimeValue (MicroSeconds (12000000)));

    Config::Connect("/NodeList/*/DeviceList/*/MacTx", MakeCallback(&MacTxTrace));
    Config::Connect("/NodeList/*/DeviceList/*/MacRx", MakeCallback(&MacRxTrace));

    SeedManager::SetSeed(55);
    SeedManager::SetRun(55);

    Simulator::Run();
    Simulator::Destroy();

    if (udp) {
        std::cout << "\n_____________APPLICATION STATISTICS_____________\n";
        std::cout << "Number of sended packets (for node 0) = " << udp0->getState().send << "\n";
        std::cout << "Number of received packets (for node 2) = " << udp2->getState().receive << "\n";
    }

    std::cout << "\n\n_____________TRACE STATISTICS_____________\n";
    std::cout << "Total number of sended packet (for all nodes) = " << macTx << "\n";
    std::cout << "Total number of received packet (for all nodes) = " << macRx << "\n";

    std::cout << std::endl;
    for (size_t i = 0; i < NN; i++) {
        std::cout << "Number of sended packet for node " << i << " = " << macTxA[i] << "\n";
    }
    std::cout << std::endl;
    for (size_t i = 0; i < NN; i++) {
        std::cout << "Number of received packet for node " << i << " = " << macRxA[i] << "\n";
    }
}
