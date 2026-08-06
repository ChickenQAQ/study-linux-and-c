#!/usr/bin/env python3
"""Deeper network diagnostics for Trae CDN access"""
import paramiko, socket, time

ssh = paramiko.SSHClient()
ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy())
ssh.connect('172.31.212.123', username='chicken', password='123456', timeout=15)

def run(cmd, timeout=30):
    stdin, stdout, stderr = ssh.exec_command(cmd, timeout=timeout)
    ec = stdout.channel.recv_exit_status()
    out = stdout.read().decode('utf-8', errors='replace').strip()
    err = stderr.read().decode('utf-8', errors='replace').strip()
    print(f'$ {cmd}')
    if out: print(out[:4000])
    if err: print(f'[e] {err[:500]}')
    print()

print("="*60)
print("1. Trying alternative DNS resolution")
print("="*60)
run("getent hosts lf-cdn.trae.com.cn 2>/dev/null || echo 'getent failed'")
run("ping -c 2 -W 3 lf-cdn.trae.com.cn 2>&1 || echo 'ping failed'")

print("="*60)
print("2. Testing with alternative CDN hosts")
print("="*60)
# Try different CDN patterns
run("curl -v --connect-timeout 10 'https://lf-cdn.trae.com.cn/obj/trae-com-cn/pkg/server/releases/stable/9b7a82724b02c7e595df5219c0c69d3ff3055458/linux-debian10/Trae%20CN-linux-x64-1191350185730_10.tar.xz.md5' -o /dev/null 2>&1 | head -30")

print("="*60)
print("3. Test raw IP connectivity (Google DNS)")
print("="*60)
run("curl -v --connect-timeout 5 https://8.8.8.8 -o /dev/null 2>&1 | head -10 || echo '8.8.8.8 unreachable'")

print("="*60)
print("4. Check routing/MTU")
print("="*60)
run("ip route show default 2>/dev/null || route -n 2>/dev/null || echo 'no route info'")
run("cat /etc/resolv.conf 2>/dev/null || echo 'no resolv.conf'")

print("="*60)
print("5. Test HTTP (not HTTPS) connectivity")
print("="*60)
run("curl -v --connect-timeout 10 'http://lf-cdn.trae.com.cn/obj/trae-com-cn/pkg/server/releases/stable/9b7a82724b02c7e595df5219c0c69d3ff3055458/linux-debian10/Trae%20CN-linux-x64-1191350185730_10.tar.xz.md5' -o /dev/null 2>&1 | head -20")

print("="*60)
print("6. Test via alternative CDN (jsdelivr or github)")
print("="*60)
run("curl -v --connect-timeout 10 https://github.com -o /dev/null 2>&1 | head -5")
run("curl -v --connect-timeout 10 https://cdn.jsdelivr.net -o /dev/null 2>&1 | head -5")

print("="*60)
print("7. Check iptables/firewall rules")
print("="*60)
run("iptables -L -n 2>/dev/null | head -20 || echo 'no iptables access'")
run("nft list ruleset 2>/dev/null | head -20 || echo 'no nftables'")

print("="*60)
print("8. Check if Trae has alternative CDN prefixes")
print("="*60)
# Check Trae's config for alternative URLs
run("find / -name '*.json' -path '*trae*' 2>/dev/null | head -5")

print("="*60)
print("Network diagnostics complete")
print("="*60)

ssh.close()
