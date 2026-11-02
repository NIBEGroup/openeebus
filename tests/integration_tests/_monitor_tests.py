"""
Shared base class for monitoring use-case tests (MPC, MGCP, …).

Mirrors the _lp_tests.py pattern: domain logic lives here, conftest.py
stays focused on pytest infrastructure.
"""

from conftest import parse_field, DEFAULT_PROPAGATION, DEFAULT_SETTLE


class MeasurementVerifier:
    """Base class for a monitoring use case (MPC, MGCP, …).

    Subclass, set the three class-level string attributes, override the five
    marker/log methods, and call run() to drive the full set/get/verify cycle.
    A module-level singleton is the typical pattern:

        class _MpcConfig(MeasurementVerifier):
            hp_prefix = "mu_mpc"; hems_prefix = "ma_mpc"; hp_label = "MU"
            def receive_marker(self, n): return f"MA MPC Measurement received: {n}"
            …
        _CFG = _MpcConfig()

        def _set_and_verify(hp, hems, measurements):
            _CFG.run(hp, hems, measurements)
    """

    hp_prefix:   str
    hems_prefix: str
    hp_label:    str
    hems_label:  str   = "MA"
    propagation: float = DEFAULT_PROPAGATION
    settle:      float = DEFAULT_SETTLE

    def receive_marker(self, name: str) -> str: raise NotImplementedError
    def hp_settle(self,      name: str) -> str: raise NotImplementedError
    def hems_settle(self,    name: str) -> str: raise NotImplementedError
    def hp_log(self,   hp,   name: str) -> str: raise NotImplementedError
    def hems_log(self, hems, name: str) -> str: raise NotImplementedError

    def run(self, hp, hems, measurements):
        for name, value in measurements:
            pos = hems.line_count()
            hp.send(f"{self.hp_prefix} set {name} {value}")
            hems.wait_for_new(self.receive_marker(name), pos, self.propagation)

        pos_hp = hp.line_count()
        for name, _ in measurements:
            hp.send(f"{self.hp_prefix} get {name}")
        pos_hems = hems.line_count()
        for name, _ in measurements:
            hems.send(f"{self.hems_prefix} get {name}")

        last_name = measurements[-1][0]
        hp.wait_for_new(self.hp_settle(last_name), pos_hp, self.settle)
        hems.wait_for_new(self.hems_settle(last_name), pos_hems, self.settle)

        for name, expected in measurements:
            hp_val   = parse_field(self.hp_log(hp,   name), "value")
            hems_val = parse_field(self.hems_log(hems, name), "value")
            print(f"  {name}: {self.hp_label}={hp_val}  {self.hems_label}={hems_val}")
            assert hp_val   == str(expected), f"{self.hp_label} {name}: expected {expected}, got {hp_val!r}"
            assert hems_val == str(expected), f"{self.hems_label} {name}: expected {expected}, got {hems_val!r}"
