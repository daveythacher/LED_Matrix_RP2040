#!/usr/bin/env groovy

/* 
 * File:   build.groovy
 * Author: David Thacher
 * License: GPL 3.0
 */

@Grab(group='org.apache.commons', module='commons-lang3', version='3.11' )
import org.apache.commons.lang3.SystemUtils

/*
DEFINE_MULTIPLEX            32          "Multiplex Scan of panel"
DEFINE_MAX_RGB_LED_STEPS    8000        "Min LED const forward current in nA divided by min light current in nA"
DEFINE_MAX_REFRESH          3840        "Max refresh of panel"
DEFINE_FPS                  30          "Frames per second"
DEFINE_COLUMNS              128         "Chain length"
DEFINE_MULTIPLEX_NUM        0           "Address pin/Multiplexing type enum"
DEFINE_POWER_DIVISOR        1           "Divisor for lowering average power. Requires bulk energy storage to smooth peak current"
DEFINE_USE_CIE1931          1           "Use CIE1931 color temperature mapping table"
DEFINE_SERIAL_CLOCK         15.625      "HUB75 serial clock speed in MHz"
DEFINE_BLANK_TIME           1           "Multiplex blanking time in uS"
*/

def cfgs = [
        [
            name: "P6-BCM",
            enable: true,
            app: "app1",
            multiplex: "8",
            multiplex_num: "0",
            max_rgb_led_steps: "500",
            max_refresh: "200",
            fps: "30",
            columns: "128",
            serial_clock: "15.625",
            blank_time: "1",
            power_divisor: "1",
            use_cie1931: "1"
        ],
        [
            name: "P4-BCM",
            enable: true,
            app: "app1",
            multiplex: "16",
            multiplex_num: "0",
            max_rgb_led_steps: "500",
            max_refresh: "200",
            fps: "30",
            columns: "128",
            serial_clock: "15.625",
            blank_time: "1",
            power_divisor: "1",
            use_cie1931: "1"
        ],
        [
            name: "P4-SPWM",
            enable: true,
            app: "app2",
            multiplex: "16",
            multiplex_num: "0",
            max_rgb_led_steps: "500",
            max_refresh: "3840",
            fps: "30",
            columns: "128",
            serial_clock: "15.625",
            blank_time: "1",
            power_divisor: "1",
            use_cie1931: "1"
        ],
        [
            name: "P4-MBI5153",
            enable: true,
            app: "app3",
            multiplex: "16",
            multiplex_num: "0",
            max_rgb_led_steps: "4000",
            max_refresh: "3840",
            fps: "30",
            columns: "128",
            serial_clock: "15.625",
            blank_time: "1",
            power_divisor: "1",
            use_cie1931: "1"
        ],
        [
            name: "POE",
            enable: true,
            app: "poe",
            multiplex: "16",
            multiplex_num: "0",
            max_rgb_led_steps: "2000",
            max_refresh: "3840",
            fps: "30",
            columns: "128",
            serial_clock: "15.625",
            blank_time: "1",
            power_divisor: "1",
            use_cie1931: "1"
        ],
        [
            name: "Cube",
            enable: true,
            app: "app1",
            multiplex: "4",
            multiplex_num: "0",
            max_rgb_led_steps: "2000",
            max_refresh: "250",
            fps: "30",
            columns: "16",
            serial_clock: "15.625",
            blank_time: "1",
            power_divisor: "1",
            use_cie1931: "1"
        ],
        [
            name: "HT1632C",
            enable: true,
            app: "HT1632C",
            multiplex: "16",                // Not used
            multiplex_num: "0",             // Not used
            max_rgb_led_steps: "2000",      // Not used
            max_refresh: "3840",            // Not used
            fps: "30",                      // Not used
            columns: "128",
            serial_clock: "15.625",
            blank_time: "1",                // Not used
            power_divisor: "1",
            use_cie1931: "1"                // Not used
        ]
    ]

stop = false
apps = ""
flags = ""

def build_flavor(Map c) {
    if (!c.enable)
        return "Skipping"
        
    if (apps == "")
        apps = c.name
    else
        apps += sprintf(";%s", c.name)
         
    flags += sprintf(" -D%s_APP=%s -D%s_DEFINE_MULTIPLEX=%s -D%s_DEFINE_MULTIPLEX_NUM=%s -D%s_DEFINE_MAX_RGB_LED_STEPS=%s -D%s_DEFINE_MAX_REFRESH=%s -D%s_DEFINE_FPS=%s -D%s_DEFINE_COLUMNS=%s -D%s_DEFINE_SERIAL_CLOCK=%s -D%s_DEFINE_BLANK_TIME=%s -D%s_DEFINE_POWER_DIVISOR=%s -D%s_DEFINE_USE_CIE1931=%s", c.name, c.app, c.name, c.multiplex, c.name, c.multiplex_num, c.name, c.max_rgb_led_steps, c.name, c.max_refresh, c.name, c.fps, c.name, c.columns, c.name, c.serial_clock, c.name, c.blank_time, c.name, c.power_divisor, c.name, c.use_cie1931)
}

def build_linux() {
    new File("./LED_Matrix/build/").mkdirs()
    File f = new File("./LED_Matrix/build/build.sh")
    f.text = ""
    f << "#!/bin/bash\n"
    f << "cd ./LED_Matrix\n"
    f << "DIR=\$(pwd)\n"
    f << "export PICO_SDK_PATH=\$DIR/lib/pico-sdk\n"
    f << "cp \$DIR/lib/pico-sdk/external/pico_sdk_import.cmake .\n"
    f << "cd build\n"
    f << sprintf("cmake .. -DAPPS=\"%s\" %s  2>&1\n", apps, flags)
    f << "make -j \$((\$(nproc) * 2)) 2>&1 #VERBOSE=1\n"
    f << "if [ \$? -eq 0 ]; then\n"
    f << "\techo \"Binary output:\"\n"
    f << "\tls -la \$DIR/build/src/led_*.*\n"
    f << "else\n"
    f << "\texit 1\n"
    f << "fi\n"
    "chmod +x ./LED_Matrix/build/build.sh".execute().waitFor()
    def proc = "./LED_Matrix/build/build.sh".execute()
    proc.waitFor()
    if (proc.exitValue() == 1) {
        stop = true
    }
    return proc.text
}

def build() {
    if (SystemUtils.IS_OS_LINUX)
        return build_linux()
    else {
        stop = true
        return "Unsupported Build Platform\n"
    }
}

cfgs.each {
    print "\nBuilding " + it.name + ":\n"
    build_flavor(it)
}

print build()
if (stop)
    System.exit(1)

