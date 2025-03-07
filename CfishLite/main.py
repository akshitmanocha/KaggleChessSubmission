from subprocess import Popen, PIPE
import subprocess

# TODO: Comment the below line for submission, and uncomment the next line
#process = Popen(["./cfish_nocounter"], stdin=PIPE, stdout=PIPE, text=True)

# Path to the compressed Ethereal
compressed_path = "/kaggle_simulations/agent/cfish.7z"
decompressed_path = "/kaggle_simulations/agent/cfish"

subprocess.run(["7z", "x", compressed_path, "-o/kaggle_simulations/agent/"], check=True)

process = Popen(["/kaggle_simulations/agent/cfish"], stdin=PIPE, stdout=PIPE, text=True)

def cfish(obs):
    process.stdin.write(f"position fen {obs.board}\n")
    process.stdin.write("go movetime 200\n")
    process.stdin.flush()
    while True:
        line = process.stdout.readline().strip().split()
        if line and line[0] == "bestmove":
            return line[1]
