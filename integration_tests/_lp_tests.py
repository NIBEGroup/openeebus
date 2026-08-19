"""
Shared LP (LPC / LPP) scenario logic.

Functions accept hp, hems, uc ("lpc" or "lpp") and variant-specific
numeric values. CLI commands and log markers are derived from uc so the
same body works for both Limitation of Power Consumption and Production.
"""

from conftest import parse_field, parse_pt, DEFAULT_PROPAGATION, DEFAULT_SETTLE

_PROPAGATION = DEFAULT_PROPAGATION
_SETTLE      = DEFAULT_SETTLE


def _check_failsafe_limit(hp, hems, uc, expected):
    uc_up = uc.upper()
    pos_hp, pos_hems = hp.line_count(), hems.line_count()
    hp.send(f"cs_{uc} get failsafe_limit")
    hems.send(f"eg_{uc} get failsafe_limit")
    hp.wait_for_new(f"CS {uc_up} Failsafe Active Power Limit: value", pos_hp, _SETTLE)
    hems.wait_for_new(f"EG {uc_up} Failsafe Active Power Limit: value", pos_hems, _SETTLE)
    cs = parse_field(hp.last_line_with(f"CS {uc_up} Failsafe Active Power Limit"), "value")
    eg = parse_field(hems.last_line_with(f"EG {uc_up} Failsafe Active Power Limit"), "value")
    print(f"  failsafe_limit (W): CS={cs}  EG={eg}")
    assert cs == str(expected), f"CS failsafe limit: expected {expected}, got {cs!r}"
    assert eg == str(expected), f"EG failsafe limit: expected {expected}, got {eg!r}"


def _check_failsafe_duration(hp, hems, uc, expected):
    uc_up = uc.upper()
    pos_hp, pos_hems = hp.line_count(), hems.line_count()
    hp.send(f"cs_{uc} get failsafe_duration")
    hems.send(f"eg_{uc} get failsafe_duration")
    hp.wait_for_new(f"CS {uc_up} Failsafe Duration Minimum:", pos_hp, _SETTLE)
    hems.wait_for_new(f"EG {uc_up} Failsafe Duration Minimum:", pos_hems, _SETTLE)
    cs = parse_pt(hp.last_line_with(f"CS {uc_up} Failsafe Duration Minimum"))
    eg = parse_pt(hems.last_line_with(f"EG {uc_up} Failsafe Duration Minimum"))
    print(f"  failsafe_duration: CS={cs}  EG={eg}")
    assert cs == expected, f"CS failsafe duration: expected {expected}, got {cs!r}"
    assert eg == expected, f"EG failsafe duration: expected {expected}, got {eg!r}"


def lp_s1_failsafe_power_limit(hp, hems, uc, limit):
    uc_up = uc.upper()
    pos_hems = hems.line_count()
    hp.send(f"cs_{uc} set failsafe_limit {limit} true")
    hems.wait_for_new(f"EG {uc_up} Failsafe Active Power Limit received", pos_hems, _PROPAGATION)
    _check_failsafe_limit(hp, hems, uc, limit)


def lp_s1_failsafe_duration(hp, hems, uc):
    uc_up = uc.upper()
    pos_hems = hems.line_count()
    hp.send(f"cs_{uc} set failsafe_duration PT2H true")
    hems.wait_for_new(f"EG {uc_up} Failsafe Duration Minimum received", pos_hems, _PROPAGATION)
    _check_failsafe_duration(hp, hems, uc, "PT2H")


def _check_active_power_limit(hp, hems, uc, value, *, check_duration=False):
    uc_up = uc.upper()
    pos_hp, pos_hems = hp.line_count(), hems.line_count()
    hp.send(f"cs_{uc} get power_limit")
    hems.send(f"eg_{uc} get power_limit")
    hp.wait_for_new(f"CS {uc_up} Active Power Limit: value", pos_hp, _SETTLE)
    hems.wait_for_new(f"EG {uc_up} Active Power Limit: value", pos_hems, _SETTLE)
    cs = hp.last_line_with(f"CS {uc_up} Active Power Limit", exclude="Failsafe")
    eg = hems.last_line_with(f"EG {uc_up} Active Power Limit", exclude="Failsafe")
    cs_val = parse_field(cs, "value")
    eg_val = parse_field(eg, "value")
    cs_act = parse_field(cs, "is active")  # CS log uses "is active"
    eg_act = parse_field(eg, "active")     # EG log uses "active"
    if check_duration:
        cs_dur = parse_field(cs, "duration")
        eg_dur = parse_field(eg, "duration")
        print(f"  active_limit (W): CS={cs_val}  EG={eg_val}  dur: CS={cs_dur}  EG={eg_dur}  active: CS={cs_act}  EG={eg_act}")
        assert cs_dur == "PT0S", f"CS duration: expected PT0S, got {cs_dur!r}"
        assert eg_dur == "PT0S", f"EG duration: expected PT0S, got {eg_dur!r}"
    else:
        print(f"  active_limit (W): CS={cs_val}  EG={eg_val}  active: CS={cs_act}  EG={eg_act}")
    assert cs_val == str(value), f"CS value: expected {value}, got {cs_val!r}"
    assert eg_val == str(value), f"EG value: expected {value}, got {eg_val!r}"
    assert cs_act == "true",     f"CS is active: expected true, got {cs_act!r}"
    assert eg_act == "true",     f"EG active: expected true, got {eg_act!r}"


def lp_s2_active_power_limit(hp, hems, uc, value):
    uc_up = uc.upper()
    pos_hp = hp.line_count()
    hems.send(f"eg_{uc} set power_limit {value} PT0S true")
    hp.wait_for_new(f"CS {uc_up} Power Limit received", pos_hp, _PROPAGATION)
    _check_active_power_limit(hp, hems, uc, value, check_duration=True)


def lp_s3_eg_writes_failsafe_limit(hp, hems, uc):
    uc_up = uc.upper()
    pos_hp = hp.line_count()
    hems.send(f"eg_{uc} set failsafe_limit 600")
    hp.wait_for_new(f"CS {uc_up} Failsafe Active Power Limit received", pos_hp, _PROPAGATION)
    _check_failsafe_limit(hp, hems, uc, 600)


def lp_s3_eg_writes_failsafe_duration(hp, hems, uc):
    uc_up = uc.upper()
    pos_hp = hp.line_count()
    hems.send(f"eg_{uc} set failsafe_duration PT3H")
    hp.wait_for_new(f"CS {uc_up} Failsafe Duration Minimum received", pos_hp, _PROPAGATION)
    _check_failsafe_duration(hp, hems, uc, "PT3H")


def lp_s4_nominal_max(hp, hems, uc, value):
    uc_up = uc.upper()
    pos_hems = hems.line_count()
    hp.send(f"cs_{uc} set nominal_max {value}")
    hems.wait_for_new(f"EG {uc_up} Power Nominal Max received", pos_hems, _PROPAGATION)

    pos_hp, pos_hems = hp.line_count(), hems.line_count()
    hp.send(f"cs_{uc} get nominal_max")
    hems.send(f"eg_{uc} get power_nominal_max")
    hp.wait_for_new(f"CS {uc_up} Nominal Max: value", pos_hp, _SETTLE)
    hems.wait_for_new(f"EG {uc_up} Power Nominal Max: value", pos_hems, _SETTLE)

    cs = parse_field(hp.last_line_with(f"CS {uc_up} Nominal Max"), "value")
    eg = parse_field(hems.last_line_with(f"EG {uc_up} Power Nominal Max"), "value")
    print(f"  nominal_max (W): CS={cs}  EG={eg}")
    assert cs == str(value), f"CS nominal max: expected {value}, got {cs!r}"
    assert eg == str(value), f"EG nominal max: expected {value}, got {eg!r}"


def lp_s5_cs_announces_active_limit(hp, hems, uc, value):
    uc_up = uc.upper()
    pos_hems = hems.line_count()
    hp.send(f"cs_{uc} set power_limit {value} true true")
    hems.wait_for_new(f"EG {uc_up} Power Limit received", pos_hems, _PROPAGATION)
    _check_active_power_limit(hp, hems, uc, value)
