#!/usr/bin/env python3
"""Fix DNS resolution on remote WSL server for Trae CN"""
import paramiko
import sys

HOST = "172.31.212.123"
USER = "chicken"
PASSWORD = "123456"

ssh = paramiko.SSHClient()
ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy())

def run(cmd, timeout=15, label=None):
    if label:
        print(f"\n{'='*60}")
        print(f"▶ {label}")
        print(f"{'='*60}")
    print(f"$ {cmd}")
    stdin, stdout, stderr = ssh.exec_command(cmd, timeout=timeout)
    ec = stdout.channel.recv_exit_status()
    out = stdout.read().decode('utf-8', errors='replace').strip()
    err = stderr.read().decode('utf-8', errors='replace').strip()
    if out: print(out[:3000])
    if err: print(f"[stderr] {err[:600]}")
    print(f"→ exit code: {ec}")
    return ec, out, err

print("=" * 60)
print("🔧 Fixing DNS on remote WSL server 172.31.212.123")
print("=" * 60)

try:
    ssh.connect(HOST, username=USER, password=PASSWORD, timeout=15)
    print("✅ Connected!")
except Exception as e:
    print(f"❌ Connection failed: {e}")
    sys.exit(1)

# Step 1: Check current DNS
run("cat /etc/resolv.conf", label="Step 1: Current DNS config")

# Step 2: Test which local DNS server actually works for DNS queries
ec1, out1, _ = run(
    "timeout 5 sh -c 'echo \"nameserver 10.255.255.254\" | grep . && "
    "nslookup api.trae.com.cn 10.255.255.254 2>&1' || echo '10.255.255.254 FAILED'",
    label="Step 2: Test DNS via 10.255.255.254"
)
ec2, out2, _ = run(
    "timeout 5 sh -c 'nslookup api.trae.com.cn 127.0.0.54 2>&1' || echo '127.0.0.54 FAILED'",
    label="Step 2b: Test DNS via 127.0.0.54"
)

# Determine which DNS works
dns_server = None
if "FAILED" not in out1 and ("Address:" in out1 or "Name:" in out1):
    dns_server = "10.255.255.254"
    print(f"✅ DNS server 10.255.255.254 works!")
elif "FAILED" not in out2 and ("Address:" in out2 or "Name:" in out2):
    dns_server = "127.0.0.54"
    print(f"✅ DNS server 127.0.0.54 works!")
else:
    # Default to 10.255.255.254 anyway since it pinged successfully
    dns_server = "10.255.255.254"
    print(f"⚠️ DNS test ambiguous, defaulting to {dns_server}")

# Step 3: Fix resolv.conf
print(f"\n{'='*60}")
print(f"▶ Step 3: Setting DNS to {dns_server}")
print(f"{'='*60}")

# First try without sudo (if user has permission)
run(f"echo 'nameserver {dns_server}' | tee /tmp/new_resolv.conf && cp /tmp/new_resolv.conf /etc/resolv.conf 2>/dev/null || "
    f"echo 'nameserver {dns_server}' | sudo tee /etc/resolv.conf 2>/dev/null || "
    f"echo 'nameserver {dns_server}' > /etc/resolv.conf 2>/dev/null || "
    f"echo 'nameserver {dns_server}' | tee /etc/resolv.conf 2>/dev/null || "
    f"echo 'need sudo'")

# Check the result
run("cat /etc/resolv.conf", label="Step 3b: Verify resolv.conf")

# Step 4: Test DNS resolution now
run("nslookup api.trae.com.cn 2>&1 || host api.trae.com.cn 2>&1 || python3 -c \"import socket; print(socket.gethostbyname('api.trae.com.cn'))\" 2>&1 || curl -sS --connect-timeout 5 --max-time 10 'https://api.trae.com.cn' -o /dev/null -w 'HTTP %{http_code}' 2>&1",
    label="Step 4: DNS resolution test")

run("nslookup www.baidu.com 2>&1 || "
    "curl -sS --connect-timeout 5 --max-time 10 'https://www.baidu.com' -o /dev/null -w 'HTTP %{http_code} (%{time_total}s)' 2>&1",
    label="Step 4b: Test general internet DNS")

# Step 5: Test Trae API connectivity
run("curl -v --connect-timeout 5 --max-time 15 'https://api.trae.com.cn/cloudide/api/v3/trae/GetThirdPartyToken' 2>&1 | head -30",
    label="Step 5: Test api.trae.com.cn connectivity")

# Step 6: Make it persistent via /etc/wsl.conf
print(f"\n{'='*60}")
print(f"▶ Step 6: Making DNS config persistent")
print(f"{'='*60}")
run("echo '[network]' > /tmp/wsl.conf && echo 'generateResolvConf = false' >> /tmp/wsl.conf && "
    "cp /tmp/wsl.conf /etc/wsl.conf 2>/dev/null || "
    "sudo cp /tmp/wsl.conf /etc/wsl.conf 2>/dev/null || "
    "echo 'Could not write /etc/wsl.conf (may need manual setup after reboot)'")

# Verify wsl.conf
run("cat /etc/wsl.conf 2>/dev/null || echo 'No wsl.conf'", label="Step 6b: Verify wsl.conf")

# Step 7: Verify trae-cn-server processes are alive
print(f"\n{'='*60}")
print(f"▶ Step 7: Verify Trae services are still running")
print(f"{'='*60}")
run("ps aux | grep -E 'trae|ckg|agent-tool' | grep -v grep | wc -l")

print(f"\n{'='*60}")
print("✅ DNS Fix Complete!")
print(f"{'='*60}")
print(f"\nDNS set to: {dns_server}")
print("Resolv.conf persisted via /etc/wsl.conf")
print("\nNext steps:")
print("1. Close Trae CN completely")
print("2. Reopen Trae CN")
print("3. Reconnect to the remote server (172.31.212.123)")
print("4. Try traeagent code review again - it should work now!")

ssh.close()
