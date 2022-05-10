#!/usr/bin/env groovy

/* 
 * File:   build.groovy
 * Author: David Thacher
 * License: GPL 3.0
 */

@Grab(group='org.apache.commons', module='commons-lang3', version='3.11' )
import org.apache.commons.lang3.SystemUtils

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
            blank_time: "1"
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
            blank_time: "1"
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
            blank_time: "1"
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
            blank_time: "1"
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
            blank_time: "1"
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
            blank_time: "1"
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
            blank_time: "1"                 // Not used
        ]
    ]

stop = false

def build_flavor_linux(Map c) {
    if (!c.enable)
        return "Skipping"
    new File("./LED_Matrix/build/" + c.name).mkdirs()
    File f = new File("./LED_Matrix/build/" + c.name + "/build.sh")
    f.text = ""
    f << "#!/bin/bash\n"
    f << "cd ./LED_Matrix/build/" + c.name + "\n"
    f << "DIR=\$(pwd)\n"
    f << "export PICO_SDK_PATH=\$DIR/../../lib/pico-sdk\n"
    f << "cp \$DIR/../../lib/pico-sdk/external/pico_sdk_import.cmake ../..\n"
    f << "mkdir build\n"
    f << "cd build\n"
    f << sprintf("cmake ../../.. -DAPP=%s -DDEFINE_MULTIPLEX=%s -DDEFINE_MULTIPLEX_NUM=%s -DDEFINE_MAX_RGB_LED_STEPS=%s -DDEFINE_MAX_REFRESH=%s -DDEFINE_FPS=%s -DDEFINE_COLUMNS=%s -DDEFINE_SERIAL_CLOCK=%s -DDEFINE_BLANK_TIME=%s 2>&1\n", c.app, c.multiplex, c.multiplex_num, c.max_rgb_led_steps, c.max_refresh, c.fps, c.columns, c.serial_clock, c.blank_time)
    f << "make -j \$((\$(nproc) * 2)) 2>&1 #VERBOSE=1\n"
    f << "if [ \$? -eq 0 ]; then\n"
    f << "\techo \"Binary output:\"\n"
    f << "\tls -la \$DIR/build/src/led_*.*\n"
    f << "else\n"
    f << "\texit 1\n"
    f << "fi\n"
    sprintf("chmod +x ./LED_Matrix/build/%s/build.sh", c.name).execute().waitFor()
    def proc = sprintf("./LED_Matrix/build/%s/build.sh", c.name).execute()
    proc.waitFor()
    if (proc.exitValue() == 1) {
        stop = true
    }
    return proc.text
}

def build_flavor(Map c) {
    if (SystemUtils.IS_OS_LINUX)
        return build_flavor_linux(c)
    else {
        stop = true
        return "Unsupported Build Platform\n"
    }
}

cfgs.each {
    print "\n\nBuilding " + it.name + ":\n"
    print build_flavor(it)
    if (stop)
        System.exit(1)
}

