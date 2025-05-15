Import("env")

def after_build(source, target, env):
    print("Skipping rebuild, uploading to all targets...")
    ports = [
        "/dev/cu.usbmodem59580084261",  # Board 1
        "/dev/cu.usbmodem59090507901",  # Board 2
        # Add more as needed
    ]
    for port in ports:
        env.Execute(f"~/.platformio/packages/tool-esptoolpy/esptool.py --chip esp32 --port {port} write_flash -z 0x10000 .pio/build/debug/firmware.bin")

env.AddPostAction("buildprog", after_build)