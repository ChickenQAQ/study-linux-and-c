#!/usr/bin/env python3
"""Debug paramiko reverse tunnel step by step"""
import paramiko, socket, select, threading, time, os, tempfile, base64, sys

PW = '123456'
HOST = '172.31.212.123'

ssh = paramiko.SSHClient()
ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy())
ssh.connect(HOST, username='chicken', password=PW, timeout=15)
print("✅ SSH connected")

def run(cmd, timeout=10):
    try:
        stdin, stdout, stderr = ssh.exec_command(cmd, timeout=timeout)
        ec = stdout.channel.recv_exit_status()
        out = stdout.read().decode('utf-8', errors='replace').strip()
        err = stderr.read().decode('utf-8', errors='replace').strip()
        if out: print(f"  OUT: {out[:300]}")
        if err: print(f"  ERR: {err[:200]}")
        return ec, out, err
    except Exception as e:
        print(f"  ERROR: {e}")
        return -1, "", str(e)

# Step 1: Check if AllowTcpForwarding is enabled
print("\n[1] Checking SSH server configuration...")
run("cat /etc/ssh/sshd_config 2>/dev/null | grep -i -E 'AllowTcpForwarding|GatewayPorts|PermitOpen' || echo 'Not configured (defaults apply)'")

# Step 2: Check if we can forward a port
print("\n[2] Getting private key...")
run("cat ~/.ssh/id_ed25519 | head -1")
run("cat ~/.ssh/id_ed25519.pub")

# Step 3: Try transport + port forward
print("\n[3] Testing transport + port forward...")

key_file = os.path.join(tempfile.gettempdir(), 'trae_debug_key')
stdin, stdout, stderr = ssh.exec_command("cat ~/.ssh/id_ed25519", timeout=5)
key_data = stdout.read().decode()
with open(key_file, 'w') as f: f.write(key_data)
os.chmod(key_file, 0o600)

from paramiko import Ed25519Key
pkey = Ed25519Key(filename=key_file)

transport = paramiko.Transport((HOST, 22))
transport.connect(username='chicken', pkey=pkey)
print("  ✅ Transport connected!")

# Test simple forward handler
def test_handler(chan, src_addr, dest_addr):
    print(f"  🔄 Forward: {src_addr} -> {dest_addr}")
    chan.close()

print("  Requesting port forward on remote:18888...")
try:
    transport.request_port_forward('127.0.0.1', 18888, test_handler)
    print("  ✅ Port forward active on remote:18888")
    
    # Test if port is open
    print("  Testing remote port...")
    time.sleep(0.3)
    ec, out, err = run("timeout 2 bash -c 'echo test > /dev/tcp/127.0.0.1/18888' 2>&1 && echo 'OPEN' || echo 'CLOSED'", timeout=5)
    
    if 'OPEN' in out:
        print("  ✅ Port is OPEN and forwarding!")
    else:
        print(f"  ❌ Port not accessible: {out}")
    
    # Clean up
    transport.cancel_port_forward('127.0.0.1', 18888)
    print("  ✅ Forward cancelled")
    
except Exception as e:
    print(f"  ❌ Failed: {type(e).__name__}: {e}")

transport.close()
ssh.close()
print("\n=== DONE ===")
