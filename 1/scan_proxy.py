#!/usr/bin/env python3
"""Auto-fix remote WSL internet access for Trae CN ai-agent"""
import paramiko
import socket
import sys

PW = '123456'
HOST = '172.31.212.123'
GATEWAY = '172.31.208.1'  # Windows host IP in this WSL network

ssh = paramiko.SSHClient()
ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy())
ssh.connect(HOST, username='chicken', password=PW, timeout=15)

def run(cmd, timeout=15):
    print(f'$ {cmd}')
    stdin, stdout, stderr = ssh.exec_command(cmd, timeout=timeout)
    ec = stdout.channel.recv_exit_status()
    out = stdout.read().decode('utf-8', errors='replace').strip()
    err = stderr.read().decode('utf-8', errors='replace').strip()
    if out: print(out[:2500])
    if err: print(f'[e] {err[:500]}')
    print(f'→ exit: {ec}')
    return ec, out, err

def sudo(cmd, timeout=15):
    return run(f"echo '{PW}' | sudo -S -k {cmd}", timeout)

def sudo_sh(cmd, timeout=15):
    return run(f"echo '{PW}' | sudo -S -k sh -c '{cmd}'", timeout)

print("=" * 60)
print("🔍 Step 1: Scan for proxy ports on Windows host")
print("=" * 60)

# Common proxy ports to check
proxy_ports = [7890, 10809, 1080, 8080, 3128, 8888, 8081, 8118]
found_proxy = None

for port in proxy_ports:
    ec, out, _ = run(f"timeout 3 bash -c 'echo > /dev/tcp/{GATEWAY}/{port}' 2>&1 || echo 'CLOSED'", timeout=5)
    if 'CLOSED' not in out and ec == 0:
        found_proxy = port
        print(f"✅ Found open port {port} on {GATEWAY}!")
        break
    else:
        print(f"  Port {port}: closed")

if found_proxy:
    print(f"\n✅ Proxy found on {GATEWAY}:{found_proxy}")
else:
    print(f"\n⚠️ No proxy found on common ports.")
    print("Will try alternative approach: tunnel through SSH")

print("\n" + "=" * 60)
print("🔍 Step 2: Check Trae SOCKS proxy ports")
print("=" * 60)

# Check if we can detect Trae's SOCKS proxy
# The SSH connection creates a SOCKS proxy on Windows at a dynamic port
# Look at current SSH connections
run("ss -tlnp 2>/dev/null | grep 127.0.0.1 || ss -tlnp 2>/dev/null | head -5")
run("env | grep SSH_CONNECTION")

# Check if we can reach the Trae SOCKS proxy through the SSH tunnel
# The Trae Remote-SSH creates a SOCKS proxy on Windows, then forwards remote 35941
# We might be able to create a reverse tunnel

print("\n" + "=" * 60)
print("🔍 Step 3: Check if Windows host is actually reachable")
print("=" * 60)

run(f"timeout 3 bash -c 'echo > /dev/tcp/{GATEWAY}/22' 2>&1 && echo 'SSH port open' || echo 'SSH port unreachable'", timeout=5)
run(f"timeout 3 bash -c 'echo > /dev/tcp/{GATEWAY}/80' 2>&1 && echo 'HTTP port open' || echo 'HTTP port unreachable'", timeout=5)

ssh.close()

# Now based on the results, determine the best approach
print("\n" + "=" * 60)
print("📋 Summary")
print("=" * 60)

if found_proxy:
    print(f"\n✅ Best approach: Configure HTTP proxy at {GATEWAY}:{found_proxy}")
else:
    print("\n⚠️ No proxy found. Will try SSH tunnel approach.")

print("Run the next script to apply the fix.")
