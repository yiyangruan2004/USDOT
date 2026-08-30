module top (
    input wire pio1,
    input wire pio2,
    input wire pio3,
        // FPGA clock
    input wire sysclk,

    // ADC interface
    input wire dout,
    input wire busy,

    output wire cnv,
    output wire sck,
    output wire pio4
);
    wire miso;
    design_1_wrapper bd (
        .cs   (pio1),
        .mosi (pio2),
        .sclk (pio3),
        .miso (miso),
        .busy(busy),
        .dout(dout),
        .cnv(cnv),
        .sck(sck),
        .sysclk(sysclk)
    );
    assign pio4 = miso;

endmodule
