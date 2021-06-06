"""
Your awesome Distance Vector router for CS 168
"""

import sim.api as api
import sim.basics as basics

# We define infinity as a distance of 16.
INFINITY = 16


class DVRouter(basics.DVRouterBase):
    NO_LOG = True  # Set to True on an instance to disable its logging
    POISON_MODE = True  # Can override POISON_MODE here
    DEFAULT_TIMER_INTERVAL = 5  # Can override this yourself for testing
    TIMEOUT = 15

    def __init__(self):
        """
        Called when the instance is initialized.

        You probably want to do some additional initialization here.
        """
        # port to latency mapping
        self.ports = {}
        # host to (destination, port, latency, receive_time)
        self.table = {}
        self.start_timer()  # Starts calling handle_timer() at correct rate

    def handle_link_up(self, port, latency):
        """
        Called by the framework when a link attached to this Entity goes up.

        The port attached to the link and the link latency are passed in.
        """
        # add new link to port-weight dict
        self.ports[port] = latency
        # tell new link what I can reach with what costs
        for host in self.table:
            info = basics.RoutePacket(host, self.table[host][2])  # 2 is index of latency
            self.send(info, port)

    def handle_link_down(self, port):
        """
        Called by the framework when a link attached to this Entity does down.

        The port number used by the link is passed in.
        """
        # remove port from our remembered ports
        if port in self.ports:
            self.ports.pop(port)

        for host in self.table:  # for host in hosts
            if port == self.table[host][1]:  # if I could reach that entity from this port
                self.table.pop(host)  # remove that route from my table
                if self.POISON_MODE:
                    info = basics.RoutePacket(host, INFINITY)  # send that host poisoned route
                    self.send(info, port, flood=True)  # with flood mode packets are sent from all ports except listed

    def handle_rx(self, packet, port):
        """
        Called by the framework when this Entity receives a packet.

        packet is a Packet (or subclass).
        port is the port number it arrived on.

        You definitely want to fill this in.
        """
        # self.log("RX %s on %s (%s)", packet, port, api.current_time())
        if isinstance(packet, basics.RoutePacket):
            # if we dont have that destination in table
            # or if we have it but new route has cheaper cost
            # or if cost changed due to congestion or some other reason
            # we should update our table
            new_cost = self.ports[port] + packet.latency
            if packet.destination not in self.table or \
                    new_cost <= self.table[packet.destination][2] or \
                    self.table[packet.destination][1] == port:
                self.table[packet.destination] = (packet.dst, port, new_cost, api.current_time())
                update_packet = basics.RoutePacket(packet.destination, new_cost)
                self.send(update_packet, port, flood=True)
        elif isinstance(packet, basics.HostDiscoveryPacket):
            # if I don't know about the host or if the new way is cheaper I update the table
            new_cost = self.ports[port]
            src = packet.src
            if src not in self.table or new_cost < self.table[src]:
                self.table[packet.src] = (packet.dst, port, new_cost, api.current_time())
                # tell other routers about update
                update_packet = basics.RoutePacket(packet.src, new_cost)
                self.send(update_packet, port, flood=True)
        else:
            # it is neither routePacket nor hostDiscover, lets just forward it
            dest = packet.dst
            if dest in self.table:
                _, dest_port, dest_latency, _ = self.table[dest]
                if dest_latency < INFINITY and port != dest_port:
                    self.send(packet, dest_port)

    def handle_timer(self):
        """
        Called periodically.

        When called, your router should send tables to neighbors.  It also might
        not be a bad place to check for whether any entries have expired.
        """
        self.remove_expired()
        # action after expiring

        for host in self.table:
            # detect neighbor with following logic:
            # if route to neighbor has same latency as one hop from that route's port then it is a neighbor
            route_port = self.table[host][1]
            if route_port in self.ports and self.ports[route_port] == self.table[host][2]:
                self.send_table(route_port)

    def remove_expired(self):
        expired = []  # to not mutate collection while iterating
        for host in self.table:
            if api.current_time() - self.table[host][3] < self.TIMEOUT:
                expired.append(host)
        for host in expired:
            self.table.pop(host)

    def send_table(self, route_port):
        for host in self.table:
            info = basics.RoutePacket(host, self.table[host][2])  # 2 is index of latency
            self.send(info, route_port)
