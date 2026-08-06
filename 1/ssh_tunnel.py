#!/usr/bin/env python3
"""
Simple approach: system SSH + Python proxy
Uses subprocess to run Windows' own ssh.exe for the reverse tunnel
"""
import subprocess, threading, socket, select, time, os, sys, signal
from http.server import HTTPServer, BaseHTTPRequestHandler
import urllib.parse

PW = '123456'
HOST = '172.31.212.123'
PROXY_PORT = 8889
KEY_FILE = os.path.join(os.environ['TEMP'], 'trae_tunnel_key')

# ====== HTTP Proxy (Python stdlib) ======
class ProxyHandler(BaseHTTPRequestHandler):
    def do_CONNECT(self):
        host, port = self.path.split(':')
        port = int(port)
        try:
            remote = socket.create_connection((host, port), timeout=15)
            self.send_response(200, 'Connection Established')
            self.end_headers()
            self.connection.setblocking(0)
            remote.setblocking(0)
            while True:
                r, _, _ = select.select([self.connection, remote], [], [], 30)
                if not r: break
                for s in r:
                    try:
                        d = s.recv(65536)
                        if not d: return
                        (self.connection if s is remote else remote).sendall(d)
                    except: return
        except:
            try: self.send_error(502)
            except: pass
        finally:
            try: remote.close()
            except: pass
    
    def log_message(self, format, *args):
        pass  # Quiet

print("=" * 60)
print("🔧 Trae CN Proxy Tunnel - System SSH approach")
print("=" * 60)

# Step 1: Start HTTP proxy
print("\n[1] Starting HTTP proxy...")
proxy = HTTPServer(('127.0.0.1', PROXY_PORT), ProxyHandler)
proxy_thread = threading.Thread(target=proxy.serve_forever, daemon=True)
proxy_thread.start()
time.sleep(0.2)
print(f"  ✅ HTTP proxy on 127.0.0.1:{PROXY_PORT}")

# Step 2: Get SSH key from remote
print("\n[2] Getting SSH key...")
import paramiko
ssh = paramiko.SSHClient()
ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy())
ssh.connect(HOST, username='chicken', password=PW, timeout=15)

stdin, stdout, stderr = ssh.exec_command("cat ~/.ssh/id_ed25519", timeout=5)
key_data = stdout.read().decode()
with open(KEY_FILE, 'w') as f:
    f.write(key_data)
# Windows doesn't use chmod, but we need the key to have proper format
print(f"  ✅ Key saved to {KEY_FILE}")

# Step 3: Write proxy config on remote
print("\n[3] Configuring remote proxy settings...")
import base64
script = (
    'export http_proxy=http://127.0.0.1:8889\n'
    'export https_proxy=http://127.0.0.1:8889\n'
    'export HTTP_PROXY=http://127.0.0.1:8889\n'
    'export HTTPS_PROXY=http://127.0.0.1:8889\n'
    'export no_proxy=localhost,127.0.0.1,10.0.0.0/8,172.16.0.0/12,192.168.0.0/16\n'
    'export NO_PROXY=localhost,127.0.0.1,10.0.0.0/8,172.16.0.0/12,192.168.0.0/16\n'
)
b64 = base64.b64encode(script.encode()).decode()
stdin, stdout, stderr = ssh.exec_command(
    f"echo '{PW}' | sudo -S -k sh -c \"echo '{b64}' | base64 -d > /etc/profile.d/trae-proxy.sh && chmod +x /etc/profile.d/trae-proxy.sh\"",
    timeout=10
)
print(f"  OUT: {stdout.read().decode().strip()}")
print(f"  ERR: {stderr.read().decode().strip()}")
print("  ✅ Proxy config written")

# Get host key for SSH
stdin, stdout, stderr = ssh.exec_command("ssh-keyscan -t ed25519 127.0.0.1 2>/dev/null || ssh-keyscan 127.0.0.1 2>/dev/null", timeout=5)
host_key = stdout.read().decode()
known_hosts = os.path.join(os.environ['USERPROFILE'], '.ssh', 'known_hosts')
os.makedirs(os.path.dirname(known_hosts), exist_ok=True)
# Add to known_hosts to avoid prompt
with open(known_hosts, 'a') as f:
    f.write(host_key)

ssh.close()

# Step 4: Start SSH reverse tunnel using system ssh
print("\n[4] Starting SSH reverse tunnel...")
ssh_cmd = [
    'ssh.exe',  # Windows OpenSSH
    '-o', 'StrictHostKeyChecking=no',
    '-o', 'UserKnownHostsFile=NUL',
    '-o', 'ServerAliveInterval=10',
    '-o', 'ServerAliveCountMax=3',
    '-i', KEY_FILE,
    '-R', f'127.0.0.1:{PROXY_PORT}:127.0.0.1:{PROXY_PORT}',
    '-N',  # No command, just tunnel
    f'chicken@{HOST}'
]

print(f"  Running: ssh -R {PROXY_PORT}:127.0.0.1:{PROXY_PORT} -N chicken@{HOST}")
proc = subprocess.Popen(
    ssh_cmd,
    stdout=subprocess.PIPE,
    stderr=subprocess.PIPE,
    creationflags=subprocess.CREATE_NO_WINDOW if hasattr(subprocess, 'CREATE_NO_WINDOW') else 0
)

time.sleep(2)

# Check if process is running
if proc.poll() is None:
    print("  ✅ SSH tunnel established!")
else:
    stdout_data = proc.stdout.read().decode() if proc.stdout else ''
    stderr_data = proc.stderr.read().decode() if proc.stderr else ''
    print(f"  ❌ SSH failed (exit code {proc.returncode})")
    if stderr_data: print(f"  STDERR: {stderr_data[:300]}")

# Step 5: Test
if proc.poll() is None:
    print("\n[5] Testing tunnel...")
    
    ssh = paramiko.SSHClient()
    ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy())
    ssh.connect(HOST, username='chicken', password=PW, timeout=15)
    
    def runtest(cmd, timeout=25):
        stdin, stdout, stderr = ssh.exec_command(cmd, timeout=timeout)
        ec = stdout.channel.recv_exit_status()
        out = stdout.read().decode().strip()
        err = stderr.read().decode().strip()
        print(f"  {out[:300] if out else '(empty)'}")
        if err: print(f"  ERR: {err[:200]}")
        return ec
    
    print("  --- Test 1: Port open ---")
    runtest("timeout 3 bash -c 'echo > /dev/tcp/127.0.0.1/8889' 2>&1 && echo 'PORT OK' || echo 'PORT FAIL'", 5)
    
    print("  --- Test 2: HTTP baidu ---")
    runtest("curl -sS --connect-timeout 10 --max-time 15 --proxy http://127.0.0.1:8889 "
            "'http://www.baidu.com' -o /dev/null -w 'HTTP %{http_code} (%{time_total}s)'", 20)
    
    print("  --- Test 3: HTTPS baidu ---")
    runtest("curl -sS --connect-timeout 10 --max-time 15 --proxy http://127.0.0.1:8889 "
            "'https://www.baidu.com' -o /dev/null -w 'HTTP %{http_code} (%{time_total}s)'", 20)
    
    print("  --- Test 4: api.trae.com.cn ---")
    runtest("curl -sS --connect-timeout 10 --max-time 15 --proxy http://127.0.0.1:8889 "
            "'https://api.trae.com.cn' -o /dev/null -w 'HTTP %{http_code} (%{time_total}s)'", 20)
    
    print("  --- Test 5: trae-api-cn.mchost.guru ---")
    runtest("curl -sS --connect-timeout 10 --max-time 15 --proxy http://127.0.0.1:8889 "
            "'https://trae-api-cn.mchost.guru' -o /dev/null -w 'HTTP %{http_code} (%{time_total}s)'", 20)
    
    ssh.close()
    
    print("\n" + "=" * 60)
    print("✅ Tunnel ACTIVE!")
    print("=" * 60)
    print("""
Keep this window open. Then:
1. RESTART Trae CN (close & reopen)
2. Reconnect to 172.31.212.123
3. Try traeagent code analysis

Press Enter to stop the tunnel.
""")
    input()

# Cleanup
print("\n🛑 Stopping...")
try:
    proc.terminate()
    proc.wait(timeout=5)
except:
    proc.kill()
proxy.shutdown()
print("✅ Done")
