
import subprocess
import os


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
    best_move = ultima.get_best_move(fen)

    return best_move
