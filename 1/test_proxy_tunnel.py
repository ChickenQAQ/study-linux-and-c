#!/usr/bin/env python3
"""Test HTTP proxy through SSH reverse tunnel"""
import paramiko, socket, select, threading, time, os, tempfile
from http.server import HTTPServer, BaseHTTPRequestHandler
import urllib.parse

PW = '123456'
HOST = '172.31.212.123'
PROXY_PORT = 8889

# HTTP Proxy Server on Windows
class ProxyHandler(BaseHTTPRequestHandler):
    def do_CONNECT(self):
        host, port = self.path.split(':')
        port = int(port)
        try:
            remote = socket.create_connection((host, port), timeout=15)
            self.send_response(200, 'Connection Established')
            self.end_headers()
            # Pipe both ways
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
        except Exception as e:
            self.send_error(502, f'Bad Gateway')
        finally:
            try: remote.close()
            except: pass
    
    def do_GET(self):
        self._proxy_http()
    def do_POST(self):
        self._proxy_http()
    
    def _proxy_http(self):
        url = self.path
        parsed = urllib.parse.urlparse(url)
        host = parsed.hostname
        port = parsed.port or 80
        path = parsed.path or '/'
        if parsed.query: path += '?' + parsed.query
        try:
            remote = socket.create_connection((host, port), timeout=15)
            req = f'{self.command} {path} HTTP/1.0\r\nHost: {host}\r\nConnection: close\r\n\r\n'
            remote.sendall(req.encode())
            resp = b''
            while True:
                d = remote.recv(65536)
                if not d: break
                resp += d
                try: self.wfile.write(d)
                except: break
            remote.close()
        except Exception as e:
            self.send_error(502, str(e))
    
    def log_message(self, format, *args):
        pass  # Suppress logs

print("[1] Starting HTTP proxy...")
proxy = HTTPServer(('127.0.0.1', PROXY_PORT), ProxyHandler)
threading.Thread(target=proxy.serve_forever, daemon=True).start()
time.sleep(0.2)
print("  ✅ HTTP proxy on 127.0.0.1:8889")

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

print("[3] Setting up bidirectional tunnel...")
def tunnel_handler(chan, src_addr, dest_addr):
    try:
        remote = socket.create_connection(('127.0.0.1', PROXY_PORT), timeout=10)
        chan.setblocking(0)
        remote.setblocking(0)
        while True:
            r, _, _ = select.select([chan, remote], [], [], 30)
            if not r: break
            for s in r:
                try:
                    d = s.recv(65536)
                    if not d: return
                    (chan if s is remote else remote).sendall(d)
                except: return
    except: pass
    finally:
        try: chan.close()
        except: pass
        try: remote.close()
        except: pass

transport.request_port_forward('127.0.0.1', PROXY_PORT, tunnel_handler)
print("  ✅ Tunnel active")

print("\n[4] Testing...")
def run(cmd, timeout=25):
    stdin, stdout, stderr = ssh.exec_command(cmd, timeout=timeout)
    ec = stdout.channel.recv_exit_status()
    out = stdout.read().decode('utf-8', errors='replace').strip()
    err = stderr.read().decode('utf-8', errors='replace').strip()
    if out: print(f"  {out[:400]}")
    if err: print(f"  ERR: {err[:200]}")
    return ec, out, err

# Test 1: TCP port open
print("\n  --- TCP test ---")
run("timeout 3 bash -c 'echo > /dev/tcp/127.0.0.1/8889' 2>&1 && echo 'PORT_OK' || echo 'PORT_FAIL'", 5)

# Test 2: HTTP via proxy (baidu.com)
print("  --- HTTP test (baidu) ---")
run("curl -sS --connect-timeout 10 --max-time 15 --proxy http://127.0.0.1:8889 "
    "'http://www.baidu.com' -o /dev/null -w 'HTTP %{http_code} (%{time_total}s)' 2>&1", 20)

# Test 3: HTTPS via proxy (baidu)
print("  --- HTTPS test (baidu) ---")
run("curl -sS --connect-timeout 10 --max-time 15 --proxy http://127.0.0.1:8889 "
    "'https://www.baidu.com' -o /dev/null -w 'HTTP %{http_code} (%{time_total}s)' 2>&1", 20)

# Test 4: Trae API
print("  --- Trae API test ---")
run("curl -sS --connect-timeout 10 --max-time 15 --proxy http://127.0.0.1:8889 "
    "'https://api.trae.com.cn' -o /dev/null -w 'HTTP %{http_code} (%{time_total}s)' 2>&1", 20)

# Test 5: Trae API v2
print("  --- Trae API v2 test ---")
run("curl -sS --connect-timeout 10 --max-time 15 --proxy http://127.0.0.1:8889 "
    "'https://trae-api-cn.mchost.guru' -o /dev/null -w 'HTTP %{http_code} (%{time_total}s)' 2>&1", 20)

print("\n✅ Tests complete. Keep this running for the tunnel.")
print("Restart Trae CN and try again. Ctrl+C to stop.")

try:
    while True:
        time.sleep(2)
        if not transport.is_active():
            print("⚠️ Tunnel lost!")
            break
except KeyboardInterrupt:
    print("\n🛑 Stopped")
finally:
    proxy.shutdown()
    transport.close()
    ssh.close()
