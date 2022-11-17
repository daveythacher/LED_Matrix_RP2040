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
DEFINE_COLUMNS              128         "Chain length"
DEFINE_SERIAL_CLOCK         15.625      "HUB75 serial clock speed in MHz"
DEFINE_BLANK_TIME           1           "Multiplex blanking time in uS"
DEFINE_RED_GAIN             1.0         "Gain for Red LED (TLC5958 only)"
DEFINE_GREEN_GAIN           1.0         "Gain for Green LED (TLC5958 only)"
DEFINE_BLUE_GAIN            1.0         "Gain for Blue LED (TLC5958 only)"
*/

apps = ""
flags = ""

def build_flavor(Map c) {     
    if (c.enable == "true") {   
        if (apps == "")
            apps = c.name
        else
            apps += sprintf(";%s", c.name)
            
        if (c.algorithm == "TLC5958")
            flags += sprintf(" -D%s_APP=%s -D%s_DEFINE_MULTIPLEX=%s -D%s_DEFINE_MULTIPLEX_NAME=%s -D%s_DEFINE_MAX_RGB_LED_STEPS=%s -D%s_DEFINE_MAX_REFRESH=%s -D%s_DEFINE_COLUMNS=%s -D%s_DEFINE_SERIAL_CLOCK=%s -D%s_DEFINE_BLANK_TIME=%s -D%s_DEFINE_ALGORITHM=%s -D%s_DEFINE_FPS=%s -D%s_DEFINE_RED_GAIN=%s -D%s_DEFINE_GREEN_GAIN=%s -D%s_DEFINE_BLUE_GAIN=%s", c.name, c.app, c.name, c.multiplex, c.name, c.multiplex_name, c.name, c.max_rgb_led_steps, c.name, c.max_refresh, c.name, c.columns, c.name, c.serial_clock, c.name, c.blank_time, c.name, c.algorithm, c.name, c.fps, c.name, c.red_gain, c.name, c.green_gain, c.name, c.blue_gain)
        else
            flags += sprintf(" -D%s_APP=%s -D%s_DEFINE_MULTIPLEX=%s -D%s_DEFINE_MULTIPLEX_NAME=%s -D%s_DEFINE_MAX_RGB_LED_STEPS=%s -D%s_DEFINE_MAX_REFRESH=%s -D%s_DEFINE_COLUMNS=%s -D%s_DEFINE_SERIAL_CLOCK=%s -D%s_DEFINE_BLANK_TIME=%s -D%s_DEFINE_ALGORITHM=%s -D%s_DEFINE_FPS=%s -D%s_DEFINE_RED_GAIN=%s -D%s_DEFINE_GREEN_GAIN=%s -D%s_DEFINE_BLUE_GAIN=%s", c.name, c.app, c.name, c.multiplex, c.name, c.multiplex_name, c.name, c.max_rgb_led_steps, c.name, c.max_refresh, c.name, c.columns, c.name, c.serial_clock, c.name, c.blank_time, c.name, c.algorithm, c.name, c.fps, c.name, "1.0", c.name, "1.0", c.name, "1.0")
    }
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
    print "Please run:\n"
    print "./LED_Matrix/build/build.sh\n"
}

def build() {
    if (SystemUtils.IS_OS_LINUX)
        return build_linux()
    else
        return "Unsupported Build Platform\n"
}

def cli = new CliBuilder(usage: 'build.groovy [-h] -c <cfg.xml>')
cli.with { 
    h longOpt: 'help', 'Show usage information' 
    c longOpt: 'config', args:1, argName: 'cfg.xml', required: true, 'XML build configuration file'
}
def options = cli.parse(args)
if (!options) return
if (options.h) cli.usage()

new XmlSlurper().parse(new File(options.c)).build.each { build_flavor(it.attributes()) }
build()

