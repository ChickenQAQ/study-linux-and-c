#!/usr/bin/env python3
"""Check what network services the WSL instance can actually reach"""
import paramiko

ssh = paramiko.SSHClient()
ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy())
ssh.connect('172.31.212.123', username='chicken', password='123456', timeout=15)

def run(cmd, timeout=15):
    print(f'$ {cmd}')
    stdin, stdout, stderr = ssh.exec_command(cmd, timeout=timeout)
    ec = stdout.channel.recv_exit_status()
    out = stdout.read().decode('utf-8', errors='replace').strip()
    err = stderr.read().decode('utf-8', errors='replace').strip()
    if out: print(out[:2000])
    if err: print(f'[e] {err[:500]}')
    print(f'→ exit: {ec}')
    return ec, out, err

print("=" * 60)
print("🔍 Network connectivity diagnostics")
print("=" * 60)

# Check gateway
run("ping -c 2 -W 2 172.31.208.1 2>&1", timeout=10)
run("ping -c 1 -W 2 172.31.208.1 2>&1", timeout=10)

# Check if Windows host is reachable (WSL2 gateway)
run("ip route show default")

# Check if there's a Windows-side WSL gateway
run("cat /etc/resolv.conf")
run("resolvectl status")

# Try HTTP (not HTTPS) to see if it's an SSL issue or general connectivity
run("curl -sS --connect-timeout 5 'http://www.baidu.com' -o /dev/null -w 'HTTP %{http_code} (%{time_total}s)' 2>&1", timeout=10)
run("curl -sS --connect-timeout 5 'http://1.1.1.1' -o /dev/null -w 'HTTP %{http_code} (%{time_total}s)' 2>&1", timeout=10)

# Test TCP connection to port 443 on api.trae.com.cn 
run("timeout 5 bash -c 'echo > /dev/tcp/api.trae.com.cn/443' 2>&1 || echo 'TCP 443 FAILED'", timeout=10)
run("timeout 5 bash -c 'echo > /dev/tcp/183.61.231.36/443' 2>&1 || echo 'TCP 443 FAILED via IP'", timeout=10)

# Check if we have internet at all via ICMP to IP
run("ping -c 2 -W 3 183.61.231.36 2>&1", timeout=10)

# Check if SSH tunnel (SOCKS) can be used for connectivity
# The Trae SSH session has a SOCKS proxy on the Windows side
run("ss -tlnp 2>/dev/null | head -10")

# See if any HTTP proxy is configured
run("env | grep -i proxy")
run("cat /etc/environment 2>/dev/null || echo 'no /etc/environment'")

# Check if the WSL host (Windows) has internet
# Try WSL's auto-resolving gateway
run("ip -4 addr show eth0 2>/dev/null")

# Check Windows host name resolution from WSL
run("hostname")

ssh.close()
print("\n" + "=" * 60)
print("Diagnostics complete")
print("=" * 60)
