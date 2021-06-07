"""
Your awesome Distance Vector router for CS 168
"""

import sim.api as api
import sim.basics as basics

# We define infinity as a distance of 16.
INFINITY = 16


class DVRouter(basics.DVRouterBase):
    NO_LOG = False  # Set to True on an instance to disable its logging
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
        # entity to (port, latency, entry_time) mapping
        self.table = {}
        # ports on which are neighboring hosts
        self.neighboring_hosts = set()
        # ports on which are neighboring routers
        self.neighboring_routers = set()
        self.start_timer()  # Starts calling handle_timer() at correct rate

    def handle_link_up(self, port, latency):
        """
        Called by the framework when a link attached to this Entity goes up.

        The port attached to the link and the link latency are passed in.
        """
        self.ports[port] = latency  # add new link to port-weight dict
        # we should send our table to new neighboring router
        # but we shouldn't send it to neighboring host
        # so let's just save everything as router and
        # we will differentiate hosts when they send hostDiscoveryPacket
        self.neighboring_routers.add(port)

    def handle_link_down(self, port):
        """
        Called by the framework when a link attached to this Entity does down.

        The port number used by the link is passed in.
        """
        pass

    def handle_rx(self, packet, port):
        """
        Called by the framework when this Entity receives a packet.

        packet is a Packet (or subclass).
        port is the port number it arrived on.

        You definitely want to fill this in.
        """
        self.log("RX %s on %s from %s trace: [%s]", packet, port, packet.src, ', '.join(map(str, packet.trace)))
        if isinstance(packet, basics.RoutePacket):
            pass
        elif isinstance(packet, basics.HostDiscoveryPacket):
            # differentiate neighboring hosts from neighboring routers
            self.neighboring_hosts.add(port)
            if port in self.neighboring_routers:
                self.neighboring_routers.remove(port)

            # add neighboring host to the table
            self.table[packet.src] = (port, self.ports[port], api.current_time())

            # send neighboring routers update
            info = basics.RoutePacket(packet.src, self.ports[port])
            self.send(info, self.neighboring_routers)

        elif packet.dst in self.table and port != self.table[packet.dst][0]:
            # it is neither routePacket nor hostDiscover, lets just forward it
            # but we should not send it back in the port where it came from
            self.log("forwarding: %s on port: %s", packet, str(self.table[packet.dst][0]))
            self.send(packet, self.table[packet.dst][0])  # 0 is index of port

    def handle_timer(self):
        """
        Called periodically.

        When called, your router should send tables to neighbors.  It also might
        not be a bad place to check for whether any entries have expired.
        """
        pass
