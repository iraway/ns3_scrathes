#include "SocketApplication.h"
#include "ns3/arp-header.h"
#include "ns3/config.h"
#include "ns3/csma-net-device.h"
#include "ns3/ethernet-header.h"
#include "ns3/ipv4-header.h"
#include "ns3/log.h"
#include "ns3/simulator.h"
#include "ns3/tcp-congestion-ops.h"
#include "ns3/tcp-socket-factory.h"
#include "ns3/udp-header.h"
#include "ns3/udp-socket.h"

#define PURPLE_CODE "\033[95m"
#define CYAN_CODE "\033[96m"
#define TEAL_CODE "\033[36m"
#define BLUE_CODE "\033[94m"
#define GREEN_CODE "\033[32m"
#define YELLOW_CODE "\033[33m"
#define LIGHT_YELLOW_CODE "\033[93m"
#define RED_CODE "\033[91m"
#define BOLD_CODE "\033[1m"
#define END_CODE "\033[0m"

namespace ns3 {
NS_LOG_COMPONENT_DEFINE("SocketApplicationErrorModel");
NS_OBJECT_ENSURE_REGISTERED(SocketApplication);

TypeId
SocketApplication::GetTypeId()
{
    static TypeId tid = TypeId("ns3::SocketApplication")
                            .AddConstructor<SocketApplication>()
                            .SetParent<Application>();
    return tid;
}

TypeId
SocketApplication::GetInstanceTypeId() const
{
    return SocketApplication::GetTypeId();
}

SocketApplication::SocketApplication()
{
    m_port = 8080;
}

void SocketApplication::SetUdp(const bool u)
{
    this->udp = u;
}

SocketApplication::~SocketApplication() { }

void SocketApplication::SetupReceiveSocket(Ptr<Socket> socket, uint16_t port)
{
    InetSocketAddress local = InetSocketAddress(Ipv4Address::GetAny(), port);
    NS_LOG_INFO(local);
    if (socket->Bind(local) == -1) {
        NS_FATAL_ERROR("Failed to bind socket");
    }
}

void SocketApplication::StartApplication()
{
    TypeId tid;
    if (udp) {
        tid = TypeId::LookupByName("ns3::UdpSocketFactory");
        m_recv_socket = Socket::CreateSocket(GetNode(), tid);
        m_send_socket = Socket::CreateSocket(GetNode(), tid);
    } else {
        tid = TypeId::LookupByName("ns3::TcpNewReno");
        std::stringstream nodeId;
        nodeId << GetNode()->GetId();
        std::string specificNode = "/NodeList/" + nodeId.str() + "/$ns3::TcpL4Protocol/SocketType";
        Config::Set(specificNode, TypeIdValue(tid));
        m_recv_socket = Socket::CreateSocket(GetNode(), TcpSocketFactory::GetTypeId());
        m_send_socket = Socket::CreateSocket(GetNode(), TcpSocketFactory::GetTypeId());
    }
    SetupReceiveSocket(m_recv_socket, m_port);
    m_recv_socket->SetRecvCallback(MakeCallback(&SocketApplication::HandleRead, this));
}

void SocketApplication::HandleRead(Ptr<Socket> socket)
{
    Ptr<Packet> packet;
    Address from;
    Address localAddress;
    while ((packet = socket->RecvFrom(from))) {
        NS_LOG_INFO(TEAL_CODE << "HandleReadOne : Received a Packet of size: " << packet->GetSize() << " at time " << Now().GetSeconds() << END_CODE
                              << " from = " << InetSocketAddress::ConvertFrom(from).GetIpv4());
        NS_LOG_INFO(packet->ToString());
        state.receive++;
    }
}

void SocketApplication::SendPacket(Ptr<Packet> packet, Ipv4Address destination, uint16_t port)
{
    NS_LOG_FUNCTION(this << packet << destination << port);
    m_send_socket->Connect(InetSocketAddress(Ipv4Address::ConvertFrom(destination), port));
    m_send_socket->Send(packet);
    state.send++;
}

State SocketApplication::getState()
{
    return this->state;
}

} // namespace ns3
