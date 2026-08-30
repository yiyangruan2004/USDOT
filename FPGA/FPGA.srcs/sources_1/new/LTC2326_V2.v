

module LTC2326_V2 
(
    input wire sysclk,
    input wire dout,
    input wire busy,

    output wire cnv,
    output wire sck,

    output wire signed [15:0] adc_sample,
//        // FIFO interface
//    input wire fifo_full,
//    input wire fifo_wr_rst_busy,

    output wire fifo_wr_en
    
);
////////////////////////////////////////////////////
// CNV generator
////////////////////////////////////////////////////


wire adc_data_valid;

cnv_generator_1 #(
    .CLK_FREQ(12_000_000),
    .SAMPLE_FREQ(180_000)
)
cnv_gen
(
    .clk(sysclk),
    .cnv(cnv)
);


wire shift_enable;
wire wr_en;
////////////////////////////////////////////////////
// ADC timing controller
////////////////////////////////////////////////////




//////////////////////////////////////////////////////
//// FIFO signals
//////////////////////////////////////////////////////

//wire [15:0] fifo_dout;
//wire fifo_empty;
//wire fifo_full;

//wire [15:0] fifo_din;

//wire fifo_wr_en;
//wire fifo_rd_en;

//wire fifo_wr_rst_busy;
//wire fifo_rd_rst_busy;



//assign fifo_din = adc_sample;

//////////////////////////////////////////////////////
//// FIFO Generator
//////////////////////////////////////////////////////

//fifo_generator_0 fifo_inst
//(
//    .rst(1'b0),

//    .wr_clk(sysclk),
//    .rd_clk(sysclk),

//    .din(fifo_din),

//    .wr_en(fifo_wr_en),
//    .rd_en(fifo_rd_en),

//    .dout(fifo_dout),

//    .full(fifo_full),
//    .empty(fifo_empty),

//    .wr_rst_busy(fifo_wr_rst_busy),
//    .rd_rst_busy(fifo_rd_rst_busy)
//);


//fifo_controller fifo_ctrl
//(
//    .clk(sysclk),

//    .sample_valid(adc_data_valid),

//    .full(fifo_full),
//    .empty(fifo_empty),

//    .wr_rst_busy(fifo_wr_rst_busy),
//    .rd_rst_busy(fifo_rd_rst_busy),

////    .read_request(fifo_read_request),

//    .wr_en(fifo_wr_en),
//    .rd_en(fifo_rd_en)
//);

adc_timing_controller adc_ctrl
(
    .clk(sysclk),

    .busy(busy),

    .sck(sck),
    .shift_enable(shift_enable),

    .adc_data_valid(adc_data_valid)
);


////////////////////////////////////////////////////
// Data capture
////////////////////////////////////////////////////


adc_shift_register adc_shift
(
    .clk(sysclk),

    .shift_enable(shift_enable),
    .dout(dout),

    .data_valid(adc_data_valid),

    .adc_sample(adc_sample)
);


////////////////////////////////////////////////////
// ILA Debug Core
////////////////////////////////////////////////////
fifo_controller fifo_wr_ctrl
(
    .clk(sysclk),

    .adc_data_valid(adc_data_valid),

//    .fifo_full(fifo_full),
//    .fifo_wr_rst_busy(fifo_wr_rst_busy),

    .fifo_wr_en(fifo_wr_en)
);

endmodule