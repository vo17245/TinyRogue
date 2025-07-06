import os
import shutil
dirs=[
"Assets/Atlas",
"Assets/Audio",
"Assets/Fonts",
"Assets/Images",
]
def get_time_str():
    import time
    return time.strftime("%Y-%m-%d_%H-%M-%S", time.localtime())
def pack():
    # ensure dir
    if os.path.exists("tmp/Assets"):
        shutil.rmtree("tmp/Assets")
    if not os.path.exists("tmp"):
        os.mkdir("tmp")
    os.mkdir("tmp/Assets")
    # copy dirs
    for d in dirs:
        if os.path.exists(d):
            name= d.split("/")[-1]
            dst=f"tmp/Assets/{name}"
            if os.path.exists(dst):
                shutil.rmtree(dst)
            shutil.copytree(d, dst)
            print(f"Copied {d} to tmp/Assets")
    # pack zip
    zip_path=f"tmp/Assets_{get_time_str()}"
    shutil.make_archive(zip_path, 'zip', "tmp","Assets")
    print(f"Packed assets to {zip_path}.zip")
    


os.chdir("..")
pack()
os.chdir("Scripts")
