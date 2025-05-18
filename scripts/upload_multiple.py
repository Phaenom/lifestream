Import("env")
import os
import subprocess
import serial.tools.list_ports

def after_build(source, target, env):
    print("\n🔍 Detecting connected ESP32 boards...")

    # Detect ports matching known ESP32 USB bridges
    ports = [
        p.device for p in serial.tools.list_ports.comports()
        if "USB" in p.description or "CP210" in p.description or "CH340" in p.description
    ]

    if not ports:
        print("❌ No ESP32 devices detected.")
        return

    print(f"🧩 Found {len(ports)} device(s): {', '.join(ports)}")

    # Path to PlatformIO's internal CLI
    platformio_path = os.path.expanduser("~/.platformio/penv/Scripts/platformio.exe")
    if not os.path.exists(platformio_path):
        print(f"❌ Cannot find PlatformIO CLI at: {platformio_path}")
        return

    summary = []

    for port in ports:
        print(f"\n🚀 Uploading to {port}...")
        result = subprocess.run(
            [platformio_path, "run", "-t", "upload", "--upload-port", port],
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True
        )

        if result.returncode == 0:
            summary.append((port, "✅ Success"))
            print(result.stdout)
        else:
            summary.append((port, "❌ Failed"))
            print(result.stderr)

    print("\n📋 Upload Summary:")
    for port, status in summary:
        print(f" - {port}: {status}")

# Register this script to run after build completes
env.AddPostAction("buildprog", after_build)
