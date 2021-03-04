#ifndef NS3_UDP_ARQ_APPLICATION_H
#define NS3_UDP_ARQ_APPLICATION_H
#include "ns3/application.h"
#include "ns3/socket.h"

using namespace ns3;

namespace ns3 {
class SocketApplication : public Application {
public:
    SocketApplication();
    virtual ~SocketApplication();

    static TypeId GetTypeId();
    virtual TypeId GetInstanceTypeId() const;

    void SetCheck(const bool);
    void SetUdp(const bool);
    void HandleRead(Ptr<Socket> socket);
    void SendPacket(Ptr<Packet> packet, Ipv4Address destination, uint16_t port);

private:
    void SetupReceiveSocket(Ptr<Socket> socket, uint16_t port);
    virtual void StartApplication();

    uint16_t m_port;
    bool check = false;
    bool udp = true; /**< Send by UDP or TCP */
    Ptr<Socket> m_send_socket; /**< A socket to listen on a specific port */
    Ptr<Socket> m_recv_socket; /**< A socket to receive on a specific port */
};
}

#endif