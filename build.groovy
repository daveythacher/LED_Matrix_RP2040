#!/usr/bin/env groovy

/* 
 * File:   build.groovy
 * Author: David Thacher
 * License: GPL 3.0
 */

@Grab(group='org.apache.commons', module='commons-lang3', version='3.11' )
@Grab(group='commons-io', module='commons-io', version='2.11.0')
import org.apache.commons.lang3.SystemUtils
import org.apache.commons.io.FileUtils
import groovy.xml.*
import groovy.cli.commons.*
import LED_Panel_Setting_Calculator.*

DEFINE_APP = ""
DEFINE_SERIAL_CLOCK = ""
DEFINE_ALGORITHM = ""
DEFINE_SERIAL_RGB_TYPE = ""
DEFINE_SERIAL_UART_BAUD = ""
DEFINE_MULTIPLEX = ""
DEFINE_MULTIPLEX_NAME = ""
DEFINE_COLUMNS = ""
DEFINE_IS_RAW = ""
DEFINE_FPS = ""
DEFINE_GCLK = ""
DEFINE_RED_GAIN = ""
DEFINE_GREEN_GAIN = ""
DEFINE_BLUE_GAIN = ""
DEFINE_BLANK_TIME = ""
DEFINE_MAX_REFRESH = ""
DEFINE_MAX_RGB_LED_STEPS = ""
flags = ""

def handle_serial_algorithm(cfg, index) {
    name = cfg.build[index].attributes().name
    c = cfg.build[index].serial[0].attributes()
    
    if (c.algorithm == "uart") {
        s = cfg.build[index].serial[0].uart[0].attributes()
        DEFINE_SERIAL_UART_BAUD = s.baud
        DEFINE_SERIAL_RGB_TYPE = c.RGB_type
        DEFINE_APP = "uart"
    }
    else if (c.algorithm == "test") {
        DEFINE_APP = "test"
        DEFINE_SERIAL_RGB_TYPE = "RGB24"
    }
    else {
        println "Unknown serial algorithm " + c.algorithm
        System.exit(1)
    }
}

def handle_matrix_algorithm(cfg, index) {
    name = cfg.build[index].attributes().name
    c = cfg.build[index].matrix[0].attributes()

    boolean result = false;

    if (c.algorithm == "BCM" || c.algorithm == "PWM") {
        def calc = new GEN_1();
        DEFINE_MULTIPLEX = c.multiplex
        DEFINE_MULTIPLEX_NAME = c.multiplex_name
        DEFINE_MAX_RGB_LED_STEPS = c.max_rgb_led_steps
        DEFINE_MAX_REFRESH = c.max_refresh
        DEFINE_COLUMNS = c.columns
        DEFINE_SERIAL_CLOCK = c.serial_clock
        DEFINE_BLANK_TIME = c.blank_time
        DEFINE_ALGORITHM = c.algorithm

        if (c.algorithm == "BCM") {
            calc.isBCM = true;
            DEFINE_IS_RAW = cfg.build[index].matrix[0].BCM[0].attributes().is_raw
        }
        else {
            DEFINE_IS_RAW = cfg.build[index].matrix[0].PWM[0].attributes().is_raw
        }
        
        result = calc.is_valid((short) Integer.parseInt(c.multiplex), Integer.parseInt(c.columns), Integer.parseInt(c.max_refresh), (short) Math.ceil(Math.log(Integer.parseInt(c.max_rgb_led_steps)) / Math.log(2)));
        
        if (result) {
            result &= Double.parseDouble(c.serial_clock) >= calc.clk_mhz;
        }
    }
    else if (c.algorithm == "TLC5958") {
        def calc = new GEN_3();
        m = cfg.build[index].matrix[0].TLC5958[0].attributes()
        DEFINE_MULTIPLEX = c.multiplex
        DEFINE_MULTIPLEX_NAME = c.multiplex_name
        DEFINE_MAX_RGB_LED_STEPS = c.max_rgb_led_steps
        DEFINE_MAX_REFRESH = c.max_refresh
        DEFINE_COLUMNS = c.columns
        DEFINE_SERIAL_CLOCK = c.serial_clock
        DEFINE_BLANK_TIME = c.blank_time
        DEFINE_ALGORITHM = c.algorithm
        DEFINE_FPS = m.fps
        DEFINE_GCLK = m.gclk
        DEFINE_RED_GAIN = m.red_gain
        DEFINE_GREEN_GAIN = m.green_gain
        DDEFINE_BLUE_GAIN = m.blue_gain
        
        calc.frames_per_second = Double.parseDouble(m.fps);
        calc.s_pwm_bits_per_seg = 8;
        
        result = calc.is_valid((short) Integer.parseInt(c.multiplex), Integer.parseInt(c.columns), Integer.parseInt(c.max_refresh), (short) Math.ceil(Math.log(Integer.parseInt(c.max_rgb_led_steps)) / Math.log(2)));
       
        if (result) {
            result &= Double.parseDouble(c.serial_clock) >= calc.clk_mhz;
            result &= Double.parseDouble(m.gclk) >= calc.gclk_mhz;
        }
    }
    else if (c.algorithm == "TLC5946") {
        def calc = new GEN_2();
        m = cfg.build[index].matrix[0].TLC5946[0].attributes()
        DEFINE_MULTIPLEX = c.multiplex
        DEFINE_MULTIPLEX_NAME = c.multiplex_name
        DEFINE_MAX_RGB_LED_STEPS = c.max_rgb_led_steps
        DEFINE_MAX_REFRESH = c.max_refresh
        DEFINE_COLUMNS = c.columns
        DEFINE_SERIAL_CLOCK = c.serial_clock
        DEFINE_BLANK_TIME = c.blank_time
        DEFINE_ALGORITHM = c.algorithm
        DEFINE_GCLK = m.gclk

        
        calc.is12bitTi = true;
        calc.max_grayscale_bits = 12;
        
        result = calc.is_valid((short) Integer.parseInt(c.multiplex), Integer.parseInt(c.columns), Integer.parseInt(c.max_refresh), (short) Math.ceil(Math.log(Integer.parseInt(c.max_rgb_led_steps)) / Math.log(2)));
       
        if (result) {
            result &= Double.parseDouble(c.serial_clock) >= calc.clk_mhz;
            result &= Double.parseDouble(m.gclk) >= calc.gclk_mhz;
        }
    }
    else if (c.algorithm == "Test") {
        DEFINE_MULTIPLEX = c.multiplex
        DEFINE_MULTIPLEX_NAME = "Decoder"
        DEFINE_MAX_RGB_LED_STEPS = "16"
        DEFINE_MAX_REFRESH = "0"
        DEFINE_COLUMNS = c.columns
        DEFINE_SERIAL_CLOCK = "0"
        DEFINE_BLANK_TIME = "0"
        DEFINE_ALGORITHM = c.algorithm
        result = true;
    }
    else {
        println "Unknown matrix algorithm " + c.algorithm
        System.exit(1)
    }

    if (!result) {
        println "Unsupported matrix algorithm configuration for " + name;
        System.exit(1);
    }
}

def build_linux(run, clean) {
    String script = "./LED_Matrix/build/build.sh"
    File dir = new File("./LED_Matrix/build/")

    if (clean)
        FileUtils.deleteDirectory(dir)
    dir.mkdirs()

    File f = new File(script)
    f.text = ""
    f << "#!/bin/bash\n"
    f << "cd ./LED_Matrix\n"
    f << "DIR=\$(pwd)\n"
    f << "export PICO_SDK_PATH=\$DIR/lib/pico-sdk\n"
    f << "cp \$DIR/lib/pico-sdk/external/pico_sdk_import.cmake .\n"
    f << "cd build\n"
    f << sprintf("cmake .. -DCMAKE_BUILD_TYPE:STRING=Release %s 2>&1\n", flags)
    f << "make -j \$((\$(nproc) * 2)) 2>&1 #VERBOSE=1\n"
    f << "cd ../..\n"
    ("chmod +x " + script).execute().waitFor()

    if (run) {
        def proc = script.execute()
        println proc.text
        System.exit(proc.exitValue())
    }
    else {
        print "Please run:\n"
        println script
    }
}

def build_windows(run, clean) {
    String script = ".\\LED_Matrix\\build\\build.bat"
    File dir = new File("./LED_Matrix/build/")

    if (clean)
        FileUtils.deleteDirectory(dir)
    dir.mkdirs()

    File f = new File(script)
    f.text = ""
    f << "@echo off\n"
    f << "cd ./LED_Matrix\n"
    f << "SET DIR=%cd%\n"
    f << "set CMAKE_GENERATOR=Ninja\n"
    f << "SET PICO_SDK_PATH=%DIR%\\lib\\pico-sdk\n"
    f << "copy %DIR%\\lib\\pico-sdk\\external\\pico_sdk_import.cmake .\n"
    f << "cd build\n"
    f << sprintf("cmake .. -DCMAKE_BUILD_TYPE:STRING=Release %s 2>&1\n", flags)
    f << "ninja -j %NUMBER_OF_PROCESSORS% 2>&1\n"
    f << "cd ..\\..\n"

    if (run) {
        def proc = script.execute()
        println proc.text
        System.exit(proc.exitValue())
    }
    else {
        print "Please run:\n"
        println script
    }
}

def build(run, clean) {
    if (SystemUtils.IS_OS_LINUX)
        build_linux(run, clean)
    else if (SystemUtils.IS_OS_WINDOWS)
        build_windows(run, clean)
    else
        println "Unsupported Build Platform"
}

def build_flavor(cfg, index, options) {
    c = cfg.build[index].attributes()

    if (c.enable == "true") {
        handle_serial_algorithm(cfg, index)
        handle_matrix_algorithm(cfg, index)
        build(options.r, options.s)
        System.exit(0)
    }
}

def cli = new CliBuilder(usage: 'build.groovy [-h] -c <cfg.xml>')
cli.with { 
    h longOpt: 'help', 'Show usage information' 
    c longOpt: 'config', args:1, argName: 'cfg.xml', required: true, 'XML build configuration file'
    r longOpt: 'run', 'Execute the script'
    s longOpt: 'scratch', 'Use clean build directory'
}
def options = cli.parse(args)
if (!options) return
if (options.h) cli.usage()

def cfg = new XmlSlurper().parse(new File(options.c))
cfg.build.eachWithIndex { it, index -> build_flavor(cfg, index, options) }
