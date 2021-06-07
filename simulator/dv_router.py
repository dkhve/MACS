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
        # entity to (port, latency, entry_time) mapping
        self.table = {}
        # ports on which are neighboring hosts
        self.neighboring_hosts = set()
        # ports on which are neighboring routers
        self.neighboring_router_ports = set()
        self.start_timer()  # Starts calling handle_timer() at correct rate

    def handle_link_up(self, port, latency):
        """
        Called by the framework when a link attached to this Entity goes up.

        The port attached to the link and the link latency are passed in.
        """
        self.ports[port] = latency  # add new link to port-weight dict
        self.neighboring_router_ports.add(port)

        # tell new link what I can reach with what costs
        for entity in self.table:
            info = basics.RoutePacket(entity, self.table[entity][1])  # 1 is index of latency
            self.send(info, port)

    def handle_link_down(self, port):
        """
        Called by the framework when a link attached to this Entity does down.

        The port number used by the link is passed in.
        """
        # remove port from our remembered ports
        if port in self.ports:
            self.ports.pop(port)
        if port in self.neighboring_router_ports:
            self.neighboring_router_ports.remove(port)
        if port in self.neighboring_hosts:
            self.neighboring_hosts.remove(port)

        to_be_removed = []  # to not mutate collection while iterating
        for entity in self.table:  # for entity in entities
            if port == self.table[entity][0]:  # if I could reach that entity through this port
                to_be_removed.append(entity)  # remove that route from my table
                if self.POISON_MODE:
                    # send poisoned packets to tell other routers that I can't reach that entity anymore
                    info = basics.RoutePacket(entity, INFINITY)
                    self.send(info, port, flood=True)  # with flood=True packets are sent from all ports except listed

        for entity in to_be_removed:
            self.table.pop(entity)

    def handle_rx(self, packet, port):
        """
        Called by the framework when this Entity receives a packet.

        packet is a Packet (or subclass).
        port is the port number it arrived on.

        You definitely want to fill this in.
        """
        if isinstance(packet, basics.RoutePacket):
            dest = packet.destination
            new_cost = packet.latency + self.ports[port]
            # momivida mesiji rom me ama da am hosts vwvdebi ama da am costito
            # tu upro metit vwvdebodi me an tu saertod ver vwvdebodi mashin davaupdateb entriebs da gavugzavni mezoblebs
            # tu am portidan vwvdebodi, mashin, axali costia es da unda gavugzavno mezoblebs
            # tu es costi > INFINITY, mashin, vegar vwvdebi da unda wavshalo da poison gavugzavno
            if dest not in self.table:
                if new_cost < INFINITY:
                    self.table[dest] = (port, new_cost, api.current_time())
                    info = basics.RoutePacket(dest, new_cost)
                    self.send(info, port, flood=True)
            else:
                old_port, old_cost, _ = self.table[dest]
                if old_cost >= new_cost:
                    self.table[dest] = (port, new_cost, api.current_time())
                    if old_cost > new_cost:
                        info = basics.RoutePacket(dest, new_cost)
                        self.send(info, port, flood=True)
                elif old_port == port:
                    if new_cost >= INFINITY:
                        self.table.pop(dest)
                        info = basics.RoutePacket(dest, new_cost)
                        self.send(info, port, flood=True)
                    else:
                        self.table[dest] = (port, new_cost, api.current_time())
                        info = basics.RoutePacket(dest, new_cost)
                        self.send(info, port, flood=True)
                elif old_cost + self.ports[port] < packet.latency:
                    # if code gets here that means that I have better alternative for sending router
                    # so let's offer it
                    info = basics.RoutePacket(dest, self.table[dest][1])
                    self.send(info, port)

        elif isinstance(packet, basics.HostDiscoveryPacket):
            # differentiate neighboring hosts from neighboring routers
            self.neighboring_hosts.add(packet.src)
            if port in self.neighboring_router_ports:
                self.neighboring_router_ports.remove(port)

            # add neighboring host to the table
            self.table[packet.src] = (port, self.ports[port], api.current_time())

            # send neighboring routers update
            info = basics.RoutePacket(packet.src, self.ports[port])
            self.send(info, port, flood=True)

        elif packet.dst in self.table and port != self.table[packet.dst][0]:
            # it is neither routePacket nor hostDiscover, lets just forward it
            # but we should not send it back in the port where it came from
            self.send(packet, self.table[packet.dst][0])  # 0 is index of port

    def handle_timer(self):
        """
        Called periodically.

        When called, your router should send tables to neighbors.  It also might
        not be a bad place to check for whether any entries have expired.
        """
        self.remove_expired()

        # send tables to neighbors
        for entity in self.table:
            info = basics.RoutePacket(entity, self.table[entity][1])  # 1 is index of latency
            self.send(info, self.table[entity][0], flood=True)

    def remove_expired(self):
        expired = []  # to not mutate collection while iterating
        for entity in self.table:
            if api.current_time() - self.table[entity][2] >= self.TIMEOUT:
                expired.append(entity)  # save expired entity for later removal
        for entity in expired:
            # should not delete neighboring hosts because they aren't expected to update their paths
            if entity not in self.neighboring_hosts:
                self.table.pop(entity)
            info = basics.RoutePacket(entity, INFINITY)  # tell others that I cant reach that entity anymore
            self.send(info, flood=True)  # sends packet to all neighbors
