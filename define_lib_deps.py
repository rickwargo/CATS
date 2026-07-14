Import("env")
import os
import subprocess
import sys

def has_define(flags, name):
    """Return True if -DNAME is active (last -D/-U wins).

    Each BUILDFLAGS entry may contain multiple space-separated flags
    (e.g. '-DNETWORKING -DNETWORK_TIME -DOTA_UPDATE' as one string),
    so we split every entry before checking.
    """
    result = False
    for entry in flags:
        for flag in str(entry).split():
            if flag == f"-D{name}" or flag.startswith(f"-D{name}="):
                result = True
            elif flag == f"-U{name}":
                result = False
    return result

build_flags = env.GetProjectOption("build_flags", [])
lib_deps = []

if has_define(build_flags, "UI_DISPLAY"):
    lib_deps += ["Bodmer/TFT_eSPI"]
    env.Append(CCFLAGS=["-include", "include/TFT_eSPI_configuration.h"])

if has_define(build_flags, "ENVIRONMENT_SENSOR"):
    lib_deps += [
        "boschsensortec/BME68x Sensor library",
        "boschsensortec/bsec2",
    ]

if has_define(build_flags, "IMU_SENSOR"):
    lib_deps += ["electroniccats/MPU6050"]

if has_define(build_flags, "LIGHT_LEVEL_SENSOR"):
    lib_deps += [
        "sparkfun/SparkFun Toolkit",
        "sparkfun/SparkFun VEML7700 Arduino Library",
    ]

if has_define(build_flags, "BUZZER_SOUND"):
    lib_deps += ["end2endzone/NonBlockingRTTTL"]

if has_define(build_flags, "ENCODER"):
    lib_deps += ["igorantolic/Ai Esp32 Rotary Encoder"]

if has_define(build_flags, "BUTTON"):
    lib_deps += ["lennarthennigs/Button2"]

if has_define(build_flags, "FAST_LED"):
    lib_deps += ["fastled/FastLED"]

if has_define(build_flags, "NETWORK_TIME"):
    lib_deps += [
        "paulstoffregen/Time",
        "jchristensen/Timezone",
    ]

if has_define(build_flags, "NETWORKING"):
    lib_deps += [
        "esp32async/AsyncTCP",
        "esp32async/ESPAsyncWebServer @ 3.11.1",
    ]

if has_define(build_flags, "OTA_UPDATE"):
    lib_deps += ["ayushsharma82/ElegantOTA"]

if has_define(build_flags, "STORAGE"):
    lib_deps += ["greiman/SdFat"]

if lib_deps:
    # PlatformIO's CLI-level package install reads lib_deps from platformio.ini,
    # which is now empty. Drive installation explicitly so the LDF finds them.
    cmd = [sys.executable, "-m", "platformio", "pkg", "install",
           "--environment", env["PIOENV"], "--no-save"]
    for lib in lib_deps:
        cmd += ["--library", lib]

    result = subprocess.run(cmd, cwd=env.subst("$PROJECT_DIR"))
    if result.returncode != 0:
        print("ERROR: Failed to install dynamic library dependencies")
        Exit(1)

env.Append(LIBDEPS=lib_deps)
