#!/usr/bin/env python3
"""
Simple approach: Use Windows OpenSSH client to create reverse tunnel + Python proxy
"""
import subprocess
import threading
import socket
import select
import time
import os
import signal
import sys
from urllib.parse import urlparse

PW = '123456'
HOST = '172.31.212.123'
PROXY_PORT = 8889
SSH_PORT = 22

# ====== SIMPLE HTTP FORWARD PROXY ======
class SimpleProxy:
    """Minimal HTTP CONNECT proxy server"""
    def __init__(self, port):
        self.port = port
        self.server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.running = False
    
    def _forward(self, client):
        try:
            data = b''
            while b'\r\n\r\n' not in data:
                chunk = client.recv(4096)
                if not chunk: return
                data += chunk
            
            first = data.split(b'\r\n')[0].decode()
            parts = first.split()
            if len(parts) < 2: return
            
            method = parts[0].upper()
            
            if method == 'CONNECT':
                # HTTPS CONNECT
                host, port = parts[1].split(':')
                port = int(port)
                remote = socket.create_connection((host, port), timeout=15)
                client.sendall(b'HTTP/1.1 200 Connection Established\r\n\r\n')
                sockets = [client, remote]
                while True:
                    r, _, _ = select.select(sockets, [], [], 30)
                    if not r: break
                    for s in r:
                        d = s.recv(65536)
                        if not d: return
                        (client if s is remote else remote).sendall(d)
            else:
                # HTTP
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
                    d = remote.recv(65536)
                    if not d: break
                    client.sendall(d)
                remote.close()
        except Exception as e:
            pass
        finally:
            try: client.close()
            except: pass
    
    def start(self):
        self.server.bind(('127.0.0.1', self.port))
        self.server.listen(50)
        self.server.settimeout(1.0)
        self.running = True
        print(f"[Proxy] Listening on 127.0.0.1:{self.port}")
        while self.running:
            try:
                c, a = self.server.accept()
                threading.Thread(target=self._forward, args=(c,), daemon=True).start()
            except socket.timeout: continue
            except: break
    
    def stop(self):
        self.running = False
        try: self.server.close()
        except: pass

# ====== MAIN ======
print("=" * 60)
print("🔧 Step 1: Start HTTP Proxy on Windows")
print("=" * 60)

proxy = SimpleProxy(PROXY_PORT)
t = threading.Thread(target=proxy.start, daemon=True)
t.start()
time.sleep(0.3)
print("✅ Proxy started")

print("\n" + "=" * 60)
print("🔗 Step 2: Create SSH reverse tunnel via Windows OpenSSH")
print("=" * 60)

# Use plink or ssh to create a reverse tunnel
# ssh -R 8889:localhost:8889 chicken@172.31.212.123 -pw 123456 -N
# But we need to do this without blocking and with password auth

# Actually, let's use paramiko's built-in transport for the reverse tunnel
import paramiko

try:
    ssh = paramiko.SSHClient()
    ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy())
    ssh.connect(HOST, username='chicken', password=PW, timeout=15)
    
    def run(cmd, timeout=10):
        stdin, stdout, stderr = ssh.exec_command(cmd, timeout=timeout)
        ec = stdout.channel.recv_exit_status()
        out = stdout.read().decode('utf-8', errors='replace').strip()
        err = stderr.read().decode('utf-8', errors='replace').strip()
        if out: print(out[:1000])
        if err: print(f'[e] {err[:300]}')
        return ec, out, err
    
    # Set proxy env vars for the remote ai-agent
    print("⚙️ Step 3: Configure proxy on remote server...")
    
    # Write proxy config
    run(f"echo '{PW}' | sudo -S -k sh -c 'cat > /etc/profile.d/trae-proxy.sh << \"EOF\"\n" +
        "export http_proxy=http://127.0.0.1:8889\n" +
        "export https_proxy=http://127.0.0.1:8889\n" +
        "export HTTP_PROXY=http://127.0.0.1:8889\n" +
        "export HTTPS_PROXY=http://127.0.0.1:8889\n" +
        "export no_proxy=localhost,127.0.0.1,10.0.0.0/8,172.16.0.0/12,192.168.0.0/16\n" +
        "export NO_PROXY=localhost,127.0.0.1,10.0.0.0/8,172.16.0.0/12,192.168.0.0/16\n" +
        "EOF\nchmod +x /etc/profile.d/trae-proxy.sh'")
    
    run("cat /etc/profile.d/trae-proxy.sh 2>/dev/null")
    
    # Now set up reverse port forwarding using paramiko Transport
    transport = ssh.get_transport()
    
    def tunnel_handler(chan, src_addr, dest_addr):
        """Forward connections from remote:8889 to localhost:8889 (our proxy)"""
        try:
            proxy_sock = socket.create_connection(('127.0.0.1', PROXY_PORT), timeout=10)
            
            # Bidirectional pipe
            while True:
                r, _, _ = select.select([chan, proxy_sock], [], [], 30)
                if not r: break
                for sock in r:
                    try:
                        data = sock.recv(65536)
                        if not data:
                            return
                        if sock is chan:
                            proxy_sock.sendall(data)
                        else:
                            chan.sendall(data)
                    except:
                        return
        except Exception as e:
            print(f"[Tunnel] Error: {e}")
        finally:
            try: chan.close()
            except: pass
            try: proxy_sock.close()
            except: pass
    
    # Request remote port forwarding
    try:
        transport.request_port_forward('127.0.0.1', 8889, tunnel_handler)
        print("✅ Reverse tunnel established: remote:8889 → Windows proxy")
    except paramiko.ssh_exception.ChannelException as e:
        print(f"⚠️ Port forward failed (may already be in use): {e}")
        # Try using a different port
        PROXY_PORT_ALT = 8890
        transport.request_port_forward('127.0.0.1', PROXY_PORT_ALT, tunnel_handler)
        print(f"✅ Using alternative port {PROXY_PORT_ALT}")
        # Update proxy config
        run(f"echo '{PW}' | sudo -S -k sed -i 's/8889/{PROXY_PORT_ALT}/g' /etc/profile.d/trae-proxy.sh")
    
    # Test connectivity through the tunnel
    print("\n🔍 Step 4: Testing proxy...")
    run("curl -sS --connect-timeout 10 --proxy http://127.0.0.1:8889 " +
        "'https://www.baidu.com' -o /dev/null -w 'HTTP %{http_code} (%{time_total}s)' 2>&1", timeout=20)
    
    run("curl -sS --connect-timeout 10 --proxy http://127.0.0.1:8889 " +
        "'https://api.trae.com.cn' -o /dev/null -w 'HTTP %{http_code} (%{time_total}s)' 2>&1", timeout=20)
    
    print("\n✅ Tunnel is running!")
    print("Keep this script running. Press Ctrl+C to stop.")
    print("Then restart Trae CN and try the agent again.")
    
    # Keep alive
    while True:
        time.sleep(1)
        if not transport.is_active():
            print("⚠️ SSH transport disconnected!")
            break
            
except KeyboardInterrupt:
    print("\n\n🛑 Stopping...")
except Exception as e:
    print(f"\n❌ Error: {e}")
    import traceback
    traceback.print_exc()
finally:
    proxy.stop()
    print("✅ Proxy stopped")
    try:
        ssh.close()
    except:
        pass
    print("✅ SSH closed")
