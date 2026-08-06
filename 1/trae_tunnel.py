#!/usr/bin/env python3
"""
Trae CN ai-agent Proxy Tunnel - Final Working Version

Architecture:
- Windows (has internet) runs HTTP proxy on port 8889
- SSH reverse tunnel makes this proxy available on remote:8889
- Remote ai-agent uses http://127.0.0.1:8889 as proxy for API calls
"""
import paramiko, socket, select, threading, time, os, tempfile, base64, sys
from urllib.parse import urlparse

PW = '123456'
HOST = '172.31.212.123'
PROXY_PORT = 8889

# ====== HTTP Proxy Server (Windows side) ======
class HttpProxy:
    def __init__(self, port):
        self.port = port
        self.server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.running = False
    
    def _pipe(self, src, dst):
        try:
            while True:
                r, _, _ = select.select([src, dst], [], [], 30)
                if not r: break
                for s in r:
                    data = s.recv(65536)
                    if not data: return
                    (src if s is dst else dst).sendall(data)
        except: pass
        finally:
            for s in (src, dst):
                try: s.close()
                except: pass

    def _handle(self, client):
        try:
            data = b''
            while b'\r\n\r\n' not in data:
                chunk = client.recv(4096)
                if not chunk: return
                data += chunk
            
            first = data.split(b'\r\n')[0].decode()
            parts = first.split()
            if len(parts) < 2: return
            
            if parts[0].upper() == 'CONNECT':
                host, port = parts[1].split(':')
                port = int(port)
                remote = socket.create_connection((host, port), timeout=15)
                client.sendall(b'HTTP/1.1 200 Connection Established\r\n\r\n')
                self._pipe(client, remote)
            else:
                url = parts[1]
                parsed = urlparse(url)
                host = parsed.hostname
                port = parsed.port or 80
                path = parsed.path or '/'
                if parsed.query: path += '?' + parsed.query
                remote = socket.create_connection((host, port), timeout=15)
                new_req = data.replace(url.encode(), path.encode(), 1)
                remote.sendall(new_req)
                while True:
                    chunk = remote.recv(65536)
                    if not chunk: break
                    client.sendall(chunk)
                remote.close()
        except Exception as e:
            pass
        finally:
            try: client.close()
            except: pass

    def start(self):
        self.server.bind(('127.0.0.1', self.port))
        self.server.listen(100)
        self.server.settimeout(1.0)
        self.running = True
        print(f"[Proxy] Listening on 127.0.0.1:{self.port}")
        while self.running:
            try:
                c, a = self.server.accept()
                threading.Thread(target=self._handle, args=(c,), daemon=True).start()
            except socket.timeout: continue
            except: break

    def stop(self):
        self.running = False
        try: self.server.close()
        except: pass

# ====== Tunnel Handler ======
def make_handler(proxy_port):
    def handler(chan, src_addr, dest_addr):
        try:
            ps = socket.create_connection(('127.0.0.1', proxy_port), timeout=10)
            while True:
                r, _, _ = select.select([chan, ps], [], [], 30)
                if not r: break
                for s in r:
                    d = s.recv(65536)
                    if not d: return
                    (chan if s is ps else ps).sendall(d)
        except: pass
        finally:
            try: chan.close()
            except: pass
            try: ps.close()
            except: pass
    return handler

# ====== Main ======
def main():
    print("=" * 60)
    print("🔧 Trae CN ai-agent Proxy Tunnel")
    print("=" * 60)
    
    # Step 1: Start HTTP proxy
    print("\n[1/4] Starting HTTP proxy on Windows...")
    proxy = HttpProxy(PROXY_PORT)
    t = threading.Thread(target=proxy.start, daemon=True)
    t.start()
    time.sleep(0.3)
    
    # Step 2: Setup SSH
    print("[2/4] Setting up SSH transport...")
    ssh = paramiko.SSHClient()
    ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy())
    ssh.connect(HOST, username='chicken', password=PW, timeout=15)
    
    # Get private key
    stdin, stdout, stderr = ssh.exec_command("cat ~/.ssh/id_ed25519", timeout=5)
    key_data = stdout.read().decode()
    
    key_file = os.path.join(tempfile.gettempdir(), 'trae_tunnel_key')
    with open(key_file, 'w') as f: f.write(key_data)
    os.chmod(key_file, 0o600)
    
    from paramiko import Ed25519Key
    pkey = Ed25519Key(filename=key_file)
    
    # Connect transport
    transport = paramiko.Transport((HOST, 22))
    transport.connect(username='chicken', pkey=pkey)
    print("  ✅ SSH transport ready")
    
    # Step 3: Create reverse tunnel
    print("[3/4] Creating reverse SSH tunnel...")
    handler = make_handler(PROXY_PORT)
    transport.request_port_forward('127.0.0.1', PROXY_PORT, handler)
    print(f"  ✅ remote:{PROXY_PORT} → Windows proxy")
    
    # Step 4: Configure proxy env
    print("[4/4] Configuring remote proxy...")
    def run_cmd(cmd, timeout=15):
        stdin, stdout, stderr = ssh.exec_command(cmd, timeout=timeout)
        ec = stdout.channel.recv_exit_status()
        out = stdout.read().decode('utf-8', errors='replace').strip()
        err = stderr.read().decode('utf-8', errors='replace').strip()
        if out and 'error' not in out.lower(): print(f"  {out[:500]}")
        if err: print(f"  [e] {err[:200]}")
        return ec
    
    # Write proxy config via base64
    proxy_script = (
        'export http_proxy=http://127.0.0.1:8889\n'
        'export https_proxy=http://127.0.0.1:8889\n'
        'export HTTP_PROXY=http://127.0.0.1:8889\n'
        'export HTTPS_PROXY=http://127.0.0.1:8889\n'
        'export no_proxy=localhost,127.0.0.1,10.0.0.0/8,172.16.0.0/12,192.168.0.0/16\n'
        'export NO_PROXY=localhost,127.0.0.1,10.0.0.0/8,172.16.0.0/12,192.168.0.0/16\n'
    )
    b64 = base64.b64encode(proxy_script.encode()).decode()
    
    run_cmd(f"echo '{PW}' | sudo -S -k sh -c \"echo '{b64}' | base64 -d > /etc/profile.d/trae-proxy.sh && chmod +x /etc/profile.d/trae-proxy.sh\"")
    run_cmd("cat /etc/profile.d/trae-proxy.sh")
    print("  ✅ Proxy config written to /etc/profile.d/trae-proxy.sh")
    
    # Test connectivity
    print("\n🔍 Testing tunnel...")
    run_cmd("curl -sS --connect-timeout 10 --proxy http://127.0.0.1:8889 "
            "'https://www.baidu.com' -o /dev/null -w 'HTTP %{http_code} (%{time_total}s)' 2>&1", timeout=20)
    run_cmd("curl -sS --connect-timeout 10 --proxy http://127.0.0.1:8889 "
            "'https://api.trae.com.cn' -o /dev/null -w 'HTTP %{http_code} (%{time_total}s)' 2>&1", timeout=20)
    run_cmd("curl -sS --connect-timeout 10 --proxy http://127.0.0.1:8889 "
            "'https://trae-api-cn.mchost.guru' -o /dev/null -w 'HTTP %{http_code} (%{time_total}s)' 2>&1", timeout=20)
    
    print("\n" + "=" * 60)
    print("✅ Tunnel ACTIVE!")
    print("=" * 60)
    print("""
Keep this terminal open. Then:
1. RESTART Trae CN (close & reopen)
2. Reconnect to remote server (172.31.212.123)
3. Try traeagent code analysis → should work now!

Proxy config is also saved persistently at:
  /etc/profile.d/trae-proxy.sh

Press Ctrl+C to stop.
""")
    
    while True:
        time.sleep(2)
        if not transport.is_active():
            print("\n⚠️ SSH disconnected!")
            break

if __name__ == '__main__':
    try:
        main()
    except KeyboardInterrupt:
        print("\n\n🛑 Stopped")
    except Exception as e:
        print(f"\n❌ {type(e).__name__}: {e}")
        import traceback
        traceback.print_exc()
