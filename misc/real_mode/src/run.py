import os
import sys
import time
import tempfile
import shutil
import subprocess

sys.stdout.write("Give me data (512 bytes): ")
sys.stdout.flush()
data = sys.stdin.read(512)

if len(data) != 512:
    return

tmp_dir = tempfile.mkdtemp(dir="/tmp")
with open(tmp_dir + "/loader.bin", "wb+") as f:
    f.write(data)

subprocess.Popen(["qemu-system-i386", "-bios",
            "./bios.bin", "-m", "32", "-smp", "1",
            "-pidfile", "{}/vm.pid".format(tmp_dir), "-nographic",
	        "-net", "none", "-drive",
            "file={}/loader.bin,format=raw".format(tmp_dir)])

time.sleep(15)
os.system("kill -9 $(cat {}/vm.pid)".format(tmp_dir))
shutil.rmtree(tmp_dir)
