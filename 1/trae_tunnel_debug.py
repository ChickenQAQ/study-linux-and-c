#!/usr/bin/env python3
"""Step-by-step debug of the tunnel"""
import paramiko, socket, select, threading, time, os, tempfile, base64

PW = '123456'
HOST = '172.31.212.123'
PROXY_PORT = 8889

# Step 1: Start proxy
print("[1] Starting proxy...")
from http.server import HTTPServer, BaseHTTPRequestHandler
import urllib.request

class ProxyHandler(BaseHTTPRequestHandler):
    def do_CONNECT(self):
        host, port = self.path.split(':')
        try:
            remote = socket.create_connection((host, int(port)), timeout=15)
            self.send_response(200, 'Connection Established')
            self.end_headers()
            self._pipe(remote)
        except Exception as e:
            self.send_error(502, f'Bad Gateway: {e}')
    
    def do_GET(self):
        self._http_request()
    def do_POST(self):
        self._http_request()
    def do_PUT(self):
        self._http_request()
    def do_DELETE(self):
        self._http_request()
    def do_PATCH(self):
        self._http_request()
    def do_HEAD(self):
        self._http_request()
    
    def _http_request(self):
        url = self.path
        parsed = urllib.parse.urlparse(url)
        host = parsed.hostname
        port = parsed.port or 80
        path = parsed.path or '/'
        if parsed.query: path += '?' + parsed.query
        
        try:
            remote = socket.create_connection((host, port), timeout=15)
            self.headers['Host'] = host
            request_line = f'{self.command} {path} {self.request_version}\r\n'
            headers = ''.join(f'{k}: {v}\r\n' for k, v in self.headers.items())
            remote.sendall((request_line + headers + '\r\n').encode())
            while True:
                d = remote.recv(65536)
                if not d: break
                self.wfile.write(d)
            remote.close()
        except Exception as e:
            self.send_error(502, str(e))
    
    def _pipe(self, remote):
        try:
            while True:
                r, _, _ = select.select([self.connection, remote], [], [], 30)
                if not r: break
                for s in r:
                    d = s.recv(65536)
                    if not d: return
                    (self.connection if s is remote else remote).sendall(d)
        except: pass
        finally:
            try: remote.close()
            except: pass

proxy_server = HTTPServer(('127.0.0.1', PROXY_PORT), ProxyHandler)
proxy_thread = threading.Thread(target=proxy_server.serve_forever, daemon=True)
proxy_thread.start()
time.sleep(0.3)
print(f"  ✅ Proxy on 127.0.0.1:{PROXY_PORT}")

# Step 2: SSH setup
print("[2] SSH setup...")
ssh = paramiko.SSHClient()
ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy())
ssh.connect(HOST, username='chicken', password=PW, timeout=15)

stdin, stdout, stderr = ssh.exec_command("cat ~/.ssh/id_ed25519", timeout=5)
key_data = stdout.read().decode()
key_file = os.path.join(tempfile.gettempdir(), 'trae_key')
with open(key_file, 'w') as f: f.write(key_data)
os.chmod(key_file, 0o600)

from paramiko import Ed25519Key
pkey = Ed25519Key(filename=key_file)

transport = paramiko.Transport((HOST, 22))
transport.connect(username='chicken', pkey=pkey)
print("  ✅ Transport connected")

# Step 3: Port forward
print("[3] Port forward...")
def handler(chan, src_addr, dest_addr):
    try:
        ps = socket.create_connection(('127.0.0.1', PROXY_PORT), timeout=10)
        while True:
            r, _, _ = select.select([chan, ps], [], [], 30)
            if not r: break
            for s in r:
                d = s.recv(65536)
                if not d: return
                (chan if s is ps else ps).sendall(d)
    except Exception as e:
        pass
    finally:
        try: chan.close()
        except: pass
        try: ps.close()
        except: pass

transport.request_port_forward('127.0.0.1', PROXY_PORT, handler)
print("  ✅ remote:8889 -> local proxy")

# Step 4: Test
print("[4] Testing tunnel...")

def run(cmd, timeout=20):
    print(f"  Running: {cmd[:80]}...")
    stdin, stdout, stderr = ssh.exec_command(cmd, timeout=timeout)
    ec = stdout.channel.recv_exit_status()
    out = stdout.read().decode('utf-8', errors='replace').strip()
    err = stderr.read().decode('utf-8', errors='replace').strip()
    print(f"  Exit: {ec}")
    if out: print(f"  OUT: {out[:300]}")
    if err: print(f"  ERR: {err[:200]}")
    return ec, out, err

# Test 1: Basic connectivity to proxy
print("\n  --- Test 1: TCP to proxy port ---")
run("timeout 3 bash -c 'echo > /dev/tcp/127.0.0.1/8889' 2>&1 && echo 'OPEN' || echo 'CLOSED'", 5)

# Test 2: Test through proxy
print("\n  --- Test 2: HTTP via proxy ---")
run("curl -v --connect-timeout 15 --proxy http://127.0.0.1:8889 "
    "'http://www.baidu.com' -o /dev/null -w 'HTTP %{http_code} (%{time_total}s)' 2>&1 | tail -5", 25)

# Step 5: Save proxy config
print("\n[5] Saving proxy config...")
script = ('export http_proxy=http://127.0.0.1:8889\n'
          'export https_proxy=http://127.0.0.1:8889\n'
          'export HTTP_PROXY=http://127.0.0.1:8889\n'
          'export HTTPS_PROXY=http://127.0.0.1:8889\n'
          'export no_proxy=localhost,127.0.0.1,10.0.0.0/8,172.16.0.0/12,192.168.0.0/16\n'
          'export NO_PROXY=localhost,127.0.0.1,10.0.0.0/8,172.16.0.0/12,192.168.0.0/16\n')
b64 = base64.b64encode(script.encode()).decode()

run(f"echo '{PW}' | sudo -S -k sh -c \"echo '{b64}' | base64 -d > /etc/profile.d/trae-proxy.sh && chmod +x /etc/profile.d/trae-proxy.sh\"", 10)
run("cat /etc/profile.d/trae-proxy.sh", 5)

print("\n✅ Debug complete!")
print("If the proxy test passed, the tunnel is working!")
print("Keep this process running. Ctrl+C to stop.")

try:
    while True:
        time.sleep(1)
        if not transport.is_active():
            print("\n⚠️ Transport disconnected!")
            break
except KeyboardInterrupt:
    print("\n🛑 Stopped")
finally:
    proxy_server.shutdown()
    transport.close()
    ssh.close()
