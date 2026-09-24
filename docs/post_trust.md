# Post-trust: user verification of an incoming connection

By default a service trusts a foreign SKI *before* the TLS handshake completes:
an incoming connection presenting an SKI that has not been registered is refused
at the socket. [SHIP] calls this **pre-trust**, and it is the only approach the
library offered before `EebusTrustMode` existed.

[SHIP] 5.2 also permits **post-trust** — "Trust within SME connection state
*Hello*":

> a SHIP node "A" may just temporarily accept the SKI/public key of SHIP node
> "B", present the SKI to SHIP node A's user for verification, and finally trust
> the foreign SKI if and only if the user accepts this SKI. Only the acceptance
> enables execution of further data exchange. A refusal by the user closes the
> connection.

This is what the **user verification** public key verification mode
([SHIP] 12.3.1.3) needs on an incoming connection: there is nothing to show a
user until the peer has presented its certificate, which happens during the TLS
handshake.

Note that [SHIP] 12.3.1.1 says **auto accept** — the timed pairing window driven
by `set_pairing_possible()` — "SHALL NOT be implemented by SHIP nodes that have a
user interface that would allow other verification modes, e.g. user verify". A
device with a display should therefore be using post-trust rather than the
pairing window.

## Enabling it

```c
EebusServiceConfig* cfg = EebusServiceConfigCreate(...);
EebusServiceConfigSetTrustMode(cfg, kEebusTrustModePostTrust);
```

Pre-trust remains the default. The two modes are independent of the `register`
flag: [SHIP] 5.1 states that "Only 'auto accept' affects the register flag", so a
node doing user verification leaves it false and does not call
`set_pairing_possible()`.

## The flow

1. An unknown peer connects. Its SKI is accepted **provisionally** — the peer is
   not trusted, and nothing is persisted.
2. Because the peer is not trusted, the connection enters the "hello" **PENDING**
   phase rather than READY ([SHIP] 13.4.4.1.2: READY "MUST ONLY be entered if the
   communication partner is already trusted"). Our node reports
   `connectionHello.phase = "pending"` to the peer, which keeps waiting.
3. `ShipNodeReaderInterface::on_ship_state_update` reports
   `kSmeHelloStatePendingListen` with the peer's SKI. That is the application's
   cue to ask its user. There is no separate callback for this.
4. The application answers, from any thread:
   - `EEBUS_SERVICE_APPROVE_PENDING_HANDSHAKE_WITH_SKI(service, ski)` — trust the
     peer. The connection moves to READY and proceeds to data exchange.
   - `EEBUS_SERVICE_CANCEL_PAIRING_WITH_SKI(service, ski)` — refuse it. The
     handshake is aborted with `connectionHello.phase = "aborted"` and the
     connection closes.

   Both are applied by the connection's own thread, so neither blocks the caller
   nor races the state machine.

## How long the user has

The peer's `connectionHello.waiting` value bounds the decision, not a locally
chosen timeout. `EEBUS_SERVICE_GET_PENDING_WAITING_MS_WITH_SKI()` reports how much
of it is left, so a countdown shown to a user is the real remaining time.

The library keeps that window open on the application's behalf: while PENDING it
sends prolongation requests, and [SHIP] 13.4.4.1.3 requires a peer to accept at
least two of them. With `T_hello_init` in the 60–240 s range, a peer that chose
60 s must grant at least 180 s in total.

If nobody answers, the window expires and the handshake aborts — the same outcome
as a refusal.

## Security note

[SHIP] 5.2 warns that post-trust "requires careful implementation beyond the TLS
handshake in order to not accidentally introduce security weaknesses", and
[SRIP] A.3 that a certificate "must … safely be discarded if the user does not
accept this node".

Accordingly, a provisionally accepted SKI is dropped whenever its connection
closes without having been trusted — whether it was refused, timed out, or the
peer went away. It is never persisted and never treated as registered on a
subsequent connection attempt, so a refused peer is prompted for again rather
than let in silently.

An application must still make its own decision durable: the library holds one
remote SKI in memory and does not store trust across restarts.

[SHIP]: EEBus SHIP TS Specification v1.1.0
[SRIP]: EEBUS TS SHIP Requirements For Installation Process V1.1.0
