# print("-- NOTE: This session manager is supposed to be started immediately after i3wm.")

import argparse,db,util
import os,shutil

p = argparse.ArgumentParser(
    prog="i3de-session",
    description="Initiates an i3de session."
)

p.add_argument("-si","-i3","--start-i3",
               help="Whether or not i3wm should be automatically started.",
               action="store_true",
               default=False)

p.add_argument("--bar",
               help="What bar should be launched.",
               default="i3de-sensible bar")
p.add_argument("--compositor",
               help="What compositor should be launched.",
               default="i3de-sensible compositor")
p.add_argument("--notify",
               help="What notification agent should be launched.",
               default="i3de-sensible notify")
p.add_argument("--polkit",
               help="What polkit agent should be launched.",
               default="i3de-sensible notify")

args = p.parse_args()


CONFIG_DIR = os.path.expanduser("~/.config/i3de/")
os.makedirs(CONFIG_DIR,exist_ok=True)

if not os.path.exists(os.path.join(CONFIG_DIR,"i3.conf")):
    DEFAULT_DIR = "/usr/share/i3de/defaults"
    for i in os.listdir(DEFAULT_DIR):
        shutil.copyfile(
            os.path.join(DEFAULT_DIR,i),
            os.path.join(CONFIG_DIR,i),
            follow_symlinks=True)

if args.start_i3:
    db.INFO("Launching i3wm...")
    util.silentOpen(["i3","-c","$HOME/.config/i3de/i3.conf"])
else:
    db.WARN("-si was not called, so i3wm was not started.")

# util.silentOpen([args.bar])
# util.silentOpen([args.compositor])
# util.silentOpen([args.notify])
# util.silentOpen([args.polkit])