# Running the SHIP Pairing Service process

Both halves of the process are in this repository, so the whole of it can be run
on one machine: `hems` asks to be trusted, `heat_pump` decides whether to trust
it (SHIP Pairing Service TS 1.0.0).

## What an administrator does

The specification has a person collect three things about the node that is to do
the trusting, and configure them into the node that wants to be trusted
(section 4.2). These examples have no display, so `pairing info` prints what a
QR code would otherwise carry (chapter 12).

## Two terminals

Generate a key pair for each node first:

```sh
for n in devA devZ; do
  openssl ecparam -name prime256v1 -genkey -noout -out $n.key
  openssl req -new -x509 -key $n.key -out $n.crt -days 1460 -sha256 \
      -subj "/CN=$n/O=OPENEEBUS"
done
```

Start the node that will be asked to trust:

```sh
./heat_pump 4712 "" devA.crt devA.key
```

```
pairing info
SHIP ID:     NIBE-HeatPump-123456789
Fingerprint: 59F1E884938BF792459452F4413723E05DBAFE66D9615FED6BA42CCEB6DB23D3
Curve:       secp256r1

pairing secret 7A37DCF81BDB50F8E92CFA4160CCB3DE
Secret set. This node now evaluates requests addressed to it.
```

Start the node that wants to be trusted, and give it what was just printed:

```sh
./hems 4713 "" devZ.crt devZ.key
```

```
pairing announce NIBE-HeatPump-123456789 59F1E88493...23D3 7A37DCF81BDB50F8E92CFA4160CCB3DE
Announcing a request for NIBE-HeatPump-123456789
  nonce:  0F3B47B304ABC83CFD66881473DA3D05
  digest: AD4BC63A0D48EE6C728B278B49706CEA7569B5BC0BEC4A6248C878D5B1BB2492
```

Within a browse interval the first node finds the request, checks that it is
addressed to it, verifies the digest against its secret, and trusts the node
that sent it:

```
pairing status
Processing addCu-requests: no
Paired by a request:       yes
```

"Processing addCu-requests: no" is the point of section 4.2, step 3: having
accepted one request, the node stops accepting others, so a later announcement
cannot displace a working pairing.

## Seeing it on the wire

The announcement is an ordinary DNS-SD service and any tool can read it:

```sh
dns-sd -B _shippairing._tcp                       # macOS
avahi-browse -r _shippairing._tcp                 # Linux
```

## What to try next

- Announce with the wrong secret. The request is discovered and rejected, and
  the first node stays unpaired.
- Announce twice with the same secret. The second is rejected as a replay,
  because its digest is already in the ring buffer (chapter 11).
- `pairing stop`, then announce again. A new request gets a new nonce and so a
  new digest, and is accepted once the first node is processing requests again.
