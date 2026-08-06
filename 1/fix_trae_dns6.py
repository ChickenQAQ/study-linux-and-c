#!/usr/bin/env python3
"""Create /etc/resolv.conf using base64 to avoid shell escaping issues"""
import paramiko, base64

PW = '123456'
ssh = paramiko.SSHClient()
ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy())
ssh.connect('172.31.212.123', username='chicken', password=PW, timeout=15)

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

print("=" * 60)
print("🔧 Step 1: Create /etc/resolv.conf via base64 + sudo")
print("=" * 60)

# Create the file content, base64 encode it, then write via sudo on remote
resolv_content = "nameserver 127.0.0.53\noptions edns0 trust-ad\nsearch .\n"
b64 = base64.b64encode(resolv_content.encode()).decode()

cmd = f"echo '{PW}' | sudo -S -k sh -c \"echo '{b64}' | base64 -d > /etc/resolv.conf && chmod 644 /etc/resolv.conf\""
run(cmd)

# Verify
run("cat /etc/resolv.conf")
run("ls -la /etc/resolv.conf")

print("=" * 60)
print("🔧 Step 2: Test DNS Resolution")
print("=" * 60)
run("curl -sS --connect-timeout 5 'https://www.baidu.com' -o /dev/null -w 'HTTP %{http_code} (%{time_total}s)' 2>&1", timeout=15)
run("curl -sS --connect-timeout 5 'https://api.trae.com.cn' -o /dev/null -w 'HTTP %{http_code} (%{time_total}s)' 2>&1", timeout=15)
run("ping -c 1 -W 3 api.trae.com.cn 2>&1", timeout=10)

# Test Trae API endpoints
run("curl -sS --connect-timeout 5 'https://api.trae.com.cn/cloudide/api/v3/trae/GetThirdPartyToken' -X POST -H 'Content-Type: application/json' -d '{}' -o /dev/null -w 'HTTP %{http_code} (%{time_total}s)' 2>&1", timeout=15)

print("\n" + "=" * 60)
print("📋 Final Status")
print("=" * 60)
run("cat /etc/resolv.conf")
run("resolvectl status 2>&1 | grep -E 'DNS Server|resolv.conf'")

ssh.close()

print("\n✅ Fix script completed!")
print("If DNS works now, close Trae CN, reopen it, reconnect to the server.")
print("Then traeagent code review should work.")
