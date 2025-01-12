import subprocess
import os
import psutil
import time


class ChessEngine:
    def __init__(self, engine_path):
        self.engine = subprocess.Popen(
            [engine_path],
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True
        )
        self._initialize_engine()

    def _initialize_engine(self):
        self._send_command("uci")
        while True:
            output = self._read_output()
            if output == "uciok":
                break
        self._send_command("setoption name Hash value 1")

    def _send_command(self, command):
        self.engine.stdin.write(command + "\n")
        self.engine.stdin.flush()

    def _read_output(self):
        output = self.engine.stdout.readline().strip()
        return output

    def get_best_move(self, fen, movetime=100):
        self._send_command(f"position fen {fen}")
        self._send_command(f"go movetime {movetime}")
        best_move = None
        while True:
            output = self._read_output()
            if output.startswith("bestmove"):
                best_move = output.split()[1]
                break
        self._send_command("setoption name Clear Hash")
        return best_move

    def stop(self):
        self._send_command("quit")
        self.engine.terminate()
        self.engine.wait()

    def get_memory_usage(self):
        try:
            # Get process object using PID
            pid = self.engine.pid
            proc = psutil.Process(pid)
            
            # Get memory info (RSS) in bytes
            memory_info = proc.memory_info()
            rss_in_mb = memory_info.rss / 1024 / 1024  # Convert bytes to MB
            
            return rss_in_mb
        except (psutil.NoSuchProcess, psutil.AccessDenied, psutil.ZombieProcess):
            print(f"Process with PID {self.engine.pid} not found or access denied.")
            return None

    def monitor_memory_usage(self, interval=1):
        while True:
            memory_usage = self.get_memory_usage()
            if memory_usage is not None:
                print(f"Memory Usage: {memory_usage:.2f} MB")
            time.sleep(interval)  # Sleep for the interval before checking again


ultima = None

def chess_bot(obs):
    global ultima
    fen = obs.board
    if os.path.exists('/kaggle_simulations/agent/Ethereal'):
        engine_path = '/kaggle_simulations/agent/Ethereal'
    else:
        engine_path = '/kaggle/working/Ethereal'
    if ultima is None:
        ultima = ChessEngine(engine_path)
        # Start memory monitoring in a separate thread to avoid blocking the main process
        import threading
        memory_thread = threading.Thread(target=ultima.monitor_memory_usage, args=(1,))
        memory_thread.daemon = True  # This will ensure the thread exits when the main program exits
        memory_thread.start()

    best_move = ultima.get_best_move(fen)
    return best_move
