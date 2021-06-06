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

    def __init__(self):
        """
        Called when the instance is initialized.

        You probably want to do some additional initialization here.
        """
        # port to latency mapping
        self.ports = {}
        # host to (destination, port, latency, expire_time)
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
            if port == self.table[host][1]:  # if I could reach that host from this port
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
            pass
        elif isinstance(packet, basics.HostDiscoveryPacket):
            pass
        else:
            # Totally wrong behavior for the sake of demonstration only: send
            # the packet back to where it came from!
            self.send(packet, port=port)

    def handle_timer(self):
        """
        Called periodically.

        When called, your router should send tables to neighbors.  It also might
        not be a bad place to check for whether any entries have expired.
        """
