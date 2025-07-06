#====== config ======
AETHER_ROOT="C:\dev\Aether"
#====================
import os 
import subprocess
GAME_ROOT=os.path.abspath("../")
def link(target, source):
    if os.path.exists(target):
        print(f"Removing existing link: {target}")
        os.remove(target)
    print(f"Creating link: {target} -> {source}")
    subprocess.run(["mklink", "/D", target, source], shell=True)

if __name__ == "__main__":
    link(f"{GAME_ROOT}\\Vendor", f"{AETHER_ROOT}\\Vendor")
    link(f"{GAME_ROOT}\\CMake", f"{AETHER_ROOT}\\CMake")
    print("Links created successfully.")
