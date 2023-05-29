# features/step_definitions/steps.rb

Given("binary named {word}") do |name|
    @app_name = name
    @serial_algorithm = String.new("uart")
    @matrix_algorithm = String.new("BCM")
    @multiplexer = String.new("Decoder")
    @multiplex_scan = 8
    @serial_clock_mhz = 15.0
    @frame_rate_hz = 30
    @contrast_ratio = 500
    @refresh_rate_hz = 100
    @columns = 16
    @blank_time_us = 1
    @baud = 4000000
    @gclk = 17.5
    @rgb_type = String.new("uint8_t")
    @red_gain = 1.0
    @green_gain = 1.0
    @blue_gain = 1.0
end

When("using serial algorithm {word}") do |algorithm|
    @serial_algorithm = String.new(algorithm)
end

When("using multiplexer algorithm {word}") do |algorithm|
    @multiplexer = String.new(algorithm)
end

When("using matrix algorithm {word}") do |algorithm|
    @matrix_algorithm = String.new(algorithm)
end

And("multiplex ratio of 1 : {int}") do |ratio|
    @multiplex_scan = ratio
end

And("serial clock {float} MHz") do |clock|
    @serial_clock_mhz = clock
end

And("frame rate of {int}") do |rate|
    @frame_rate_hz = rate
end

And("LED contrast ratio of 1 : {int}") do |ratio|
    @contrast_ratio = ratio
end

And("panel refresh rate of {int} Hz") do |rate|
    @refresh_rate_hz = rate
end

And("panel with {int} columns") do |num|
    @columns = num
end

And("anti-ghosting blank time of {int} uS") do |num|
    @blank_time_us = num
end

And("red gain of {float}") do |num|
    @red_gain = num
end

And("green gain of {float}") do |num|
    @green_gain = num
end

And("blue gain of {float}") do |num|
    @blue_gain = num
end

And("grayscale clock of {float} MHz") do |num|
    @gclk = num
end

And("UART baud of {int}") do |num|
    @baud = num
end

And("serial protocol uses RGB-{int}") do |num|
    if num == 24
        @rgb_type = String.new("uint8_t")
    elsif num == 48
        @rgb_type = String.new("uint8_t")
    else
        assert(false)
    end
end

Then("build it") do
    file = File.new(@app_name + ".xml", "w")
    if file
        file.syswrite("<cfg>\n")
        file.syswrite("\t<build name=\"" + @app_name + "\" enable=\"true\">\n")
        file.syswrite("\t\t<serial algorithm=\"" + @serial_algorithm + "\">\n")
        if (@serial_algorithm == "uart")
            file.syswrite("\t\t\t<uart baud=\"" + @baud.to_s + "\" RGB_type=\"" + @rgb_type + "\"/>\n")
        end
        file.syswrite("\t\t</serial>\n")
        file.syswrite("\t\t<matrix algorithm=\"" + @matrix_algorithm + "\" multiplex=\"" + @multiplex_scan.to_s + "\" multiplex_name=\"" + @multiplexer + "\" max_rgb_led_steps=\"" + @contrast_ratio.to_s + "\" ")
        file.syswrite("max_refresh=\"" + @refresh_rate_hz.to_s + "\" columns=\"" + @columns.to_s + "\" serial_clock=\"" + @serial_clock_mhz.to_s + "\" blank_time=\"" + @blank_time_us.to_s + "\">\n")
        if (@matrix_algorithm == "TLC5958")
            file.syswrite("\t\t\t<TLC5958 fps=\"" + @frame_rate_hz.to_s + "\" gclk=\"" + @gclk.to_s + "\" red_gain=\"" + @red_gain.to_s + "\" green_gain=\"" + @green_gain.to_s + "\" blue_gain=\"" + @blue_gain.to_s + "\"/>\n")
        elsif (@matrix_algorithm == "TLC5946")
            file.syswrite("\t\t\t<TLC5946 gclk=\"" + @gclk.to_s + "\"/>\n")
        end
        file.syswrite("\t\t</matrix>\n")
        file.syswrite("\t</build>\n")
        file.syswrite("</cfg>\n")
        file.close
    end

    cmd = "groovy build.groovy -c " + @app_name + ".xml -r 2>&1"
    puts `#{cmd}`
    result = $?.exitstatus
    expect(result.to_s).to eq("0")
end