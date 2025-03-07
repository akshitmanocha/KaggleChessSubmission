from subprocess import Popen, PIPE

# TODO: Comment the below line for submission, and uncomment the next line
process = Popen(["./cfish"], stdin=PIPE, stdout=PIPE, text=True)
#process = Popen(["/kaggle_simulations/agent/cfish_O1"], stdin=PIPE, stdout=PIPE, text=True)

# Set the hash size
process.stdin.write("setoption name Hash value 1\n")
process.stdin.flush()

def cfish(obs):
    process.stdin.write(f"position fen {obs.board}\n")
    process.stdin.write("go depth 10 movetime 100\n")
    process.stdin.flush()
    while True:
        line = process.stdout.readline().strip().split()
        if line and line[0] == "bestmove":
            return line[1]
